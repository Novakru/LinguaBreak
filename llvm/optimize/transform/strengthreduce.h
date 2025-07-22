#ifndef STRENGTHREDUCE_H
#define STRENGTHREDUCE_H
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