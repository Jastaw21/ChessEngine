//
// Created by jacks on 16/06/2025.
//

#ifndef ENGINEBASE_H
#define ENGINEBASE_H
#include <string>
#include <vector>


class EngineBase {
public:
    virtual ~EngineBase() = default;

    virtual bool moveIsLegal(const std::string &move) = 0;

    virtual bool moveIsPossible(const std::string &move) = 0;

    virtual bool checkMove(const std::string &move) = 0;

    virtual void evaluate() = 0;

    virtual void search() = 0;

private:
};


#endif //ENGINEBASE_H
