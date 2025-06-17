#ifndef MACHINE_PASS_H
#define MACHINE_PASS_H
#include "../basic/machine.h"
class MachinePass {
protected:
    MachineUnit *unit;
    // MachineFunction *current_func;
    // MachineBlock *cur_block;

public:
    virtual void Execute() = 0;
    MachinePass(MachineUnit *unit) : unit(unit) {}
};
#endif