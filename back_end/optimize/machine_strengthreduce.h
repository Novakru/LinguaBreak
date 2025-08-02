#ifndef MACHINESTRENGTHREDUCE_H
#define MACHINESTRENGTHREDUCE_H
#include "MachinePass.h"
class MachineStrengthReducePass : MachinePass {
    // 标量强度削弱
    void ScalarStrengthReduction() ;
public:
    MachineStrengthReducePass(MachineUnit *unit) : MachinePass(unit) {}
    void Execute();
};
#endif