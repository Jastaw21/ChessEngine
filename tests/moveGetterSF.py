import sys

import chess


def get_moves(fen):
    board = chess.Board(fen)
    moves = [move.uci() for move in board.legal_moves]
    return moves


def perft(board, depth):
    if depth == 0:
        return 1

    nodes = 0
    for move in board.legal_moves:
        board.push(move)
        nodes += perft(board, depth - 1)
        board.pop()
    return nodes


def divide_perft(board, depth):
    result = {}
    for move in board.legal_moves:
        board.push(move)
        count = perft(board, depth - 1)
        board.pop()
        result[move.uci()] = count
    return result


def divide(board, depth):
    if depth == 0:
        return []

    moves = []
    for move in board.legal_moves:
        moves.append(move.uci())
        board.push(move)
        child_moves = divide(board, depth - 1)
        moves.extend(child_moves)
        board.pop()

    return moves


def run_depth_divide(fen, depth):
    board = chess.Board(fen)
    moves = divide(board, depth)
    return moves


def run_divide_perft(fen, depth):
    board = chess.Board(fen)
    result = divide_perft(board, depth)
    return result


def main(argv):
    # usable arguments
    args = argv[1:]

    # arg 1 == fen
    print(args[0])
    # arg 2 = output file
    print(args[1])
    dest_file = args[1]
    # arg 3 = depth
    depth = int(args[2])

    moves = run_divide_perft(args[0], depth)

    with open(dest_file, "w+") as f:
        for move in moves:
            f.write(move + ' ' + str(moves[move]) + "\n")


if __name__ == "__main__":
    main(sys.argv)
