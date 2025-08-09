#include "loopidiomrecognize.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include "../../include/Instruction.h"

// 调试宏定义
#define LOOP_IDIOM_DEBUG

#ifdef LOOP_IDIOM_DEBUG
#define LOOP_IDIOM_DEBUG_PRINT(x) do { x; } while(0)
#else
#define LOOP_IDIOM_DEBUG_PRINT(x) do {} while(0)
#endif

void LoopIdiomRecognizePass::Execute() {
    LOOP_IDIOM_DEBUG_PRINT(std::cerr << "=== 开始循环习语识别优化 ===" << std::endl);
    
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        LOOP_IDIOM_DEBUG_PRINT(std::cerr << "处理函数: " 
                                      << cfg->function_def->GetFunctionName() << std::endl);
        
        processFunction(cfg);
        
        // 重新构建CFG和支配树
        cfg->BuildCFG();
        cfg->getDomTree()->BuildDominatorTree();
    }
    
    LOOP_IDIOM_DEBUG_PRINT(std::cerr << "=== 循环习语识别优化完成 ===" << std::endl);
}

void LoopIdiomRecognizePass::processFunction(CFG* C) {
    auto loop_info = C->getLoopInfo();
    if (!loop_info) return;
    
    ScalarEvolution* SE = C->getSCEVInfo();
    if (!SE) return;
    
    for (auto loop : loop_info->getTopLevelLoops()) {
        LOOP_IDIOM_DEBUG_PRINT(std::cerr << "分析循环，header block_id=" 
                                      << loop->getHeader()->block_id << std::endl);
        
        processLoop(loop, C, SE);
    }
}

void LoopIdiomRecognizePass::processLoop(Loop* loop, CFG* C, ScalarEvolution* SE) {
    for (auto sub_loop : loop->getSubLoops()) {
        processLoop(sub_loop, C, SE);
    }
    
    if (!loop->verifySimplifyForm(C) && loop->getExits().size() != 1) {
        LOOP_IDIOM_DEBUG_PRINT(std::cerr << "不是简单循环，跳过" << std::endl);
        return;
    }
    
    if (recognizeMemsetIdiom(loop, C, SE)) {
        LOOP_IDIOM_DEBUG_PRINT(std::cerr << "识别到memset习语" << std::endl);
        return;
    }
    
    if (recognizeArithmeticSumIdiom(loop, C, SE)) {
        LOOP_IDIOM_DEBUG_PRINT(std::cerr << "识别到算术级数求和习语" << std::endl);
        return;
    }
    
    if (recognizeModuloSumIdiom(loop, C, SE)) {
        LOOP_IDIOM_DEBUG_PRINT(std::cerr << "识别到模运算求和习语" << std::endl);
        return;
    }
    
    if (recognizeMultiplicationIdiom(loop, C, SE)) {
        LOOP_IDIOM_DEBUG_PRINT(std::cerr << "识别到乘法习语" << std::endl);
        return;
    }
    
    if (recognizeXorSumIdiom(loop, C, SE)) {
        LOOP_IDIOM_DEBUG_PRINT(std::cerr << "识别到异或求和习语" << std::endl);
        return;
    }
}


/**
 * 识别memset习语：for(int i = 0; i <= n; i++) a[i] = const;
 * maybe bug: 暂时只支持全部常数
 */
