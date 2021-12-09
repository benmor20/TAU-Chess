import chess
import chess.engine
import chess.svg
from hardware import drive_system as drive
from hardware import serial_protocol as serial
from hardware.devices import DigitalOutput
from time import sleep


def move_from_uci(drive_sys, magnet, move):
    if len(move) == 4:
        old_rank = ord(move[0]) - ord('a')
        old_file = int(move[1])

        new_rank = ord(move[2]) - ord('a')
        new_file = int(move[3])

        drive_sys.move_to((old_rank, old_file))
        sleep(2)
        magnet.turn_on()
        drive_sys.move((0.5, 0))
        drive_sys.move_to((new_rank, new_file))
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
    s = serial.Serial()
    drive_sys = drive.ChessDrive(s)
    magnet = DigitalOutput(s, 4)

    # user plays chess against engine
    while not board.is_game_over():
        player_move = input('Please enter a valid move or type stop if you would like to quit: ')
        if player_move == 'stop':
            engine.quit()
            print('Game ended.')
            break
        else:
            # makes player's move on the board
            move = chess.Move.from_uci(player_move)
            board.push(move)
            # interface with arduino to move pieces
            move_from_uci(drive_sys, magnet, move)
            # print board after player move is made
            print(board)
            print('\n')

            # makes engine's move on the board
            result = engine.play(board, chess.engine.Limit(time=0.1))
            board.push(result.move)
            # interface with arduino to move pieces
            move_from_uci(drive_sys, magnet, result.move)
            # print board after engine move is made
            print(board)
            print('\n')
    engine.quit()


if __name__ == '__main__':
    main()
