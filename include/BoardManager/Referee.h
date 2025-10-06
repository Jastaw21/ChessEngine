//
// Created by jacks on 06/10/2025.
//

#ifndef CHESS_REFEREE_H
#define CHESS_REFEREE_H


struct Move;
class BitBoards;

class Referee {
public:

    Referee();

    bool moveIsLegal(Move& move, BitBoards& boardState);

private:

    bool validateMove(Move& move, BitBoards& boardState);
    bool validateMovePostMaking(Move& move, BitBoards& boardState);
};


#endif //CHESS_REFEREE_H