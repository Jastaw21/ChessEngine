import sys

import chess


def get_move_result(board, move):
    resultBits = 0
    is_cap = board.is_capture(move)
    is_ep = board.is_en_passant(move)
    is_castle = board.is_castling(move)
    is_check = board.gives_check(move)
    board.push(move)
    is_checkmate = board.is_checkmate()
    is_promote = len(move.uci()) == 5
    board.pop()

    if is_check:
        resultBits |= 1 << 4
    if is_cap:
        resultBits |= 1 << 1
    if is_ep:
        resultBits |= 1 << 2
    if is_promote:
        resultBits |= 1 << 6
    if is_castle:
        resultBits |= 1 << 3
    if is_checkmate:
        resultBits |= 1 << 7

    if not (is_checkmate or is_check or is_castle or is_ep or is_promote or is_cap):
        resultBits |= 1

    return resultBits


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
            f.write(move[0] + ' ' + str(move[1]) + "\n")


if __name__ == "__main__":
    main(sys.argv)
