#ifndef LOOPROTATE_H
#define LOOPROTATE_H
#include "../../include/ir.h"
#include "../pass.h"

#include "../analysis/dominator_tree.h"

class LoopRotate : public IRPass { 
private:

public:
    LoopRotate(LLVMIR *IR) : IRPass(IR) {}
    void Execute();
};

#endif