#ifndef REASSOCIATE_H
#define REASSOCIATE_H

#include "../../include/ir.h"
#include "../pass.h"

class ReassociatePass : public IRPass {
private:
    
public:
    ReassociatePass(LLVMIR *IR) : IRPass(IR) {}
	void Execute();

};

#endif