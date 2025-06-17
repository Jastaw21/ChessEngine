//
// Created by jacks on 16/06/2025.
//

#ifndef TESTENGINE_H
#define TESTENGINE_H
#include "EngineBase.h"


class TestEngine final : public EngineBase{

    public:
    virtual bool moveIsLegal(const std::string &move) override;
    virtual bool moveIsPossible(const std::string &move) override;
    virtual bool checkMove(const std::string &move) override;
    virtual void evaluate() override;
    virtual void search() override;

};



#endif //TESTENGINE_H
