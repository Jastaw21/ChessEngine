//
// Created by jacks on 16/06/2025.
//

#ifndef TESTENGINE_H
#define TESTENGINE_H
#include "EngineBase.h"


class TestEngine final : public EngineBase {
public:
    TestEngine();

    void evaluate() override;

    void search() override;
};


#endif //TESTENGINE_H
