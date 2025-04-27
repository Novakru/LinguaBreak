#ifndef WEAKEN_MEM2REG_H
#define WEAKEN_MEM2REG_H
#include "../../include/ir.h"
#include "../pass.h"

class WeakenMem2RegPass : public IRPass {
private:
    void EliminateNoUseAlloca(CFG *C);
    void EliminateUseDefSameBlockLoadStore(CFG *C);
    void WeakenMem2Reg(CFG *C);
public:
    WeakenMem2RegPass(LLVMIR *IR) : IRPass(IR) {}
    void Execute();
};
#endif