//
// Created by jacks on 16/06/2025.
//

#ifndef ENGINEBASE_H
#define ENGINEBASE_H

class EngineBase {
public:

    EngineBase();
    virtual ~EngineBase() = default;
    virtual void evaluate() = 0;
    virtual void search() = 0;
};


#endif //ENGINEBASE_H
