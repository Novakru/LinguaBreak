#ifndef GLOBALOPT_H
#define GLOBALOPT_H
#include "../../include/ir.h"
#include "../pass.h"
#include "../analysis/AliasAnalysis.h"

extern AliasAnalysisPass AA; 

class GlobalOptPass : public IRPass { 
private:
	

public:
    GlobalOptPass(LLVMIR *IR) : IRPass(IR) {}
    void Execute();
};

#endif
/*
global array
    - no-read-write:         delete
    - read-only:             几乎没有
    - write-only:            delete


global value
    - no-read-write:         delete
    - read-only:             inline+delete
    - write-only:            delete
    - read-write:                  单函数: mem2reg        多函数：尽可能mem2reg+函数间保持mem
*/
//不对是否exec做判断，默认都是exec的