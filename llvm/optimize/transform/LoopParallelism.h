#ifndef LOOP_PARALLELISM_PASS_H
#define LOOP_PARALLELISM_PASS_H

#include "../pass.h"
#include "../analysis/ScalarEvolution.h"
#include "../analysis/loop.h"
#include "../../include/cfg.h"
#include <functional>
#include <vector>
#include <map>
#include <string>

class LoopParallelismPass : public IRPass {
public:
    LoopParallelismPass(LLVMIR* IR) : IRPass(IR) {}
    void Execute() override;

private:
    // 主要处理函数
    void ProcessFunction(CFG* cfg);
    void ProcessLoop(Loop* loop, CFG* cfg, ScalarEvolution* SE);
    
    // 循环可并行化性检查
    bool CanParallelizeLoop(Loop* loop, CFG* cfg, ScalarEvolution* SE);
    bool IsSimpleLoop(Loop* loop, CFG* cfg);
    bool HasNoLoopCarriedDependencies(Loop* loop, CFG* cfg);
    bool IsConstantIterationCount(Loop* loop, CFG* cfg, ScalarEvolution* SE);
    
    // 循环体提取和函数生成
    void ExtractLoopBodyToFunction(Loop* loop, CFG* cfg, ScalarEvolution* SE);
    std::string GenerateFunctionName(CFG* cfg, Loop* loop);
    void CreateParallelFunction(Loop* loop, CFG* cfg, const std::string& func_name, ScalarEvolution* SE);
    void CreateParallelCall(Loop* loop, CFG* cfg, const std::string& func_name, ScalarEvolution* SE);
    
    // 循环体指令处理
    void CopyLoopBodyInstructions(Loop* loop, CFG* cfg, LLVMBlock& new_func_block, 
                                 std::map<int, int>& reg_mapping);
    Instruction CloneInstruction(Instruction inst, CFG* cfg, std::map<int, int>& reg_mapping);
    
    // 参数和返回值处理
    void AddFunctionParameters(LLVMBlock& func_block, CFG* cfg);
    void AddThreadRangeCalculation(LLVMBlock& func_block, CFG* cfg);
    
    // 辅助函数
    std::string GenerateUniqueName(const std::string& base_name);
    
    // 运行时库函数声明
    void AddRuntimeLibraryDeclarations(CFG* cfg);
    
    // 分支结构生成
    void CreateConditionalParallelization(Loop* loop, CFG* cfg, const std::string& func_name, 
                                         ScalarEvolution* SE);
    
    // 全局变量
    std::map<std::string, int> name_counter;
};

#endif // LOOP_PARALLELISM_PASS_H
