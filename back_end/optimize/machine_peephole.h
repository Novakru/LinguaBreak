#ifndef MACHINEPEEPHOLE_H
#define MACHINEPEEPHOLE_H
#include "MachinePass.h"
class MachinePeephole : MachinePass {

public:
    MachinePeephole(MachineUnit *unit) : MachinePass(unit) {}
    void Execute();
};
#endif