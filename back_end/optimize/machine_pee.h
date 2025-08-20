#ifndef MACHINE_PEE_H
#define MACHINE_PEE_H
#include "MachinePass.h"
#include "machine_dataflowanalysis.h"
class MachinePeePass : MachinePass {
    void ConstComputingEliminate(MachineCFG* cfg);  //恒等运算，如x=x+0, x=x*1
    bool RedundantReplacementEliminate(MachineCFG* cfg); //冗余替换，如x=y+0,直接用y取代x的use
    void ConstantFolding(MachineCFG* cfg);//常量折叠
    bool ReplaceReg(Register reg, MachineBlock*block,MachineCFG* cfg);
    void DCE(MachineCFG* cfg); //删除无用指令
public:
    MachinePeePass(MachineUnit *unit) : MachinePass(unit) {}
    void FloatCompFusion();//use FMA
    void Execute();
};
#endif
/*
TODO:对于multi_def的，或许可以进行块内的...

*/