bool LoopIdiomRecognizePass::recognizeMemsetIdiom(Loop* loop, CFG* C, ScalarEvolution* SE) {
	LOOP_IDIOM_DEBUG_PRINT(std::cerr << "尝试识别memset习语..." << std::endl);
    auto blocks = loop->getBlocks();
    if (blocks.size() != 2) return false;
    
    LLVMBlock body = (blocks[0] == loop->getHeader()) ? blocks[1] : blocks[0];
    
    StoreInstruction* store = nullptr;
    for (auto inst : body->Instruction_list) {
        if (inst->GetOpcode() == BasicInstruction::STORE) {
            store = (StoreInstruction*)inst;
            break;
        }
    }
    if (!store || store->GetValue()->GetOperandType() != BasicOperand::IMMI32) return false;

    // 使用extractLoopParams获取循环参数
    LoopParams params = extractLoopParams(loop, C, SE);
    // 暂时只支持步长为1的memset；trip count 可为动态（上下界循环不变量）
    if (params.step_val != 1) return false;
	
    // 检查数组访问模式
    SCEV* array_scev = SE->getSimpleSCEV(store->GetPointer(), loop);
    LLVMBlock header = loop->getHeader();
    LLVMBlock preheader = loop->getPreheader();
    LLVMBlock exit = *loop->getExits().begin();
    BrCondInstruction* br_cond = (BrCondInstruction*)header->Instruction_list.back();
    IcmpInstruction* icmp = (IcmpInstruction*)SE->getDef(br_cond->GetCond());

    // 确定归纳变量
    Operand induction_var = (icmp->GetOp1()->GetOperandType() == BasicOperand::REG) ? icmp->GetOp1() : icmp->GetOp2();
    
    if (!isLinearArrayAccess(array_scev, induction_var, loop, SE)) {
        return false;
    }

    // 提取GEP参数
    GepParams gep_params = extractGepParams(array_scev, loop, SE, preheader, C);
    if (!gep_params.base_ptr) {
        return false;
    }
    
    LOOP_IDIOM_DEBUG_PRINT(std::cerr << "线性GEP参数: base_ptr=" << gep_params.base_ptr->GetFullName() 
                                  << ", offset_op=" << (gep_params.offset_op ? gep_params.offset_op->GetFullName() : "null") << std::endl);
    
    replaceWithMemset(loop, C, store->GetPointer(), store->GetValue(), gep_params);
    return true;
}

/**
 * 识别算术级数求和习语：sum = 0; for(int i = 1; i <= const; i++) { sum += i; }
 */
bool LoopIdiomRecognizePass::recognizeArithmeticSumIdiom(Loop* loop, CFG* C, ScalarEvolution* SE) {
    LOOP_IDIOM_DEBUG_PRINT(std::cerr << "尝试识别算术级数求和习语..." << std::endl);
    return false;
    // // 使用extractLoopParams获取循环参数
    // LoopParams params = extractLoopParams(loop, C, SE);
    // if (params.count <= 0) return false;
    
    // // 检查循环体
    // auto blocks = loop->getBlocks();
    // if (blocks.size() != 2) return false;
    
    // LLVMBlock body = nullptr;
    // for (auto block : blocks) {
    //     if (block != loop->getHeader()) {
    //         body = block;
    //         break;
    //     }
    // }
    // if (!body) return false;
    
    // // 查找加法操作
    // Operand sum_var, induction_var;
    // for (auto inst : body->Instruction_list) {
    //     if (auto* arith = dynamic_cast<ArithmeticInstruction*>(inst)) {
    //         if (arith->GetOpcode() == BasicInstruction::ADD) {
    //             Operand lhs, rhs;
    //             if (isArithmeticOperation(inst, BasicInstruction::ADD, lhs, rhs)) {
    //                 // 检查是否有一个操作数是归纳变量
    //                 if (isInductionVariable(lhs, loop, SE)) {
    //                     sum_var = arith->GetResult();
    //                     induction_var = lhs;
    //                 } else if (isInductionVariable(rhs, loop, SE)) {
    //                     sum_var = arith->GetResult();
    //                     induction_var = rhs;
    //                 }
    //             }
    //         }
    //     }
    // }
    
    // if (!sum_var || !induction_var) return false;
    
    // LOOP_IDIOM_DEBUG_PRINT(std::cerr << "识别到算术级数求和习语: sum=" << sum_var->GetName() 
    //                               << ", bound=" << params.bound_val << std::endl);
    
    // // 执行优化
    // replaceWithArithmeticSum(loop, C, sum_var, params.bound_val);
    // return true;
}

/**
 * 识别模运算求和习语：sum = 0; for(int i = 1; i <= const; i++) { sum = (sum + i) % p; }
 */
