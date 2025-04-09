#ifndef PEEPHOLE_H
#define PEEPHOLE_H

#include "../../include/ir.h"
#include "../pass.h"

class PeepholePass : public IRPass {
private:

public:
    PeepholePass(LLVMIR *IR) : IRPass(IR) {}
	void Execute();
    void SrcEqResultInstEliminateExecute();
    void ImmResultReplaceExecute();
	void DeadArgElim();
};

#endif