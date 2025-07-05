import sys

import chess


def get_moves(fen):
    board = chess.Board(fen)
    moves = [move.uci() for move in board.legal_moves]
    return moves


def main(argv):
    # usable arguments
    args = argv[1:]

    # arg 1 == fen
    print(args[0])
    moves = get_moves(args[0])

    # arg 2 = output file
    print(args[1])
    dest_file = args[1]

    with open(dest_file, "w+") as f:
        for move in moves:
            f.write(move + "\n")


if __name__ == "__main__":
    main(sys.argv)