bool LoopIdiomRecognizePass::recognizeModuloSumIdiom(Loop* loop, CFG* C, ScalarEvolution* SE) {
    LOOP_IDIOM_DEBUG_PRINT(std::cerr << "尝试识别模运算求和习语..." << std::endl);
    return false;
    // // 使用extractLoopParams获取循环参数
    // LoopParams params = extractLoopParams(loop, C, SE);
    // if (params.count <= 0) return false;
    
    // // 检查循环体
    // auto blocks = loop->getBlocks();
    // if (blocks.size() != 2) return false;
    
    // LLVMBlock body = nullptr;
    // for (auto block : blocks) {
    //     if (block != loop->getHeader()) {
    //         body = block;
    //         break;
    //     }
    // }
    // if (!body) return false;
    
    // // 查找模运算操作
    // Operand sum_var, modulo_const;
    // for (auto inst : body->Instruction_list) {
    //     if (auto arith = dynamic_cast<ArithmeticInstruction*>(inst)) {
    //         if (arith->GetOpcode() == BasicInstruction::MOD) {
    //             Operand lhs, rhs;
    //             if (isArithmeticOperation(inst, BasicInstruction::MOD, lhs, rhs)) {
    //                 // 检查左操作数是否为加法结果
    //                 Instruction lhs_def = SE->getDef(lhs);
    //                 if (auto add_inst = dynamic_cast<ArithmeticInstruction*>(lhs_def)) {
    //                     if (add_inst->GetOpcode() == BasicInstruction::ADD) {
    //                         sum_var = arith->GetResult();
    //                         modulo_const = rhs;
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }
    
    // if (!sum_var || !modulo_const) return false;
    
    // // 检查模数是否为常数
    // if (modulo_const->GetType() != OperandType::IMMEDIATE) return false;
    // int p = ((ImmI32Operand*)modulo_const)->GetValue();
    
    // LOOP_IDIOM_DEBUG_PRINT(std::cerr << "识别到模运算求和习语: sum=" << sum_var->GetName() 
    //                               << ", bound=" << params.bound_val << ", modulo=" << p << std::endl);
    
    // // 执行优化
    // replaceWithModuloSum(loop, C, sum_var, params.bound_val, p);
    // return true;
}

/**
 * 识别乘法习语：prod = 1; for(int i = 1; i <= const; i++) { prod *= i; }
 */
bool LoopIdiomRecognizePass::recognizeMultiplicationIdiom(Loop* loop, CFG* C, ScalarEvolution* SE) {
    LOOP_IDIOM_DEBUG_PRINT(std::cerr << "尝试识别乘法习语..." << std::endl);
    return false;
    // // 使用extractLoopParams获取循环参数
    // LoopParams params = extractLoopParams(loop, C, SE);
    // if (params.count <= 0 || params.bound_val > 20) return false; // 限制阶乘大小
    
    // // 检查循环体
    // auto blocks = loop->getBlocks();
    // if (blocks.size() != 2) return false;
    
    // LLVMBlock body = nullptr;
    // for (auto block : blocks) {
    //     if (block != loop->getHeader()) {
    //         body = block;
    //         break;
    //     }
    // }
    // if (!body) return false;
    
    // // 查找乘法操作
    // Operand prod_var, induction_var;
    // for (auto inst : body->Instruction_list) {
    //     if (auto arith = dynamic_cast<ArithmeticInstruction*>(inst)) {
    //         if (arith->GetOpcode() == BasicInstruction::MUL) {
    //             Operand lhs, rhs;
    //             if (isArithmeticOperation(inst, BasicInstruction::MUL, lhs, rhs)) {
    //                 // 检查是否有一个操作数是归纳变量
    //                 if (isInductionVariable(lhs, loop, SE)) {
    //                     prod_var = arith->GetResult();
    //                     induction_var = lhs;
    //                 } else if (isInductionVariable(rhs, loop, SE)) {
    //                     prod_var = arith->GetResult();
    //                     induction_var = rhs;
    //                 }
    //             }
    //         }
    //     }
    // }
    
    // if (!prod_var || !induction_var) return false;
    
    // LOOP_IDIOM_DEBUG_PRINT(std::cerr << "识别到乘法习语: prod=" << prod_var->GetName() 
    //                               << ", bound=" << params.bound_val << std::endl);
    
    // // 执行优化
    // replaceWithFactorial(loop, C, prod_var, params.bound_val);
    // return true;
}

/**
 * 识别异或求和习语：xor_sum = 0; for(int i = 1; i <= const; i++) { xor_sum ^= i; }
 */
