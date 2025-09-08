import chess


def get_moves(fen):
    board = chess.Board(fen)
    moves = [move.uci() for move in board.legal_moves]
    return moves


class PerftResult:
    nodes = 0
    captures = 0
    en_passant = 0
    castling = 0
    check = 0
    checkmate = 0
    promotion = 0

    def __add__(self, other):
        result = PerftResult()
        result.nodes = self.nodes + other.nodes
        result.captures = self.captures + other.captures
        result.en_passant = self.en_passant + other.en_passant
        result.castling = self.castling + other.castling
        result.check = self.check + other.check
        result.checkmate = self.checkmate + other.checkmate
        result.promotion = self.promotion + other.promotion
        return result

    def __repr__(self):
        alignment = 12
        return f"{str(self.nodes):^{alignment}} {str(self.captures):^{alignment}} {str(self.en_passant):^{alignment}} {str(self.castling):^{alignment}} {str(self.check):^{alignment}} {str(self.checkmate):^{alignment}} {str(self.promotion):^{alignment}}"


def perft(board, depth):
    if depth == 0:
        return 1

    nodes = 0
    for move in board.legal_moves:
        board.push(move)
        nodes += perft(board, depth - 1)
        board.pop()
    return nodes


def perft_complex(board, depth):
    if depth == 0:
        result = PerftResult()
        result.nodes = 1
        return result

    perft_result = PerftResult()

    for move in board.legal_moves:

        is_cap = board.is_capture(move)
        is_ep = board.is_en_passant(move)
        is_castle = board.is_castling(move)
        is_check = board.gives_check(move)
        board.push(move)
        is_checkmate = board.is_checkmate()
        is_promote = len(move.uci()) == 5

        if depth == 1:
            perft_result.nodes += 1
            if is_check:
                perft_result.check += 1
            if is_checkmate:
                perft_result.checkmate += 1
            if is_ep:
                perft_result.en_passant += 1
                perft_result.captures += 1
            elif is_cap:
                perft_result.captures += 1
            elif is_castle:
                perft_result.castling += 1
            if is_promote:
                perft_result.promotion += 1


        else:
            perft_result += perft_complex(board, depth - 1)

        board.pop()
    return perft_result


def divide_perft(board, depth):
    result = {}

    for move in board.legal_moves:
        count = PerftResult()
        board.push(move)
        count += perft_complex(board, depth - 1)
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
    dest_file = args[1]
    # arg 3 = depth
    depth = int(args[2])

    moves = run_divide_perft(args[0], depth)

    with open(dest_file, "w+") as f:
        for move in moves:
            f.write(move
                    + ' ' + str(moves[move].nodes) + ' ' + str(moves[move].captures)
                    + ' ' + str(moves[move].en_passant) + ' ' + str(moves[move].castling)
                    + ' ' + str(moves[move].check) + ' ' + str(moves[move].checkmate) + "\n")


# move n c ep ca ch chm

if __name__ == "__main__":
    board = chess.Board("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8")

    alignment = 12
    print(
        f"{'depth':-^6} {'nodes':-^12} {'captures':-^12} {'en-passant':-^12} {'captures':-^12} {'castling':-^12} {'check':-^12} {'checkmate':-^12} {'promotion':-^12}")
    for depth in range(1, 4):
        print(f"{depth:^{6}}", perft_complex(board, depth))
