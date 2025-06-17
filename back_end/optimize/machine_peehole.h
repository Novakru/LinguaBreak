#ifndef MACHINE_PEEHOLE_H
#define MACHINE_PEEHOLE_H
#include "MachinePass.h"
class Machine_Peehole:MachinePass {


public:
    void Execute() ;
    Machine_Peehole(MachineUnit *unit) : MachinePass(unit) {}
};
#endif