bool LoopIdiomRecognizePass::recognizeXorSumIdiom(Loop* loop, CFG* C, ScalarEvolution* SE) {
    LOOP_IDIOM_DEBUG_PRINT(std::cerr << "尝试识别异或求和习语..." << std::endl);
    return false;
    // // 使用extractLoopParams获取循环参数
    // LoopParams params = extractLoopParams(loop, C, SE);
    // if (params.count <= 0) return false;
    
    // // 检查循环体
    // auto blocks = loop->getBlocks();
    // if (blocks.size() != 2) return false;
    
    // LLVMBlock body = nullptr;
    // for (auto block : blocks) {
    //     if (block != loop->getHeader()) {
    //         body = block;
    //         break;
    //     }
    // }
    // if (!body) return false;
    
    // // 查找异或操作
    // Operand xor_var, induction_var;
    // for (auto inst : body->Instruction_list) {
    //     if (auto arith = dynamic_cast<ArithmeticInstruction*>(inst)) {
    //         if (arith->GetOpcode() == BasicInstruction::BITXOR) {
    //             Operand lhs, rhs;
    //             if (isArithmeticOperation(inst, BasicInstruction::BITXOR, lhs, rhs)) {
    //                 // 检查是否有一个操作数是归纳变量
    //                 if (isInductionVariable(lhs, loop, SE)) {
    //                     xor_var = arith->GetResult();
    //                     induction_var = lhs;
    //                 } else if (isInductionVariable(rhs, loop, SE)) {
    //                     xor_var = arith->GetResult();
    //                     induction_var = rhs;
    //                 }
    //             }
    //         }
    //     }
    // }
    
    // if (!xor_var || !induction_var) return false;
    
    // LOOP_IDIOM_DEBUG_PRINT(std::cerr << "识别到异或求和习语: xor_sum=" << xor_var->GetName() 
    //                               << ", bound=" << params.bound_val << std::endl);
    
    // // 执行优化
    // replaceWithXorSum(loop, C, xor_var, params.bound_val);
    // return true;
}

bool LoopIdiomRecognizePass::isInductionVariable(Operand var, Loop* loop, ScalarEvolution* SE) {
    SCEV* scev = SE->getSCEV(var, loop);
    return scev && scev->getType() == scAddRecExpr;
}

bool LoopIdiomRecognizePass::isLinearArrayAccess(SCEV* array_scev, Operand induction_var, Loop* loop, ScalarEvolution* SE) {
    // 检查数组访问是否为线性模式：base + induction_var
	array_scev->print(std::cerr);
    if (auto* add_expr = dynamic_cast<SCEVAddExpr*>(array_scev)) {
        bool has_induction = false;
        bool has_base = false;
        
        for (SCEV* operand : add_expr->getOperands()) {
            if (auto* addrec = dynamic_cast<SCEVAddRecExpr*>(operand)) {
                if (addrec->getLoop() == loop) has_induction = true;
            } else if (dynamic_cast<SCEVUnknown*>(operand) || dynamic_cast<SCEVConstant*>(operand)) {
                has_base = true;
            }
        }
        
        return has_induction && has_base;
    }
    return false;
}

bool LoopIdiomRecognizePass::isArithmeticOperation(Instruction inst, BasicInstruction::LLVMIROpcode op, Operand& lhs, Operand& rhs) {
    if (auto arith = dynamic_cast<ArithmeticInstruction*>(inst)) {
        if (arith->GetOpcode() == op) {
            lhs = arith->GetOperand1();
            rhs = arith->GetOperand2();
            return true;
        }
    }
    return false;
}

