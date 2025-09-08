import chess

board = chess.Board("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1")

for move in board.legal_moves:
    print(move)
    board.push(move)
    for followup in board.legal_moves:
        print("after ", move, " followup ", followup)
    board.pop()
