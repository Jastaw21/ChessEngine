//
// Created by jacks on 16/06/2025.
//

#ifndef TESTENGINE_H
#define TESTENGINE_H
#include "EngineBase.h"


class TestEngine final : public EngineBase {
public:

    explicit TestEngine(Colours colour);

    virtual void evaluate() override;
    virtual void search() override;
};


#endif //TESTENGINE_H