LoopParams LoopIdiomRecognizePass::extractLoopParams(Loop* loop, CFG* C, ScalarEvolution* SE) {
    LLVMBlock header = loop->getHeader();
    BrCondInstruction* br_cond = (BrCondInstruction*)header->Instruction_list.back();
    IcmpInstruction* icmp = (IcmpInstruction*)SE->getDef(br_cond->GetCond());
    
    SCEV* scev1 = SE->getSimpleSCEV(icmp->GetOp1(), loop);
    SCEV* scev2 = SE->getSimpleSCEV(icmp->GetOp2(), loop);
    
    SCEV* induction_scev = nullptr;
    SCEV* bound_scev = nullptr;
    bool induction_is_op1 = false;
    
    if (auto* addrec = dynamic_cast<SCEVAddRecExpr*>(scev1)) {
        if (addrec->getLoop() == loop) {
            induction_scev = scev1;
            bound_scev = scev2;
            induction_is_op1 = true;
        }
    } else if (auto* addrec = dynamic_cast<SCEVAddRecExpr*>(scev2)) {
        if (addrec->getLoop() == loop) {
            induction_scev = scev2;
            bound_scev = scev1;
            induction_is_op1 = false;
        }
    }
    
    if (!induction_scev || !bound_scev) {
        return {0, 0, 0}; // 无法解析
    }
    
    int start_val = 0;
    int bound_val = 0;
    int step_val = 1;
    
    // 提取归纳变量的起始值和步长
    if (auto* addrec = dynamic_cast<SCEVAddRecExpr*>(induction_scev)) {
        if (auto* start_const = dynamic_cast<SCEVConstant*>(addrec->getStart())) {
            start_val = start_const->getValue()->GetIntImmVal();
        }
        if (auto* step_const = dynamic_cast<SCEVConstant*>(addrec->getStep())) {
            step_val = step_const->getValue()->GetIntImmVal();
        }
        LOOP_IDIOM_DEBUG_PRINT(std::cerr << "SCEV AddRec: start=" << start_val << ", step=" << step_val << std::endl);
    }
    
    // 提取边界值
    if (auto* bound_const = dynamic_cast<SCEVConstant*>(bound_scev)) {
        bound_val = bound_const->getValue()->GetIntImmVal();
    }
    
    // 根据比较操作符和步长方向计算迭代次数
    int count = 0;
    BasicInstruction::IcmpCond cmp_op = icmp->GetCond();
    
    if (step_val > 0) {
        // 递增循环：i++, i += 2, etc.
        if (cmp_op == BasicInstruction::IcmpCond::slt || cmp_op == BasicInstruction::IcmpCond::ult) {
            // i < bound
            count = (bound_val - start_val) / step_val;
        } else if (cmp_op == BasicInstruction::IcmpCond::sle || cmp_op == BasicInstruction::IcmpCond::ule) {
            // i <= bound
            count = (bound_val - start_val) / step_val + 1;
        } else if (cmp_op == BasicInstruction::IcmpCond::sgt || cmp_op == BasicInstruction::IcmpCond::ugt) {
            // i > bound (这种情况通常不会发生，但为了完整性)
            count = 0;
        } else if (cmp_op == BasicInstruction::IcmpCond::sge || cmp_op == BasicInstruction::IcmpCond::uge) {
            // i >= bound (这种情况通常不会发生，但为了完整性)
            count = 0;
        }
    } else if (step_val < 0) {
        // 递减循环：i--, i -= 2, etc.
        if (cmp_op == BasicInstruction::IcmpCond::sgt || cmp_op == BasicInstruction::IcmpCond::ugt) {
            // i > bound
            count = abs(start_val - bound_val) / abs(step_val);
        } else if (cmp_op == BasicInstruction::IcmpCond::sge || cmp_op == BasicInstruction::IcmpCond::uge) {
            // i >= bound
            count = abs(start_val - bound_val) / abs(step_val) + 1;
        } else if (cmp_op == BasicInstruction::IcmpCond::slt || cmp_op == BasicInstruction::IcmpCond::ult) {
            // i < bound (这种情况通常不会发生，但为了完整性)
            count = 0;
        } else if (cmp_op == BasicInstruction::IcmpCond::sle || cmp_op == BasicInstruction::IcmpCond::ule) {
            // i <= bound (这种情况通常不会发生，但为了完整性)
            count = 0;
        }
    }
    
    // 确保count不为负数
    if (count < 0) count = 0;
    
    LOOP_IDIOM_DEBUG_PRINT(std::cerr << "循环参数: start=" << start_val << ", bound=" << bound_val 
                          << ", step=" << step_val << ", count=" << count 
                          << ", cmp_op=" << (int)cmp_op << std::endl);
    
    return {start_val, bound_val, count, step_val};
}

GepParams LoopIdiomRecognizePass::extractGepParams(SCEV* array_scev, Loop* loop, ScalarEvolution* SE, LLVMBlock preheader, CFG* C) {
    Operand base_ptr = nullptr;
    Operand offset_op = nullptr;
    
    // 处理加法表达式：base + index1*stride1 + index2*stride2 + ... + induction_var
    if (auto* add_expr = dynamic_cast<SCEVAddExpr*>(array_scev)) {
        for (SCEV* operand : add_expr->getOperands()) {
            if (auto* unknown = dynamic_cast<SCEVUnknown*>(operand)) {
                Operand val = unknown->getValue();
                // 仅当是指针类型时作为base_ptr；否则视为偏移量的一部分
                if (val && val->GetOperandType() == BasicOperand::REG && base_ptr == nullptr) {
                    base_ptr = val;
                    continue;
                }
                // 非指针unknown归入offset
                Operand operand_op = generateExpressionOperand(operand, SE, preheader, C);
                if (offset_op == nullptr) {
                    offset_op = operand_op;
                } else {
                    Operand res = GetNewRegOperand(++C->max_reg);
                    auto* addInst = new ArithmeticInstruction(BasicInstruction::LLVMIROpcode::ADD, BasicInstruction::I32, offset_op, operand_op, res);
                    preheader->Instruction_list.push_back(addInst);
                    offset_op = res;
                }
            } else {
                // 常量、AddRec、Mul 等均加入offset表达式（AddRec将取其start，表示起始地址）
                Operand operand_op = generateExpressionOperand(operand, SE, preheader, C);
                if (offset_op == nullptr) {
                    offset_op = operand_op;
                } else {
                    Operand res = GetNewRegOperand(++C->max_reg);
                    auto* addInst = new ArithmeticInstruction(BasicInstruction::LLVMIROpcode::ADD, BasicInstruction::I32, offset_op, operand_op, res);
                    preheader->Instruction_list.push_back(addInst);
                    offset_op = res;
                }
            }
        }
    }
    
    return {base_ptr, offset_op};
}

