import sys

import chess


def get_move_result(board, move):
    results = set()  # Using a set to avoid duplicates

    if board.is_capture(move):
        results.add("capture")
    elif board.is_castling(move):
        results.add("castling")
    elif board.is_en_passant(move):
        results.add("en_passant")
    else:
        results.add("push")

    if board.gives_check(move):
        results.add("check")

    return "+".join(sorted(results))  # Sort for consistent output


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
