from pathlib import Path
import chess

def get_lengths(games):
    lengthMultipleDict = {}    
    for game in games:
        rawLengths = len(game)
        isMulOf4 = rawLengths % 4 == 0
        if not (isMulOf4):
            print(game, "Not 4 mul")        
       
        else:
            moves = rawLengths/4
            if (existingrecord:= lengthMultipleDict.get(moves)):
                lengthMultipleDict[moves] = lengthMultipleDict[moves] +1
            else:
                lengthMultipleDict[moves] = 1
    
    return lengthMultipleDict

def get_move_strings(games):
    move_strings = []

    for game in games:
        thisGame = []
        startIndex = 0
        endIndex = 3

        while (endIndex < len(game)):
            move_string = game[startIndex:endIndex+1]
            startIndex += 4
            endIndex += 4

            thisGame.append(move_string)
        
        move_strings.append(thisGame)
    
    return move_strings

def get_raw_games():
 
    here = Path(__file__).resolve().parent
    bok = here / "Amundsen.bok"
    with  bok.open("r", encoding="utf-8") as file:
       return [line.rstrip("\n") for line in file]

def get_games():
    return get_move_strings(get_raw_games())

def get_fen_after_game(game):
    board = chess.Board()

    for move in game:
        board.push_uci(move)

    return board.fen()


games = get_games()

for game in games:
    print(get_fen_after_game(game))