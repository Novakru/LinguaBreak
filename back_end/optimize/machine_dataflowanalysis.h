#ifndef MACHINE_DATAFLOWANALYSIS_H
#define MACHINE_DATAFLOWANALYSIS_H
#include "MachinePass.h"


class DataFlowAnalysisPass : MachinePass {
    bool ConstPropagation(MachineCFG* cfg);
public:
    DataFlowAnalysisPass(MachineUnit *unit) : MachinePass(unit) {}
    void ConstPropagationExecute();
    void Execute();
};

std::pair<bool,int> getConstVal(Register reg, MachineCFG* cfg);
std::pair<bool,float> getFloatConstVal(Register reg, MachineCFG* cfg);
#endif