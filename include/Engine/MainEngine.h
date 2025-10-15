//
// Created by jacks on 16/06/2025.
//

#ifndef TESTENGINE_H
#define TESTENGINE_H
#include <vector>

#include "EngineBase.h"
#include "Evaluation.h"

class MainEngine final : public EngineBase {
public:

    explicit MainEngine() : EngineBase(){}

    virtual bool sendCommand(const std::string& command) override;
    virtual std::string readResponse() override;
};


#endif //TESTENGINE_H