from pathlib import Path

import chess.pgn


def get_game_log_moves(game_log_path: str) -> list[str]:
    path_object = Path(game_log_path)

    with open(path_object) as file:
        move_lines = [line for line in file]

    move_strings = []
    for index, line in enumerate(move_lines):
        if index == 0:
            continue
        else:
            last_space_start = line.rfind(" ")
            raw_string = line[last_space_start + 1:]
            result_string = raw_string.rstrip()
            lowered_string = result_string.lower()
            move_strings.append(lowered_string)

    result = [chess.Move.from_uci(move) for move in move_strings]
    return result


def main():
    game = chess.pgn.Game()

    listOfMoves = get_game_log_moves(r"C:\Users\jacks\source\repos\Chess\cmake-build-debug-visual-studio\gameLog.txt")
    node = game.add_line(listOfMoves)

    print(game)


main()
