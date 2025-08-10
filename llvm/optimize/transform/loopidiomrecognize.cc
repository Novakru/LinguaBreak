#include "loopidiomrecognize.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <set>
#include "../../include/Instruction.h"

// 调试宏定义
// 精简调试输出，只保留关键信息
#define LOOP_IDIOM_DEBUG

#ifdef LOOP_IDIOM_DEBUG
#define LOOP_IDIOM_DEBUG_PRINT(x) do {std::cerr << "[LoopIdiom] "; x; } while(0)
#else
#define LOOP_IDIOM_DEBUG_PRINT(x) do {} while(0)
#endif

void LoopIdiomRecognizePass::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        processFunction(cfg);
        
        // 重新构建CFG和支配树
        cfg->BuildCFG();
        cfg->getDomTree()->BuildDominatorTree();
    }
}

void LoopIdiomRecognizePass::processFunction(CFG* C) {
    auto loop_info = C->getLoopInfo();
    if (!loop_info) return;
    
    ScalarEvolution* SE = C->getSCEVInfo();
    if (!SE) return;
    
    for (auto loop : loop_info->getTopLevelLoops()) {
        processLoop(loop, C, SE);
    }
}

void LoopIdiomRecognizePass::processLoop(Loop* loop, CFG* C, ScalarEvolution* SE) {
    for (auto sub_loop : loop->getSubLoops()) {
        processLoop(sub_loop, C, SE);
    }
    
    if (!loop->verifySimplifyForm(C) && loop->getExits().size() != 1) {
        return;
    }
    
    // 首先尝试循环外提优化（独立执行）
    std::set<Operand> hoistedVariables;
    bool hasHoisted = recognizeLoopHoisting(loop, C, SE, hoistedVariables);
    
    // 如果外提成功，检查是否所有外部使用的变量都已经外提
    if (hasHoisted) {
        // 检查是否还有未外提的外部使用变量（除了induction variable）
        std::vector<HoistingCandidate> remainingCandidates = findHoistingCandidates(loop, C, SE);
        bool allHoisted = true;
        
        for (const auto& candidate : remainingCandidates) {
            // 如果不是induction variable且未外提，说明还有变量无法外提
            if (!isInductionVariable(candidate.operand, loop) && 
                hoistedVariables.find(candidate.operand) == hoistedVariables.end()) {
                allHoisted = false;
                break;
            }
        }
        
        // 如果除了induction variable之外的所有变量都外提了，就不需要memset优化
        if (allHoisted) {
            return;
        }
    }
    
    // 然后检查是否可以进行memset优化
    if (canRecognizeMemsetIdiom(loop, C, SE)) {
        // 检查是否所有外部使用的变量都已经外提
        if (recognizeMemsetIdiom(loop, C, SE, hoistedVariables)) {
            LOOP_IDIOM_DEBUG_PRINT(std::cerr << "识别到memset习语" << std::endl);
            return;
        }
    }
}


/**
 * 检查循环是否能被memset优化（只检查可行性，不实际执行）
 */
bool LoopIdiomRecognizePass::canRecognizeMemsetIdiom(Loop* loop, CFG* C, ScalarEvolution* SE) {
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
    LoopParams params = SE->extractLoopParams(loop, C);
    // 暂时只支持步长为1的memset；trip count 可为动态（上下界循环不变量）
    if (params.step_val != 1) return false;
    
    // 检查数组访问模式
    SCEV* array_scev = SE->getSimpleSCEV(store->GetPointer(), loop);
    LLVMBlock header = loop->getHeader();
    BrCondInstruction* br_cond = (BrCondInstruction*)header->Instruction_list.back();
    IcmpInstruction* icmp = (IcmpInstruction*)SE->getDef(br_cond->GetCond());

    // 确定归纳变量
    Operand induction_var = (icmp->GetOp1()->GetOperandType() == BasicOperand::REG) ? icmp->GetOp1() : icmp->GetOp2();
    
    if (!isLinearArrayAccess(array_scev, induction_var, loop, SE)) {
        return false;
    }

    // 提取GEP参数
    LLVMBlock preheader = loop->getPreheader();
    GepParams gep_params = SE->extractGepParams(array_scev, loop, preheader, C);
    if (!gep_params.base_ptr) {
        return false;
    }
    
    return true;
}

/**
 * 识别memset习语：for(int i = 0; i <= n; i++) a[i] = const;
 * 注意：除了store的寄存器外，如果有循环外使用的变量不能外提，则不能进行整个循环的删除
 */