Operand LoopIdiomRecognizePass::generateExpressionOperand(SCEV* scev, ScalarEvolution* SE, LLVMBlock preheader, CFG* C) {
    if (auto* const_val = dynamic_cast<SCEVConstant*>(scev)) {
        return new ImmI32Operand(const_val->getValue()->GetIntImmVal());
    } else if (auto* unknown = dynamic_cast<SCEVUnknown*>(scev)) {
        return unknown->getValue();
    } else if (auto* addrec = dynamic_cast<SCEVAddRecExpr*>(scev)) {
        // 处理归纳变量，提取起始值
        if (auto* start_const = dynamic_cast<SCEVConstant*>(addrec->getStart())) {
            return new ImmI32Operand(start_const->getValue()->GetIntImmVal());
        } else if (auto* start_unknown = dynamic_cast<SCEVUnknown*>(addrec->getStart())) {
            return start_unknown->getValue();
        } else if (auto* start_add = dynamic_cast<SCEVAddExpr*>(addrec->getStart())) {
            return generateExpressionOperand(start_add, SE, preheader, C);
        } else if (auto* start_mul = dynamic_cast<SCEVMulExpr*>(addrec->getStart())) {
            return generateExpressionOperand(start_mul, SE, preheader, C);
        }
    } else if (auto* add_expr = dynamic_cast<SCEVAddExpr*>(scev)) {
        // 处理加法表达式，参考loopstrengthreduce.cc的buildOffsetExpr方法
        Operand result = nullptr;
        for (SCEV* operand : add_expr->getOperands()) {
            Operand op = generateExpressionOperand(operand, SE, preheader, C);
            if (result == nullptr) {
                result = op;
            } else {
                // 生成加法指令
                Operand res = GetNewRegOperand(++C->max_reg);
                auto* mulInst = new ArithmeticInstruction(BasicInstruction::LLVMIROpcode::ADD, BasicInstruction::I32, result, op, res);
                preheader->Instruction_list.push_back(mulInst);
                result = res;
            }
        }
        return result;
    } else if (auto* mul_expr = dynamic_cast<SCEVMulExpr*>(scev)) {
        // 处理乘法表达式，参考loopstrengthreduce.cc的buildOffsetExpr方法
        Operand result = nullptr;
        for (SCEV* operand : mul_expr->getOperands()) {
            Operand op = generateExpressionOperand(operand, SE, preheader, C);
            if (result == nullptr) {
                result = op;
            } else {
                // 生成乘法指令
                Operand res = GetNewRegOperand(++C->max_reg);
                auto* mulInst = new ArithmeticInstruction(BasicInstruction::LLVMIROpcode::MUL, BasicInstruction::I32, result, op, res);
                preheader->Instruction_list.push_back(mulInst);
                result = res;
            }
        }
        return result;
    }
    return nullptr;
}



