import sys

import chess


def get_move_result(board, move):
    capture = board.is_capture(move)
    castling = board.is_castling(move)
    en_passant = board.is_en_passant(move)

    if capture:
        result = "capture"
    elif castling:
        result = "castling"
    elif en_passant:
        result = "en_passant"
    else:
        result = "push"
    return result


def get_moves(fen):
    board = chess.Board(fen)
    moves = [move.uci() for move in board.legal_moves]
    return moves


def get_move_results(fen):
    board = chess.Board(fen)

    moves_and_results = []
    for move in board.legal_moves:
        result = get_move_result(board, move)
        moves_and_results.append((move.uci(), result))
    return moves_and_results


def main(argv):
    # usable arguments
    args = argv[1:]

    # arg 1 == fen
    print(args[0])
    moves = get_move_results(args[0])

    # arg 2 = output file
    print(args[1])
    dest_file = args[1]

    with open(dest_file, "w+") as f:
        for move in moves:
            f.write(move[0] + ' ' + move[1] + "\n")


if __name__ == "__main__":
    main(sys.argv)