bool LoopIdiomRecognizePass::recognizeMemsetIdiom(Loop* loop, CFG* C, ScalarEvolution* SE, const std::set<Operand>& hoistedVariables) {
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
    LoopParams params = SE->extractLoopParams(loop, C);
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
    GepParams gep_params = SE->extractGepParams(array_scev, loop, preheader, C);
    if (!gep_params.base_ptr) {
        return false;
    }
    
    // 检查是否有循环外使用的变量不能外提（排除store指令本身使用的寄存器和已外提的变量）
    std::vector<HoistingCandidate> candidates = findHoistingCandidates(loop, C, SE);
    
    // 获取store指令使用的寄存器，这些不需要外提
    std::set<Operand> store_operands;
    if (store->GetPointer()->GetOperandType() == BasicOperand::REG) {
        store_operands.insert(store->GetPointer());
    }
    
    // 收集需要外提的induction variable
    std::vector<HoistingCandidate> induction_candidates;
    
    for (const auto& candidate : candidates) {
        // 跳过store指令本身使用的寄存器
        if (store_operands.find(candidate.operand) != store_operands.end()) {
            continue;
        }
        
        // 跳过已经成功外提的变量
        if (hoistedVariables.find(candidate.operand) != hoistedVariables.end()) {
            continue;
        }
        
        // 检查是否为induction variable
        if (isInductionVariable(candidate.operand, loop)) {
            // induction variable 先跳过检查，但收集起来
            induction_candidates.push_back(candidate);
            continue;
        }
        
        // 检查该变量是否可以被外提
        if (!canCalculateFinalValue(candidate, loop, C, SE)) {
            return false;
        }
    }
    

    replaceWithMemset(loop, C, store->GetPointer(), store->GetValue(), gep_params);
    
    // memset优化成功后，外提induction variable
    params = SE->extractLoopParams(loop, C);
    for (const auto& candidate : induction_candidates) {
        if (canCalculateFinalValue(candidate, loop, C, SE)) {
            int finalValue = calculateFinalValue(candidate, params, loop, C, SE);
            hoistVariable(loop, C, candidate, finalValue);
        }
    }
    
    return true;
}

