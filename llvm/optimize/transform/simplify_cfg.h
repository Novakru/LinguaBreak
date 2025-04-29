#ifndef SIMPLIFY_CFG_H
#define SIMPLIFY_CFG_H
#include "../../include/ir.h"
#include "../pass.h"

class SimplifyCFGPass : public IRPass {
private:
    void EliminateUnreachedBlocksInsts(CFG *C);
public:
    SimplifyCFGPass(LLVMIR *IR) : IRPass(IR) {}
    void Execute();
};

#endif