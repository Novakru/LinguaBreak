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

struct LoopParams {
	int start_val;
	int bound_val;
	int count;
	int step_val;
};

struct GepParams {
	Operand base_ptr;
	Operand offset_op;             // 计算出的总偏移量操作数
};

class LoopIdiomRecognizePass : public IRPass {
public:
    LoopIdiomRecognizePass(LLVMIR* IR) : IRPass(IR) {}
    void Execute() override;

private:
    void processFunction(CFG* C);
    void processLoop(Loop* loop, CFG* C, ScalarEvolution* SE);
    
    // recognize idiom
    bool recognizeMemsetIdiom(Loop* loop, CFG* C, ScalarEvolution* SE);
    bool recognizeArithmeticSumIdiom(Loop* loop, CFG* C, ScalarEvolution* SE);
    bool recognizeModuloSumIdiom(Loop* loop, CFG* C, ScalarEvolution* SE);
    bool recognizeMultiplicationIdiom(Loop* loop, CFG* C, ScalarEvolution* SE);
    bool recognizeXorSumIdiom(Loop* loop, CFG* C, ScalarEvolution* SE);
    
    bool isInductionVariable(Operand var, Loop* loop, ScalarEvolution* SE);

    bool isLinearArrayAccess(SCEV* array_scev, Operand induction_var, Loop* loop, ScalarEvolution* SE);
    bool isArithmeticOperation(Instruction inst, BasicInstruction::LLVMIROpcode op, Operand& lhs, Operand& rhs);

    LoopParams extractLoopParams(Loop* loop, CFG* C, ScalarEvolution* SE);
    GepParams extractGepParams(SCEV* array_scev, Loop* loop, ScalarEvolution* SE, LLVMBlock preheader, CFG* C);
    
    // 辅助函数：生成表达式操作数
    Operand generateExpressionOperand(SCEV* scev, ScalarEvolution* SE, LLVMBlock preheader, CFG* C);
    
    // transform function
    void replaceWithMemset(Loop* loop, CFG* C, Operand array, Operand value, GepParams gep_params);
    void replaceWithConstant(Loop* loop, CFG* C, Operand target, int value);
    void replaceWithArithmeticSum(Loop* loop, CFG* C, Operand target, int n);
    void replaceWithModuloSum(Loop* loop, CFG* C, Operand target, int n, int p);
    void replaceWithFactorial(Loop* loop, CFG* C, Operand target, int n);
    void replaceWithXorSum(Loop* loop, CFG* C, Operand target, int n);
    
    void removeLoop(Loop* loop, CFG* C);
    void connectPreheaderToExit(Loop* loop, CFG* C);
    void updatePhiNodes(Loop* loop, CFG* C);
    
    int calculateArithmeticSum(int n);
    int calculateModuloSum(int n, int p);
    int calculateFactorial(int n);
    int calculateXorSum(int n);

    void debugPrint(const std::string& message);
};

#endif // LOOP_IDIOM_RECOGNIZE_PASS_H
