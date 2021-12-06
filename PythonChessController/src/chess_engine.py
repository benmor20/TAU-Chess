import chess
import chess.engine
import chess.svg
from hardware import drive_system as drive
from hardware import serial_protocol as serial


# create chess board
board = chess.Board()

# download stockfish chess engine
engine = chess.engine.SimpleEngine.popen_uci("PythonChessController\src\stockfish_14.1_win_x64_avx2.exe")

# display the board at starting position
print(board)
print("\n")

# Create arduino interfacing
s = serial.Serial()
drive_sys = drive.ChessDrive(s)


def move_from_uci(drive_sys, move):
    if(move.length() == 4):
        old_rank = move[0:1]
        old_file = move[1:2]

        new_rank = move[2:3]
        new_file = move[3:4]

        x_squares = ord(new_rank) - ord(old_rank)
        y_squares = ord(new_file) - ord(old_file)

        drive_sys.move([x_squares, y_squares, 0])
    else:
        print("Please enter a start and end square.")


# user plays chess against engine
while not board.is_game_over():
    player_move = input("Please enter a valid move or type stop if you would like to quit: ")
    if(player_move == "stop"):
        engine.quit()
        print("Game ended.")
        break
    else:
        # makes player's move on the board
        move = chess.Move.from_uci(player_move)
        board.push(move)
        # interface with arduino to move pieces
        move_from_uci(drive_sys, move)
        # print board after player move is made
        print(board)
        print("\n")

        # makes engine's move on the board
        result = engine.play(board, chess.engine.Limit(time=0.1))
        board.push(result.move)
        # interface with arduino to move pieces
        move_from_uci(drive_sys, result.move)
        # print board after engine move is made
        print(board)
        print("\n")

engine.quit()

