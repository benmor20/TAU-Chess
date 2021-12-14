import chess
import chess.engine
import chess.svg
from hardware.drive_system import ChessDrive
from hardware.serial_protocol import Serial
from hardware.devices import *
from time import sleep


def move_from_uci(drive_sys, magnet, move, capture=False):
    if len(move) == 4:
        old_rank = ord(move[0]) - ord('a')
        old_file = int(move[1]) - 1

        new_rank = ord(move[2]) - ord('a')
        new_file = int(move[3]) - 1

        print(f'Moving to piece at {old_file, old_rank}')
        drive_sys.move_to((old_file, old_rank))
        drive_sys.update()
        print('Moving over')
        magnet.turn_on()
        drive_sys.move((0, 0.5))
        drive_sys.update()
        print(f'Moving to location {new_file, new_rank}')
        drive_sys.move_to((new_file - 0.5, new_rank))
        drive_sys.update()
        drive_sys.move((0.5, 0))
        drive_sys.update()
        magnet.turn_off()
    else:
        print('Please enter a start and end square.')


def main():
    # create chess board
    board = chess.Board()

    # download stockfish chess engine
    engine = chess.engine.SimpleEngine.popen_uci('PythonChessController\src\stockfish_14.1_win_x64_avx2.exe')

    # display the board at starting position
    print(board)
    print('\n')

    # Create arduino interfacing
    s = Serial('COM7')
    drive_sys = ChessDrive(Serial('COM13'))
    magnet = DigitalOutput(s, 8)
    moves = MoveSensor(s, 'A0')

    # user plays chess against engine
    while not board.is_game_over():
        player_move = None
        while player_move is None:
            player_move = moves.value
            moves.reset()
        # makes player's move on the board
        move = chess.Move.from_uci(player_move)
        board.push(move)
        # print board after player move is made
        print(board)
        print('\n')

        # makes engine's move on the board
        result = engine.play(board, chess.engine.Limit(time=0.1))
        # interface with arduino to move pieces
        move_from_uci(drive_sys, magnet, result.move.uci(), capture=board.is_capture(result.move))
        board.push(result.move)
        # print board after engine move is made
        print(board)
        print('\n')
    engine.quit()


if __name__ == '__main__':
    main()
