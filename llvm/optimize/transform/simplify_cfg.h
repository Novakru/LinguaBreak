#ifndef SIMPLIFY_CFG_H
#define SIMPLIFY_CFG_H
#include "../../include/ir.h"
#include "../pass.h"
#include <stack>

class SimplifyCFGPass : public IRPass {
private:
    // TODO():添加更多你需要的成员变量
    void EliminateUnreachedBlocksInsts(CFG *C);
    void EliminateUselessPhi(CFG *C);
    void EliminateDoubleUnBrUnCond(CFG *C);

public:
    SimplifyCFGPass(LLVMIR *IR) : IRPass(IR) {}
    void Execute();
};

#endif