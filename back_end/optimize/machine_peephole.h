#ifndef MACHINEPEEPHOLE_H
#define MACHINEPEEPHOLE_H
#include "MachinePass.h"
class MachinePeephole : MachinePass {
    // 删除冗余指令 - add 0/ mul 1/ sub 0/ div 1 
    void EliminateRedundantInstructions() ;
    // 浮点指令合并 - fmul+fadd / fmul+fsub --> fma/fnma
    void FloatCompFusion();
public:
    MachinePeephole(MachineUnit *unit) : MachinePass(unit) {}
    void Execute();
};
#endif