#ifndef LOOP_UNROLL_PASS_H
#define LOOP_UNROLL_PASS_H

#include "../pass.h"
#include "../analysis/ScalarEvolution.h"
#include "../analysis/loop.h"
#include "../../include/cfg.h"
#include <functional>

class LoopUnrollPass : public IRPass {
public:
    LoopUnrollPass(LLVMIR* IR) : IRPass(IR) {}
    void Execute() override;

private:
    // 常量循环完全展开
    void ConstantLoopFullyUnroll(CFG *C);
    bool ConstantLoopFullyUnrollLoop(CFG *C, Loop *L);
    
    // 简单for循环部分展开
    void SimpleForLoopUnroll(CFG *C);
    bool SimpleForLoopUnrollLoop(CFG *C, Loop *L);
    
    // 辅助函数
    static bool IsLoopEnd(int i, int ub, BasicInstruction::IcmpCond cond);
    void processLoopRecursively(CFG *C, Loop *L, 
                               std::function<bool(CFG*, Loop*)> processFunc);
    
    // 循环分析辅助函数
    bool isSimpleForLoop(CFG *C, Loop *L);
    bool getLoopBounds(CFG *C, Loop *L, int &lb, int &ub, int &step, BasicInstruction::IcmpCond &cond);
    int estimateLoopIterations(int lb, int ub, int step, BasicInstruction::IcmpCond cond);
    
    // 循环展开实现函数
    bool performConstantLoopUnroll(CFG *C, Loop *L, int lb, int ub, int step, BasicInstruction::IcmpCond cond);
    bool performSimpleLoopUnroll(CFG *C, Loop *L, int unroll_times);
    
    // 控制流修改函数
    void modifyControlFlowForUnroll(CFG *C, LLVMBlock old_exiting, LLVMBlock exit, 
                                   LLVMBlock old_latch, LLVMBlock new_header, 
                                   LLVMBlock old_preheader, LLVMBlock new_preheader);
    void finalizeLoopUnroll(CFG *C, LLVMBlock old_header, LLVMBlock old_exiting, 
                           LLVMBlock exit, LLVMBlock old_latch);
    
    // 部分展开相关函数
    void createRemainLoop(CFG *C, Loop *L, std::set<LLVMBlock>& remain_loop_nodes,
                         LLVMBlock& remain_header, LLVMBlock& remain_latch,
                         LLVMBlock& remain_exiting, std::map<int, int>& RemainRegReplaceMap,
                         std::map<int, int>& RemainLabelReplaceMap);
    void modifyMainLoopBoundary(CFG *C, LLVMBlock exiting, int unroll_times);
    void createConditionalBlock(CFG *C, LLVMBlock preheader, LLVMBlock CondBlock,
                               LLVMBlock npreheader, LLVMBlock remain_header,
                               LLVMBlock header, int unroll_times);
    void expandMainLoop(CFG *C, Loop *L, int unroll_times, LLVMBlock exit);
    void connectRemainLoop(CFG *C, LLVMBlock exiting, LLVMBlock exit,
                          LLVMBlock remain_header, LLVMBlock mid_exit);
};

#endif // LOOP_UNROLL_PASS_H 