bool LoopIdiomRecognizePass::isLinearArrayAccess(SCEV* array_scev, Operand induction_var, Loop* loop, ScalarEvolution* SE) {
    // 检查数组访问是否为线性模式：base + induction_var

    
    if (auto* add_expr = dynamic_cast<SCEVAddExpr*>(array_scev)) {
        bool has_induction = false;
        bool has_base = false;
        
        for (SCEV* operand : add_expr->getOperands()) {
            if (auto* addrec = dynamic_cast<SCEVAddRecExpr*>(operand)) {
                if (addrec->getLoop() == loop) {
                    has_induction = true;
                }
            } else if (auto* mul_expr = dynamic_cast<SCEVMulExpr*>(operand)) {
                // 检查乘法表达式中是否包含当前循环的归纳变量
                for (SCEV* mul_operand : mul_expr->getOperands()) {
                    if (auto* addrec = dynamic_cast<SCEVAddRecExpr*>(mul_operand)) {
                        if (addrec->getLoop() == loop) {
                            has_induction = true;
                        }
                    }
                }
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

void LoopIdiomRecognizePass::replaceWithMemset(Loop* loop, CFG* C, Operand array, Operand value, GepParams gep_params) {
    LLVMBlock preheader = loop->getPreheader();
    LLVMBlock exit = *loop->getExits().begin();

    ScalarEvolution* SE = C->getSCEVInfo();
    LoopParams params = SE->extractLoopParams(loop, C);

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
        Operand start_op = SE->buildExpression(induction_addrec->getStart(), preheader, C);
        Operand bound_op = SE->buildExpression(bound_scev, preheader, C);

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
    
    } else {
        // 如果没有偏移量，直接使用基址
        gep = new GetElementptrInstruction(
            BasicInstruction::I32,
            GetNewRegOperand(++C->max_reg),
            gep_params.base_ptr,
            new ImmI32Operand(0),
            BasicInstruction::I32
        );
    
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

    
    // 在循环前插入常数赋值
    // 简化实现：直接移除循环

}

/**
 * 识别循环外提优化：识别循环内计算的值在循环外被使用，并且可以根据循环迭代次数计算出最终值
 * 例如：i在循环内按照{0,+,1}递增，循环外使用i，可以计算出i的最终值为50
 */
bool LoopIdiomRecognizePass::recognizeLoopHoisting(Loop* loop, CFG* C, ScalarEvolution* SE, std::set<Operand>& hoistedVariables) {
    // 获取循环参数
    LoopParams params = SE->extractLoopParams(loop, C);
    if (params.count <= 0) {
        return false;
    }
    
    // 查找在循环外被使用的变量
    std::vector<HoistingCandidate> candidates = findHoistingCandidates(loop, C, SE);
    if (candidates.empty()) {
        return false;
    }
    
    // 分离induction variable和其他变量
    auto [induction_candidates, other_candidates] = separateInductionVariables(candidates, loop);
    
    // 先处理非induction variable
    bool hasHoisted = false;
    bool allOthersHoisted = true;
    
    for (const auto& candidate : other_candidates) {
        if (canCalculateFinalValue(candidate, loop, C, SE)) {
            int finalValue = calculateFinalValue(candidate, params, loop, C, SE);
            hoistVariable(loop, C, candidate, finalValue);
            hasHoisted = true;
        } else {
            allOthersHoisted = false;
        }
    }
    
    // 如果除了induction variable之外的所有变量都外提成功了，也外提induction variable
    if (allOthersHoisted) {
        for (const auto& candidate : induction_candidates) {
            if (canCalculateFinalValue(candidate, loop, C, SE)) {
                int finalValue = calculateFinalValue(candidate, params, loop, C, SE);
                hoistVariable(loop, C, candidate, finalValue);
                hasHoisted = true;
            }
        }
    }
    
    return hasHoisted;
}

bool LoopIdiomRecognizePass::isInductionVariable(Operand op, Loop* loop) {
    LLVMBlock header = loop->getHeader();
    for (auto inst : header->Instruction_list) {
        if (inst->GetOpcode() == BasicInstruction::ICMP) {
            auto icmp = (IcmpInstruction*)inst;
            if (icmp->GetOp1() == op || icmp->GetOp2() == op) {
                return true;
            }
        }
    }
    return false;
}

std::pair<std::vector<HoistingCandidate>, std::vector<HoistingCandidate>> 
LoopIdiomRecognizePass::separateInductionVariables(const std::vector<HoistingCandidate>& candidates, Loop* loop) {
    std::vector<HoistingCandidate> induction_candidates;
    std::vector<HoistingCandidate> other_candidates;
    
    for (const auto& candidate : candidates) {
        if (isInductionVariable(candidate.operand, loop)) {
            induction_candidates.push_back(candidate);
        } else {
            other_candidates.push_back(candidate);
        }
    }
    
    return {induction_candidates, other_candidates};
}

/**
 * 查找在循环外被使用的变量
 */
std::vector<HoistingCandidate> 
LoopIdiomRecognizePass::findHoistingCandidates(Loop* loop, CFG* C, ScalarEvolution* SE) {
    std::vector<HoistingCandidate> candidates;
    std::set<Operand> loopExternUses;
    
    // 查找循环外对循环内定义变量的使用
    for (auto [block_id, block] : *C->block_map) {
        if (loop->contains(block)) continue; // 跳过循环内的块
        
        for (auto inst : block->Instruction_list) {
            for (auto op : inst->GetNonResultOperands()) {
                if (op->GetOperandType() == BasicOperand::REG) {
                    // 检查这个寄存器是否在循环内定义
                    Instruction def_inst = SE->getDef(op);
                    if (def_inst) {
                        LLVMBlock def_block = C->GetBlockWithId(def_inst->GetBlockID());
                        if (def_block && loop->contains(def_block)) {
                            loopExternUses.insert(op);
                        }
                    }
                }
            }
        }
    }
    
    // 为每个在循环外使用的变量创建候选，并检查循环内使用情况
    for (auto op : loopExternUses) {
        // 检查该变量在循环内是否除了自身迭代外不被使用
        if (!isVariableOnlyUsedForSelfIteration(op, loop, C)) {
            continue;
        }
        
        SCEV* scev = SE->getSCEV(op, loop);
        if (scev) {
            candidates.push_back({op, scev});
        }
    }
    
    return candidates;
}

/**
 * 检查变量在循环内是否只用于自身迭代和循环控制
 * 对于归纳变量，只有在其他外部使用的变量都外提之后才能外提
 * 允许的使用：
 * 1. 不被循环内的其他指令使用，除了自己的phi指令
 * 2. 非induction变量, 因为induction变量需要确保整个循环无效了才能外提
 */
bool LoopIdiomRecognizePass::isVariableOnlyUsedForSelfIteration(Operand op, Loop* loop, CFG* C) {
    LLVMBlock header = loop->getHeader();
    
    // 检查该变量是否为归纳变量
    if (isInductionVariable(op, loop)) {
        return false;
    } else {
        // 非归纳变量的检查逻辑保持不变
        for (auto block : loop->getBlocks()) {
            for (auto inst : block->Instruction_list) {
                if (inst->GetOpcode() == BasicInstruction::PHI) {
                    continue;
                }
                
                // 跳过循环控制指令
                if (block == header) {
                    if (inst->GetOpcode() == BasicInstruction::ICMP || 
                        inst->GetOpcode() == BasicInstruction::BR_COND ||
                        inst->GetOpcode() == BasicInstruction::BR_UNCOND) {
                        continue;
                    }
                }
                
                // 跳过增量指令（i = i + 1 或 i = i - 1）
                if (inst->GetOpcode() == BasicInstruction::ADD || 
                    inst->GetOpcode() == BasicInstruction::SUB) {
                    auto operands = inst->GetNonResultOperands();
                    if (operands.size() == 2) {
                        if ((operands[0] == op && operands[1]->GetOperandType() == BasicOperand::IMMI32 && 
                             ((ImmI32Operand*)operands[1])->GetIntImmVal() == 1) ||
                            (operands[1] == op && operands[0]->GetOperandType() == BasicOperand::IMMI32 && 
                             ((ImmI32Operand*)operands[0])->GetIntImmVal() == 1)) {
                            continue;
                        }
                    }
                }
                
                for (auto operand : inst->GetNonResultOperands()) {
                    if (operand == op) {
                        return false;
                    }
                }
            }
        }
        
        return true;
    }
}

/**
 * 判断是否可以计算出最终值
 */
bool LoopIdiomRecognizePass::canCalculateFinalValue(const HoistingCandidate& candidate, 
                                                   Loop* loop, CFG* C, ScalarEvolution* SE) {
    SCEV* scev = candidate.scev;
    
    // 检查是否为AddRecExpr（归纳变量）
    if (auto* addrec = dynamic_cast<SCEVAddRecExpr*>(scev)) {
        if (addrec->getLoop() != loop) return false;
        
        // 检查起始值和步长是否为常数
        if (auto* start_const = dynamic_cast<SCEVConstant*>(addrec->getStart())) {
            if (auto* step_const = dynamic_cast<SCEVConstant*>(addrec->getStep())) {
                return true; // 简单递推表达式：{start,+,step}
            }
        }
        
        // 检查是否为复杂的递推表达式，如 {0,+,{0,+,1}}（sum变量）
        SCEV* start = addrec->getStart();
        SCEV* step = addrec->getStep();
        
        if (auto* start_addrec = dynamic_cast<SCEVAddRecExpr*>(start)) {
            if (start_addrec->getLoop() == loop) {
                // 这是一个嵌套的递推表达式，可能是sum变量
                return canCalculateNestedRecursion(addrec, loop, C, SE);
            }
        }
    }
    
    return false;
}

/**
 * 检查嵌套递推表达式是否可以计算最终值
 * 例如：{0,+,{0,+,1}} 表示 sum += i，其中 i 是 {0,+,1}
 */
bool LoopIdiomRecognizePass::canCalculateNestedRecursion(SCEVAddRecExpr* addrec, 
                                                        Loop* loop, CFG* C, ScalarEvolution* SE) {
    SCEV* start = addrec->getStart();
    SCEV* step = addrec->getStep();
    
    // 检查起始值是否为嵌套的AddRecExpr
    if (auto* start_addrec = dynamic_cast<SCEVAddRecExpr*>(start)) {
        if (start_addrec->getLoop() != loop) return false;
        
        // 检查步长是否为AddRecExpr（表示每次增加i）
        if (auto* step_addrec = dynamic_cast<SCEVAddRecExpr*>(step)) {
            if (step_addrec->getLoop() != loop) return false;
            
            // 检查起始值的起始值和步长是否为常数
            if (!dynamic_cast<SCEVConstant*>(start_addrec->getStart())) return false;
            if (!dynamic_cast<SCEVConstant*>(start_addrec->getStep())) return false;
            
            // 检查步长的起始值和步长是否为常数
            if (!dynamic_cast<SCEVConstant*>(step_addrec->getStart())) return false;
            if (!dynamic_cast<SCEVConstant*>(step_addrec->getStep())) return false;
            
            return true;
        }
    }
    
    return false;
}

/**
 * 计算变量的最终值
 * 支持多种习语：
 * 1. 简单递推：{0,+,1} -> i = n
 * 2. 算术级数求和：{0,+,{0,+,1}} -> sum = n*(n-1)/2
 * 3. 模运算求和：{0,+,{0,+,1}} % p -> 需要特殊处理
 * 4. 乘法习语：{1,*,{0,+,1}} -> prod = n!
 * 5. 异或求和：{0,^,{0,+,1}} -> xor_sum = 0^1^2^...^(n-1)
 */
int LoopIdiomRecognizePass::calculateFinalValue(const HoistingCandidate& candidate, 
                                               const LoopParams& params, 
                                               Loop* loop, CFG* C, ScalarEvolution* SE) {
    SCEV* scev = candidate.scev;
    
    if (auto* addrec = dynamic_cast<SCEVAddRecExpr*>(scev)) {
        if (addrec->getLoop() == loop) {
            // 简单递推表达式：{start,+,step}
            if (auto* start_const = dynamic_cast<SCEVConstant*>(addrec->getStart())) {
                if (auto* step_const = dynamic_cast<SCEVConstant*>(addrec->getStep())) {
                    int start_val = start_const->getValue()->GetIntImmVal();
                    int step_val = step_const->getValue()->GetIntImmVal();
                    
                    // 计算最终值：start + step * count
                    // 对于 while(i < n) 循环，循环结束后 i = n
                    return start_val + step_val * params.count;
                }
            }
            
            // 处理嵌套递推表达式
            SCEV* start = addrec->getStart();
            SCEV* step = addrec->getStep();
            
            // 算术级数求和：{0,+,{0,+,1}}
            if (auto* start_addrec = dynamic_cast<SCEVAddRecExpr*>(start)) {
                if (auto* step_addrec = dynamic_cast<SCEVAddRecExpr*>(step)) {
                    if (start_addrec->getLoop() == loop && step_addrec->getLoop() == loop) {
                        // 检查是否为标准的算术级数求和模式
                        if (auto* start_start_const = dynamic_cast<SCEVConstant*>(start_addrec->getStart())) {
                            if (auto* start_step_const = dynamic_cast<SCEVConstant*>(start_addrec->getStep())) {
                                if (auto* step_start_const = dynamic_cast<SCEVConstant*>(step_addrec->getStart())) {
                                    if (auto* step_step_const = dynamic_cast<SCEVConstant*>(step_addrec->getStep())) {
                                        int start_start = start_start_const->getValue()->GetIntImmVal();
                                        int start_step = start_step_const->getValue()->GetIntImmVal();
                                        int step_start = step_start_const->getValue()->GetIntImmVal();
                                        int step_step = step_step_const->getValue()->GetIntImmVal();
                                        
                                        // 标准算术级数求和：sum = 0; for(i=1; i<=n; i++) sum += i;
                                        if (start_start == 0 && start_step == 0 && step_start == 0 && step_step == 1) {
                                            return params.count * (params.count - 1) / 2;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    // 处理其他类型的习语（乘法、异或等）
    // 这里可以根据需要扩展
    
    return 0; // 默认值
}

/**
 * 执行变量外提
 */
void LoopIdiomRecognizePass::hoistVariable(Loop* loop, CFG* C, 
                                          const HoistingCandidate& candidate, int finalValue) {
    std::cerr << "=== 进入hoistVariable函数 ===" << std::endl;
    LLVMBlock preheader = loop->getPreheader();
    
    std::cerr << "Preheader block_id: " << preheader->block_id 
              << ", 指令数量: " << preheader->Instruction_list.size() << std::endl;
    
    // 打印preheader中的所有指令
    for (int i = 0; i < preheader->Instruction_list.size(); i++) {
        std::cerr << "  [" << i << "] ";
        preheader->Instruction_list[i]->PrintIR(std::cerr);
    }
    
    // 在preheader中插入最终值的赋值
    Operand result_reg = GetNewRegOperand(++C->max_reg);
    
    // 对于常量值，直接使用常量赋值指令
    auto* assign_inst = new ArithmeticInstruction(
        BasicInstruction::LLVMIROpcode::ADD, 
        BasicInstruction::I32, 
        new ImmI32Operand(finalValue), 
        new ImmI32Operand(0), // 加0作为赋值
        result_reg
    );
    
    // 设置指令的块ID
    assign_inst->SetBlockID(preheader->block_id);
    
    // 在preheader的最后一条指令之前插入（通常是跳转指令）
    if (!preheader->Instruction_list.empty()) {
        // 先弹出最后一条指令（通常是跳转指令）
        Instruction last_inst = preheader->Instruction_list.back();
        preheader->Instruction_list.pop_back();
        
        // 插入新的赋值指令
        preheader->Instruction_list.push_back(assign_inst);
        
        // 再把跳转指令放回去
        preheader->Instruction_list.push_back(last_inst);
        
    } else {
        preheader->Instruction_list.push_back(assign_inst);
    }
    
    // 替换循环外对该变量的使用
    loop->replaceLoopExternUses(C, candidate.operand, result_reg);
}


