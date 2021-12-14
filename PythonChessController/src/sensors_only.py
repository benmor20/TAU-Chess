import chess
import chess.engine
import chess.svg
from src.hardware.serial_protocol import Serial
from src.hardware.devices import MoveSensor
from time import sleep
import os


def wait_for_move(move_sensor: MoveSensor, target_move: str):
    while True:
        move = None
        while move is None:
            move = move_sensor.value
            move_sensor.reset()
            sleep(0.1)
        return
        # if move == target_move:
        #     return
        # print(f'Please undo {move} and play {target_move}')
        # wait_for_move(move_sensor, move[2:] + move[:2])


def get_player_move(move_sensor: MoveSensor, board: chess.Board) -> chess.Move:
    while True:
        player_move = None
        while player_move is None:
            player_move = move_sensor.value
            move_sensor.reset()
            sleep(0.1)
        move = chess.Move.from_uci(player_move)
        if board.is_legal(move):
            return move
        print(f'{player_move} is not a valid move. Please reset.')
        wait_for_move(move_sensor, player_move[2:] + player_move[:2])


def main():
    serial = Serial('COM7')
    move_sensor = MoveSensor(serial, 'A0')
    move_sensor.wait_for_setup()

    board = chess.Board()
    print(os.getcwd())
    engine = chess.engine.SimpleEngine.popen_uci('stockfish_14.1_win_x64_avx2.exe')
    print(board, end='\n\n')

    while not board.is_game_over():
        print('Your move.')
        player_move = get_player_move(move_sensor, board)
        board.push(player_move)
        print(board, end='\n\n')

        ai_move = engine.play(board, chess.engine.Limit(time=0.1))
        print(f'AI plays {ai_move.move.uci()} (please replicate on the board)')
        board.push(ai_move.move)
        print(board, end='\n\n')
        wait_for_move(move_sensor, ai_move.move.uci())


if __name__ == '__main__':
    main()
