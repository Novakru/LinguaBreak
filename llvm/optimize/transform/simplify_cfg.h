#ifndef SIMPLIFY_CFG_H
#define SIMPLIFY_CFG_H
#include "../../include/ir.h"
#include "../pass.h"
#include "../analysis/dominator_tree.h"

class SimplifyCFGPass : public IRPass {
private:
    void EliminateUnreachedBlocksInsts(CFG *C);
    void EliminateOneBrUncondBlocks(CFG *C);
	bool IsSafeToEliminate(CFG *C, LLVMBlock block, LLVMBlock pred_block, LLVMBlock nextbb);
    void EliminateOnePredPhi(CFG *C,LLVMBlock nowblock,std::unordered_set<int> regno_tobedeleted);
    void EliminateNotExistPhi(CFG *C);
public:
    SimplifyCFGPass(LLVMIR *IR) : IRPass(IR){}
    void Execute();//消除不可达块
    void EOBB(); //消除只有一条无条件跳转指令的基本块
    void EOPPhi();//消除无用Phi指令
    void RebuildCFG();
};

#endif