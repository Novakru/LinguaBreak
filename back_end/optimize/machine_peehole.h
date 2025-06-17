#ifndef MACHINE_PEEHOLE_H
#define MACHINE_PEEHOLE_H
#include "MachinePass.h"
class Machine_Peehole:MachinePass {


public:
    Machine_Peehole(MachineUnit *unit) : MachinePass(unit) {}
    void Execute() ;
};
#endif