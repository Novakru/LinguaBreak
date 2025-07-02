#ifndef ADCE_H
#define ADCE_H
#include "../../include/ir.h"
#include "../pass.h"

class ADCEPass : public IRPass { 
private:
    DomAnalysis *domtrees;
    std::set<Instruction> live;
    std::set<int> live_block;

public:
    ADCEPass(LLVMIR *IR, DomAnalysis *dom) : IRPass(IR) { domtrees = dom; }
    Instruction GetTerminal(CFG *C, int label);
    void ADCE(CFG *C);
    void CleanUnlive(CFG *C);
    void Execute();
};

#endif