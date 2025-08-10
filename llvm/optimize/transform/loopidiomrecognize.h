#ifndef LOOP_IDIOM_RECOGNIZE_PASS_H
#define LOOP_IDIOM_RECOGNIZE_PASS_H

#include "../pass.h"
#include "../analysis/ScalarEvolution.h"
#include "../analysis/loop.h"
#include "../../include/cfg.h"
#include <functional>
#include <vector>
#include <map>
#include <set>

/**
 * 循环习语识别Pass
 * 
 * 主要识别和优化的循环习语：
 * 1. memset习语：for(int i = 0; i <= n; i++) a[i] = const; -> memset(a, const, size)
 * 2. 算术级数求和：sum = 0; for(int i = 1; i <= const; i++) { sum += i; } -> sum = const*(const+1)/2
 * 3. 模运算习语：sum = 0; for(int i = 1; i <= const; i++) { sum = (sum + i) % p; } -> 直接计算
 * 4. 乘法习语：prod = 1; for(int i = 1; i <= const; i++) { prod *= i; } -> 阶乘计算
 * 5. 异或习语：xor_sum = 0; for(int i = 1; i <= const; i++) { xor_sum ^= i; } -> 直接计算
 */

// 使用ScalarEvolution中定义的结构体
using LoopParams = ScalarEvolution::LoopParams;
using GepParams = ScalarEvolution::GepParams;
using HoistingCandidate = ScalarEvolution::HoistingCandidate;

class LoopIdiomRecognizePass : public IRPass {
public:
    LoopIdiomRecognizePass(LLVMIR* IR) : IRPass(IR) {}
    void Execute() override;

private:
    void processFunction(CFG* C);
    void processLoop(Loop* loop, CFG* C, ScalarEvolution* SE);
    
    // recognize idiom
    bool canRecognizeMemsetIdiom(Loop* loop, CFG* C, ScalarEvolution* SE);
    bool recognizeMemsetIdiom(Loop* loop, CFG* C, ScalarEvolution* SE, const std::set<Operand>& hoistedVariables);
    
    // hoist like sum = 0; for(int i = 1; i <= n; i++) { sum += i; } -> sum = n*(n+1)/2
	// scev: {0,+,{0,+,1}}
    bool recognizeLoopHoisting(Loop* loop, CFG* C, ScalarEvolution* SE, std::set<Operand>& hoistedVariables);
    std::vector<HoistingCandidate> findHoistingCandidates(Loop* loop, CFG* C, ScalarEvolution* SE);
    bool canCalculateFinalValue(const HoistingCandidate& candidate, Loop* loop, CFG* C, ScalarEvolution* SE);
    bool canCalculateNestedRecursion(SCEVAddRecExpr* addrec, Loop* loop, CFG* C, ScalarEvolution* SE);
    int calculateFinalValue(const HoistingCandidate& candidate, const LoopParams& params, Loop* loop, CFG* C, ScalarEvolution* SE);
    void hoistVariable(Loop* loop, CFG* C, const HoistingCandidate& candidate, int finalValue);

    bool isVariableOnlyUsedForSelfIteration(Operand op, Loop* loop, CFG* C);
    
    // 检查是否为induction variable
    bool isInductionVariable(Operand op, Loop* loop);
    
    // 分离induction variable和其他变量
    std::pair<std::vector<HoistingCandidate>, std::vector<HoistingCandidate>> 
    separateInductionVariables(const std::vector<HoistingCandidate>& candidates, Loop* loop);
    
    bool isLinearArrayAccess(SCEV* array_scev, Operand induction_var, Loop* loop, ScalarEvolution* SE);
    bool isArithmeticOperation(Instruction inst, BasicInstruction::LLVMIROpcode op, Operand& lhs, Operand& rhs);
    
    // transform function
    void replaceWithMemset(Loop* loop, CFG* C, Operand array, Operand value, GepParams gep_params);
    void replaceWithConstant(Loop* loop, CFG* C, Operand target, int value);
};

#endif // LOOP_IDIOM_RECOGNIZE_PASS_H
