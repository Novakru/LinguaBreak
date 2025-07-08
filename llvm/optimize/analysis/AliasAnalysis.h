#ifndef ALIASANALYSIS_H
#define ALIASANALYSIS_H
#include "../../include/ir.h"
#include "../pass.h"

class AliasAnalysisPass : public IRPass { 
private:

public:
    AliasAnalysisPass(LLVMIR *IR) : IRPass(IR) {}
    void Execute();
};

#endif