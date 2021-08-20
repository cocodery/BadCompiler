//
// Created by Administrator on 2021/7/22.
//

#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include "common.h"

class Optimizer {
public:
    vector<BasicBlock> Block;
public:
    Optimizer(vector<Quaternary>&);
    ~Optimizer() { };
    void showBlocks();
    vector<BasicBlock>& RetBlock();
private:
    void DivideBlock(vector<Quaternary>&);
    void UselessCalculation(BasicBlock&);
    void assignAc(Map&, Map&, Operand&, bool);
    void Activity(BasicBlock&);
};

#endif //OPTIMIZATION_H
