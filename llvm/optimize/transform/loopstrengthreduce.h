#ifndef LOOPSTRENGTHREDUCE_H
#define LOOPSTRENGTHREDUCE_H
#include "../../include/ir.h"
#include "../pass.h"

class StrengthReducePass : public IRPass { 
private:
    void ScalarReduce();

public:
    StrengthReducePass(LLVMIR *IR) : IRPass(IR) {}
    void Execute();
};

#endif