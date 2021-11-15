import chess
import chess.engine
import chess.svg

# create chess board
board = chess.Board()

# download stockfish chess engine
engine = chess.engine.SimpleEngine.popen_uci(r"C:\Users\gblake\OneDrive\Downloads\stockfish_14.1_win_x64_avx2\stockfish_14.1_win_x64_avx2.exe")

# display the board at starting position
print(board)
print("\n")

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
        # print board after player move is made
        print(board)
        print("\n")

        # makes engine's move on the board
        result = engine.play(board, chess.engine.Limit(time=0.1))
        board.push(result.move)
        # print board after engine move is made
        print(board)
        print("\n")

engine.quit()

