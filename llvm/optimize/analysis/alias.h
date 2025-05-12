#ifndef ALIAS_H
#define ALIAS_H
#include "../pass.h"
#include "../../include/ir.h"

class AliasAnalysis : public IRPass {
    private:

    
    public:
        AliasAnalysis(LLVMIR *IR) : IRPass(IR) {}
        void Execute();
};
#endif