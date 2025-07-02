#ifndef ONERET_H
#define ONERET_H
#include "../../include/ir.h"
#include "../pass.h"

class OneRetPass : public IRPass { 
private:
	
public:
    OneRetPass(LLVMIR *IR) : IRPass(IR) {}
    void Execute();
};

#endif