void LoopIdiomRecognizePass::replaceWithMemset(Loop* loop, CFG* C, Operand array, Operand value, GepParams gep_params) {
    LLVMBlock preheader = loop->getPreheader();
    LLVMBlock exit = *loop->getExits().begin();

    ScalarEvolution* SE = C->getSCEVInfo();
    LoopParams params = extractLoopParams(loop, C, SE);

    // 删除原 preheader 末尾到循环头的无条件跳转，准备插入新指令
    if (!preheader->Instruction_list.empty()) {
        preheader->Instruction_list.pop_back();
    }

    // 计算动态/静态的memset字节数参数：trip_count * 4
    // 从header的icmp提取 start 与 bound（均允许为循环不变量表达式）
    LLVMBlock header = loop->getHeader();
    BrCondInstruction* br_cond = (BrCondInstruction*)header->Instruction_list.back();
    IcmpInstruction* icmp = (IcmpInstruction*)SE->getDef(br_cond->GetCond());

    SCEV* scev1 = SE->getSimpleSCEV(icmp->GetOp1(), loop);
    SCEV* scev2 = SE->getSimpleSCEV(icmp->GetOp2(), loop);

    SCEVAddRecExpr* induction_addrec = nullptr;
    SCEV* bound_scev = nullptr;
    if (auto* addrec = dynamic_cast<SCEVAddRecExpr*>(scev1); addrec && addrec->getLoop() == loop) {
        induction_addrec = addrec;
        bound_scev = scev2;
    } else if (auto* addrec = dynamic_cast<SCEVAddRecExpr*>(scev2); addrec && addrec->getLoop() == loop) {
        induction_addrec = addrec;
        bound_scev = scev1;
    }

    Operand trip_bytes_op = nullptr;
    if (induction_addrec && bound_scev) {
        // start = addrec.start; bound = expr
        Operand start_op = generateExpressionOperand(induction_addrec->getStart(), SE, preheader, C);
        Operand bound_op = generateExpressionOperand(bound_scev, SE, preheader, C);

        if (start_op && bound_op) {
            // 保证二者均为循环不变量（若为寄存器）
            bool start_ok = true, bound_ok = true;
            if (start_op->GetOperandType() == BasicOperand::REG) {
                start_ok = SE->isLoopInvariant(start_op, loop);
            }
            if (bound_op->GetOperandType() == BasicOperand::REG) {
                bound_ok = SE->isLoopInvariant(bound_op, loop);
            }
            if (start_ok && bound_ok) {
				// diff = bound - start
				Operand diff_reg = GetNewRegOperand(++C->max_reg);
				auto* subInst = new ArithmeticInstruction(BasicInstruction::LLVMIROpcode::SUB, BasicInstruction::I32, bound_op, start_op, diff_reg);
				preheader->Instruction_list.push_back(subInst);

				Operand trip_reg = diff_reg;
				auto cmp_op = icmp->GetCond();
				if (cmp_op == BasicInstruction::IcmpCond::sle || cmp_op == BasicInstruction::IcmpCond::ule) {
					// trip = diff + 1
					Operand plus1_reg = GetNewRegOperand(++C->max_reg);
					auto* add1 = new ArithmeticInstruction(BasicInstruction::LLVMIROpcode::ADD, BasicInstruction::I32, trip_reg, new ImmI32Operand(1), plus1_reg);
					preheader->Instruction_list.push_back(add1);
					trip_reg = plus1_reg;
				}

				// bytes = trip * 4
				Operand bytes_reg = GetNewRegOperand(++C->max_reg);
				auto* mulBytes = new ArithmeticInstruction(BasicInstruction::LLVMIROpcode::MUL, BasicInstruction::I32, trip_reg, new ImmI32Operand(4), bytes_reg);
				preheader->Instruction_list.push_back(mulBytes);
				trip_bytes_op = bytes_reg;
            }
        }
    }

    // 生成线性GEP指令计算起始地址
    GetElementptrInstruction* gep = nullptr;
    if (gep_params.offset_op) {
        // 使用计算出的偏移量生成线性GEP指令
        gep = new GetElementptrInstruction(
            BasicInstruction::I32,
            GetNewRegOperand(++C->max_reg),
            gep_params.base_ptr,
            gep_params.offset_op,
            BasicInstruction::I32
        );
        LOOP_IDIOM_DEBUG_PRINT(std::cerr << "生成线性GEP指令，偏移量: " << gep_params.offset_op->GetFullName() << std::endl);
    } else {
        // 如果没有偏移量，直接使用基址
        gep = new GetElementptrInstruction(
            BasicInstruction::I32,
            GetNewRegOperand(++C->max_reg),
            gep_params.base_ptr,
            new ImmI32Operand(0),
            BasicInstruction::I32
        );
        LOOP_IDIOM_DEBUG_PRINT(std::cerr << "生成线性GEP指令，无偏移量" << std::endl);
    }
    preheader->Instruction_list.push_back(gep);

    // 生成memset调用
    std::vector<std::pair<BasicInstruction::LLVMType, Operand>> args;
    args.push_back(std::make_pair(BasicInstruction::PTR, gep->GetResult()));
    args.push_back(std::make_pair(BasicInstruction::I8, value));
    if (trip_bytes_op) {
        args.push_back(std::make_pair(BasicInstruction::I32, trip_bytes_op));
    } else {
        // 回退到静态参数（仅当可用）
        args.push_back(std::make_pair(BasicInstruction::I32, new ImmI32Operand(params.count * 4)));
    }
    args.push_back(std::make_pair(BasicInstruction::I1, new ImmI32Operand(0)));

    CallInstruction* memset_call = new CallInstruction(BasicInstruction::VOID, nullptr, "llvm.memset.p0.i32", args);
    preheader->Instruction_list.push_back(memset_call);

    // preheader -> exit
    BrUncondInstruction* br = new BrUncondInstruction(GetNewLabelOperand(exit->block_id));
    preheader->Instruction_list.push_back(br);
}

void LoopIdiomRecognizePass::replaceWithConstant(Loop* loop, CFG* C, Operand target, int value) {
    LOOP_IDIOM_DEBUG_PRINT(std::cerr << "替换为常数: " << value << std::endl);
    
    // 在循环前插入常数赋值
    // 简化实现：直接移除循环
    removeLoop(loop, C);
}

void LoopIdiomRecognizePass::replaceWithArithmeticSum(Loop* loop, CFG* C, Operand target, int n) {
    int sum = calculateArithmeticSum(n);
    replaceWithConstant(loop, C, target, sum);
}

void LoopIdiomRecognizePass::replaceWithModuloSum(Loop* loop, CFG* C, Operand target, int n, int p) {
    int result = calculateModuloSum(n, p);
    replaceWithConstant(loop, C, target, result);
}

void LoopIdiomRecognizePass::replaceWithFactorial(Loop* loop, CFG* C, Operand target, int n) {
    int factorial = calculateFactorial(n);
    replaceWithConstant(loop, C, target, factorial);
}

void LoopIdiomRecognizePass::replaceWithXorSum(Loop* loop, CFG* C, Operand target, int n) {
    int xor_sum = calculateXorSum(n);
    replaceWithConstant(loop, C, target, xor_sum);
}

void LoopIdiomRecognizePass::removeLoop(Loop* loop, CFG* C) {
    LOOP_IDIOM_DEBUG_PRINT(std::cerr << "移除循环" << std::endl);
    
    // 连接循环前驱到循环出口
    connectPreheaderToExit(loop, C);
    
    // 更新PHI节点
    updatePhiNodes(loop, C);
    
    // 移除循环块
    auto blocks = loop->getBlocks();
    for (auto block : blocks) {
        if (block != loop->getHeader()) {
            // 从CFG中移除块
            // 简化实现
        }
    }
}

void LoopIdiomRecognizePass::connectPreheaderToExit(Loop* loop, CFG* C) {
    LLVMBlock preheader = loop->getPreheader();
    LLVMBlock exit = *loop->getExits().begin();
    
    // 修改preheader的最后一条指令，让它跳转到exit而不是循环头
    if (!preheader->Instruction_list.empty()) {
        Instruction last_inst = preheader->Instruction_list.back();
        if (last_inst->GetOpcode() == BasicInstruction::BR_UNCOND) {
            BrUncondInstruction* br = (BrUncondInstruction*)last_inst;
            br->ChangeDestLabel(GetNewLabelOperand(exit->block_id));
        }
    }
}

void LoopIdiomRecognizePass::updatePhiNodes(Loop* loop, CFG* C) {
    // 更新循环出口块的PHI节点
    // 简化实现
}


int LoopIdiomRecognizePass::calculateArithmeticSum(int n) {
    return n * (n + 1) / 2;
}

int LoopIdiomRecognizePass::calculateModuloSum(int n, int p) {
    int sum = 0;
    for (int i = 1; i <= n; i++) {
        sum = (sum + i) % p;
    }
    return sum;
}

int LoopIdiomRecognizePass::calculateFactorial(int n) {
    if (n <= 1) return 1;
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

int LoopIdiomRecognizePass::calculateXorSum(int n) {
    int xor_sum = 0;
    for (int i = 1; i <= n; i++) {
        xor_sum ^= i;
    }
    return xor_sum;
}


void LoopIdiomRecognizePass::debugPrint(const std::string& message) {
    LOOP_IDIOM_DEBUG_PRINT(std::cerr << "[LoopIdiom] " << message << std::endl);
}
