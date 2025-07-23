#include "basic_cse.h"
#include <algorithm>
#include <cassert>
#include<functional>
#include <stack>
DomAnalysis *domtrees;
// 获取指令的CSE特征信息
static InstCSEInfo GetCSEInfo(Instruction inst) {
    InstCSEInfo info;
    info.opcode = inst->GetOpcode();
    auto operands = inst->GetNonResultOperands();
    std::string op1, op2;
    
    switch(info.opcode) {
        case BasicInstruction::CALL: {
            auto call_inst = static_cast<CallInstruction*>(inst);
            info.operand_list.push_back(call_inst->GetFunctionName());
            break;
        }
        case BasicInstruction::ADD:    // 操作数可交换
        case BasicInstruction::FADD:
        case BasicInstruction::FMUL:
        case BasicInstruction::MUL: {  // 操作数可交换
            assert(operands.size() == 2);
            
            // 创建排序后的操作数列表
            op1 = operands[0]->GetFullName();
            op2 = operands[1]->GetFullName();
            
            if (op1 > op2) 
                std::swap(op1, op2);
            
            info.operand_list.push_back(std::move(op1));
            info.operand_list.push_back(std::move(op2));
            break;
        }
        case BasicInstruction::GETELEMENTPTR: {
            auto gep_inst = static_cast<GetElementptrInstruction*>(inst);
        
            // 添加基本操作数
            for (auto op : operands) {
                info.operand_list.push_back(op->GetFullName());
            }
            
            // 添加维度信息
            for (auto dim : gep_inst->GetDims()) {
                info.operand_list.push_back(std::to_string(dim));
            }
            break;
        }
        default:
            info.operand_list.reserve(operands.size());
            for (auto op : operands) {
                info.operand_list.push_back(op->GetFullName());
            }
            break;
    }  
    return info;
}
// 从指令中提取CSE关键信息
static InstCSEInfo GetBranchCSEInfo(Instruction I) {
    InstCSEInfo ans;
    ans.opcode = I->GetOpcode();  // 获取操作码

    auto list = I->GetNonResultOperands(); // 获取非结果操作数
    
    // 处理函数调用指令的特殊情况
    if (I->GetOpcode() == BasicInstruction::CALL) {
        auto CallI = (CallInstruction *)I;
        ans.operand_list.push_back(CallI->GetFunctionName()); // 添加函数名
    }

    // 处理整型比较指令
    if (I->GetOpcode() == BasicInstruction::ICMP) {
        auto IcmpI = (IcmpInstruction *)I;
        ans.cond = IcmpI->GetCompareCondition(); // 记录比较条件
    }

    // 处理浮点比较指令
    if (I->GetOpcode() == BasicInstruction::FCMP) {
        auto FcmpI = (FcmpInstruction *)I;
        ans.cond = FcmpI->GetCompareCondition(); // 记录比较条件
    }

    // 处理可交换操作（加法和乘法）
    if (I->GetOpcode() == BasicInstruction::ADD || I->GetOpcode() == BasicInstruction::MUL) {
        assert(list.size() == 2); // 确保有两个操作数
        auto op1 = list[0], op2 = list[1];
        // 按名称排序保证交换律一致性
        if (op1->GetFullName() > op2->GetFullName()) {
            std::swap(op1, op2);
        }
        ans.operand_list.push_back(op1->GetFullName());
        ans.operand_list.push_back(op2->GetFullName());
    } else {
        // 普通指令按顺序添加操作数
        for (auto op : list) {
            ans.operand_list.push_back(op->GetFullName());
        }
    }
    return ans;
}

int GetResultRegNo(Instruction inst)
{
    auto result = (RegOperand*)inst->GetResult();
    if(result!=nullptr){return result->GetRegNo();}
    return -1;
}

// InstCSEInfo的比较运算符实现
bool InstCSEInfo::operator<(const InstCSEInfo &other) const {
    if (opcode != other.opcode) 
        {return opcode < other.opcode;}
    
    if (cond != other.cond) 
        {return cond < other.cond;}
    
    
    if (operand_list.size() != other.operand_list.size())
        {return operand_list.size() < other.operand_list.size();}
    
    return std::lexicographical_compare(
        operand_list.begin(), operand_list.end(),
        other.operand_list.begin(), other.operand_list.end()
    );
}


// CSE初始化函数实现
void SimpleCSEPass::CSEInit(CFG* cfg) {
    // 1. 把store立即数转换成store寄存器：add reg,val,0;store reg mem
    // 2.设置blockID
    // for (auto& block_pair : *(cfg->block_map)) {
    //     BasicBlock* bb = block_pair.second;
    //     std::deque<Instruction> new_instrs;
        
    //     for (BasicInstruction* inst : bb->Instruction_list) {
    //         if (inst->GetOpcode() == BasicInstruction::STORE) {
    //             StoreInstruction* store_inst = static_cast<StoreInstruction*>(inst);
    //             BasicOperand* val = store_inst->GetValue();
    //             auto type = val->GetOperandType();
                
    //             // 只处理支持的两种类型
    //             if (type == BasicOperand::IMMI32 || type ==BasicOperand::IMMF32 ) {
    //                 // 创建新寄存器
    //                 BasicOperand* new_reg = GetNewRegOperand(++cfg->max_reg);
    //                 auto new_intr = (type == BasicOperand::IMMI32) ? 
    //                     new ArithmeticInstruction(BasicInstruction::ADD, BasicInstruction::I32, val, new ImmI32Operand(0), new_reg) :
    //                     new ArithmeticInstruction(BasicInstruction::FADD, BasicInstruction::FLOAT32, val, new ImmF32Operand(0), new_reg);
    //                 new_intr->SetBlockID(block_pair.first);
    //                 new_instrs.push_back(new_intr);
    //                 // 更新store操作数
    //                 store_inst->SetValue(new_reg);
    //             }
    //         }
    //         inst->SetBlockID(block_pair.first);
    //         new_instrs.push_back(inst); // 添加原始指令
    //     }
    //     // 替换原指令列表
    //     bb->Instruction_list = new_instrs;
    // }
    for (auto [id, bb] : *cfg->block_map) {
        for (auto I : bb->Instruction_list) {
            I->SetBlockID(id);
        }
    }
}

// 判断指令是否不考虑CSE
bool CSENotConsider(Instruction I) {
    // 跳过控制流/内存分配/比较等指令
    if (I->GetOpcode() == BasicInstruction::PHI || I->GetOpcode() == BasicInstruction::BR_COND || 
        I->GetOpcode() == BasicInstruction::BR_UNCOND || I->GetOpcode() == BasicInstruction::ALLOCA ||
        I->GetOpcode() == BasicInstruction::RET || I->GetOpcode() == BasicInstruction::ICMP || 
        I->GetOpcode() == BasicInstruction::FCMP) {
        return false;
    }
    return true;
}
bool BranchCSENotConsider(Instruction I) {
    // 跳过控制流/内存分配/比较等指令
    if (I->GetOpcode() == BasicInstruction::PHI || I->GetOpcode() == BasicInstruction::BR_COND || 
        I->GetOpcode() == BasicInstruction::BR_UNCOND || I->GetOpcode() == BasicInstruction::ALLOCA ||
        I->GetOpcode() == BasicInstruction::RET) {
        return false;
    }
    return true;
}



// BasicBlockCSEOptimizer成员函数实现
bool BasicBlockCSEOptimizer::optimize() {
    // 多次迭代直到没有变化
    while (true) {
        reset();
        processAllBlocks();
        removeDeadInstructions();
        applyRegisterReplacements();
        
        if (!changed) break;
    }
    return changed;
}

void BasicBlockCSEOptimizer::reset() {
    changed = false;
    reg_replace_map.clear();
    erase_set.clear();
    //inst_map.clear();
}

void BasicBlockCSEOptimizer::processAllBlocks() {
    for (auto [id, bb] : *C->block_map) {
        inst_map.clear();
        changed|=processBlock(bb);
    }
}

bool BasicBlockCSEOptimizer::processBlock(LLVMBlock bb) {
    auto instructions = bb->Instruction_list;
    flag=false;
    for (auto I : instructions) {
        if (!CSENotConsider(I)) continue;
        
        switch (I->GetOpcode()) {
            case BasicInstruction::CALL:
                processCallInstruction(static_cast<CallInstruction*>(I));
                break;
            case BasicInstruction::STORE:
                processStoreInstruction(static_cast<StoreInstruction*>(I));
                break;
            case BasicInstruction::LOAD:
                processLoadInstruction(static_cast<LoadInstruction*>(I));
                break;
            default:
                processRegularInstruction(I);
                break;
        }
    }
    return flag;
}

void BasicBlockCSEOptimizer::processCallInstruction(CallInstruction* callI) {
    // 外部调用处理逻辑（待实现）
}

void BasicBlockCSEOptimizer::processStoreInstruction(StoreInstruction* storeI) {
    // 存储指令处理逻辑（待实现）
}

void BasicBlockCSEOptimizer::processLoadInstruction(LoadInstruction* loadI) {
    // 加载指令处理逻辑（待实现）
}

void BasicBlockCSEOptimizer::processRegularInstruction(BasicInstruction* I) {
    InstCSEInfo info = GetCSEInfo(I);
    
    if (inst_map.find(info) != inst_map.end()) {
        erase_set.insert(I);
        reg_replace_map[GetResultRegNo(I)] = inst_map[info];
        flag = true;
    } else {
        inst_map[info] = GetResultRegNo(I);
    }
}

void BasicBlockCSEOptimizer::removeDeadInstructions() {
    for (auto [id, bb] : *C->block_map) {
        std::deque<Instruction> new_instructions;
        
        for (auto I : bb->Instruction_list) {
            if (erase_set.find(I) == erase_set.end()) {
                new_instructions.push_back(I);
            }
        }
        
        bb->Instruction_list = new_instructions;
    }
}

void BasicBlockCSEOptimizer::applyRegisterReplacements() {
    // 规范化寄存器替换映射
    for (auto& [o_id, n_id] : reg_replace_map) {
        while (reg_replace_map.find(n_id) != reg_replace_map.end()) {
            n_id = reg_replace_map[n_id];
        }
    }
    
    // 应用寄存器替换
    for (auto [id, bb] : *C->block_map) {
        for (auto I : bb->Instruction_list) {
            I->ChangeResult(reg_replace_map);
            I->ChangeReg(reg_replace_map,reg_replace_map);
        }
    }
}

// 基本块内CSE优化入口
void SimpleCSEPass::SimpleBlockCSE(CFG* cfg) {
    BasicBlockCSEOptimizer optimizer(cfg);
    optimizer.optimize();
}
//DFS版本的BlockDef出错，暂沿用BFS版本
// 检查基本块是否满足"无定义使用"条件
static bool BlockDefNoUseCheck(CFG *C, int bb_id, int st_id) {
    //std::cout<<"begin BlockDefNoUseCheck\n";
    auto bb = (*C->block_map)[bb_id];
    std::set<Operand> defs;  // 存储bb中定义的操作数
    
    // 收集bb中的定义
    for (auto I : bb->Instruction_list) {
        if (I->GetOpcode() == BasicInstruction::PHI) { // PHI指令使优化复杂化
            return false;
        } else if (I->GetResult() != nullptr) {
            defs.insert(I->GetResult()); // 记录定义
        }
    }
    
    // 检查是否存在存储或调用（可能有副作用）
    for (auto I : bb->Instruction_list) {
        if (I->GetOpcode() == BasicInstruction::STORE || I->GetOpcode() == BasicInstruction::CALL) {
            return false;
        }
    }

    // 检查目标块是否包含PHI指令
    auto bb2 = (*C->block_map)[st_id];
    for (auto I : bb2->Instruction_list) {
        if (I->GetOpcode() == BasicInstruction::PHI) {
            return false;
        }
    }

    // BFS遍历从st_id开始的所有路径（跳过bb_id）
    std::vector<int> vis;
    std::queue<int> q;
    vis.resize(C->max_label + 1);
    q.push(st_id);

    while (!q.empty()) {
        auto x = q.front();
        q.pop();
        if (vis[x]) continue;
        vis[x] = true;
        if (x == bb_id) continue;  // 跳过定义块本身
        
        auto bbx = (*C->block_map)[x];
        // 检查操作数是否使用bb中的定义
        for (auto I : bbx->Instruction_list) {
            for (auto op : I->GetNonResultOperands()) {
                if (defs.find(op) != defs.end()) {
                    return false;  // 发现使用，不满足条件
                }
            }
        }

        // 后继入队
        for (auto bb : C->GetSuccessor(x)) {
            q.push(bb->block_id);
        }
    }
    //std::cout<<"end BlockDefNoUseCheck\n";
    return true;  // 满足"无定义使用"条件
}
// static bool EmptyBlockJumping(CFG *C) {
//     bool optimized = false;
//     auto dom_tree = domtrees->GetDomTree(C);
    
//     // 使用工作列表处理候选块
//     std::vector<std::pair<LLVMBlock, LLVMBlock>> candidates;
//     for (auto& [id, bb] : *C->block_map) {
//         if (bb->Instruction_list.size() < 2) continue;
        
//         auto cmp_instr = bb->Instruction_list.end() - 2;
//         for (auto succ : C->GetSuccessor(id)) {
//             if (succ->Instruction_list.size() >= 2 && 
//                 dom_tree->dominates(bb, succ) && 
//                 bb != succ) {
//                 candidates.emplace_back(bb, succ);
//             }
//         }
//     }

//     for (auto& [bb, bb2] : candidates) {
//         auto cmp1 = *(bb->Instruction_list.end() - 2);
//         auto cmp2 = *(bb2->Instruction_list.end() - 2);
        
//         // 比较指令匹配检查
//         if (cmp1->GetOpcode() != cmp2->GetOpcode()) continue;
        
//         Operand op1_1, op1_2, op2_1, op2_2;
//         int cond1 = 0, cond2 = 0;
        
//         if (auto icmp = dynamic_cast<IcmpInstruction*>(cmp1)) {
//             op1_1 = icmp->GetOp1(); op1_2 = icmp->GetOp2(); cond1 = icmp->GetCompareCondition();
//             auto icmp2 = dynamic_cast<IcmpInstruction*>(cmp2);
//             if (!icmp2) continue;
//             op2_1 = icmp2->GetOp1(); op2_2 = icmp2->GetOp2(); cond2 = icmp2->GetCompareCondition();
//         } 
//         else if (auto fcmp = dynamic_cast<FcmpInstruction*>(cmp1)) {
//             op1_1 = fcmp->GetOp1(); op1_2 = fcmp->GetOp2(); cond1 = fcmp->GetCompareCondition();
//             auto fcmp2 = dynamic_cast<FcmpInstruction*>(cmp2);
//             if (!fcmp2) continue;
//             op2_1 = fcmp2->GetOp1(); op2_2 = fcmp2->GetOp2(); cond2 = fcmp2->GetCompareCondition();
//         }
//         else continue;
        
//         if (op1_1->GetFullName() != op2_1->GetFullName() ||
//             op1_2->GetFullName() != op2_2->GetFullName() ||
//             cond1 != cond2) continue;
//         if(bb->Instruction_list.back()->GetOpcode()!=BasicInstruction::BR_COND ){continue;}
//         if(bb2->Instruction_list.back()->GetOpcode()!=BasicInstruction::BR_COND ){continue;}
//         auto br1 = static_cast<BrCondInstruction*>(bb->Instruction_list.back());
//         auto br2 = static_cast<BrCondInstruction*>(bb2->Instruction_list.back());
        
//         int target_id = (static_cast<LabelOperand*>(br1->GetTrueLabel())->GetLabelNo() == bb2->block_id)
//             ? static_cast<LabelOperand*>(br2->GetTrueLabel())->GetLabelNo()
//             : static_cast<LabelOperand*>(br2->GetFalseLabel())->GetLabelNo();
        
//         if (!BlockDefNoUseCheck(C, bb2->block_id, target_id)) continue;
        
//         // 优化处理
//         if (static_cast<LabelOperand*>(br1->GetTrueLabel())->GetLabelNo() == bb2->block_id) {
//             br1->SetTrueLabel(br2->GetTrueLabel());
//             bb2->Instruction_list.pop_back();
//             bb2->Instruction_list.pop_back();
//             auto new_cmp = new IcmpInstruction(BasicInstruction::I32, 
//                 new ImmI32Operand(1), new ImmI32Operand(0), 
//                 BasicInstruction::eq, cmp2->GetResult());
//             bb2->InsertInstruction(bb2->Instruction_list.size(), new_cmp);
//             bb2->InsertInstruction(bb2->Instruction_list.size(), br2);
//         } else {
//             br1->SetFalseLabel(br2->GetFalseLabel());
//             bb2->Instruction_list.pop_back();
//             bb2->Instruction_list.pop_back();
//             auto new_cmp = new IcmpInstruction(BasicInstruction::I32, 
//                 new ImmI32Operand(1), new ImmI32Operand(1), 
//                 BasicInstruction::eq, cmp2->GetResult());
//             bb2->InsertInstruction(bb2->Instruction_list.size(), new_cmp);
//             bb2->InsertInstruction(bb2->Instruction_list.size(), br2);
//         }
//         optimized = true;
//     }
//     return optimized;
// }

// static bool EmptyBlockJumping(CFG *C) {
//     bool optimized = false;
//     auto dom_tree = domtrees->GetDomTree(C);
    
//     // 使用工作列表处理候选块
//     std::vector<std::pair<LLVMBlock, LLVMBlock>> candidates;
//     for (auto& [id, bb] : *C->block_map) {
//         if (bb->Instruction_list.size() < 2) continue;
        
//         auto cmp_instr = bb->Instruction_list.end() - 2;
//         for (auto succ : C->GetSuccessor(id)) {
//             if (succ->Instruction_list.size() >= 2 && 
//                 dom_tree->dominates(bb, succ) && 
//                 bb != succ) {
//                 candidates.emplace_back(bb, succ);
//             }
//         }
//     }

//     for (auto& [bb, bb2] : candidates) {
//         auto cmp1 = *(bb->Instruction_list.end() - 2);
//         auto cmp2 = *(bb2->Instruction_list.end() - 2);
        
//         // 比较指令匹配检查
//         if (cmp1->GetOpcode() != cmp2->GetOpcode()) continue;
        
//         Operand op1_1, op1_2, op2_1, op2_2;
//         int cond1 = 0, cond2 = 0;
        
//         if (auto icmp = dynamic_cast<IcmpInstruction*>(cmp1)) {
//             op1_1 = icmp->GetOp1(); op1_2 = icmp->GetOp2(); cond1 = icmp->GetCompareCondition();
//             auto icmp2 = dynamic_cast<IcmpInstruction*>(cmp2);
//             if (!icmp2) continue;
//             op2_1 = icmp2->GetOp1(); op2_2 = icmp2->GetOp2(); cond2 = icmp2->GetCompareCondition();
//         } 
//         else if (auto fcmp = dynamic_cast<FcmpInstruction*>(cmp1)) {
//             op1_1 = fcmp->GetOp1(); op1_2 = fcmp->GetOp2(); cond1 = fcmp->GetCompareCondition();
//             auto fcmp2 = dynamic_cast<FcmpInstruction*>(cmp2);
//             if (!fcmp2) continue;
//             op2_1 = fcmp2->GetOp1(); op2_2 = fcmp2->GetOp2(); cond2 = fcmp2->GetCompareCondition();
//         }
//         else continue;
        
//         if (op1_1->GetFullName() != op2_1->GetFullName() ||
//             op1_2->GetFullName() != op2_2->GetFullName() ||
//             cond1 != cond2) continue;
//         if(bb->Instruction_list.back()->GetOpcode()!=BasicInstruction::BR_COND ){continue;}
//         if(bb2->Instruction_list.back()->GetOpcode()!=BasicInstruction::BR_COND ){continue;}
//         auto br1 = static_cast<BrCondInstruction*>(bb->Instruction_list.back());
//         auto br2 = static_cast<BrCondInstruction*>(bb2->Instruction_list.back());
        
//         int target_id = (static_cast<LabelOperand*>(br1->GetTrueLabel())->GetLabelNo() == bb2->block_id)
//             ? static_cast<LabelOperand*>(br2->GetTrueLabel())->GetLabelNo()
//             : static_cast<LabelOperand*>(br2->GetFalseLabel())->GetLabelNo();
        
//         if (!BlockDefNoUseCheck(C, bb2->block_id, target_id)) continue;
        
//         // 优化处理
//         if (static_cast<LabelOperand*>(br1->GetTrueLabel())->GetLabelNo() == bb2->block_id) {
//             br1->SetTrueLabel(br2->GetTrueLabel());
//             bb2->Instruction_list.pop_back();
//             bb2->Instruction_list.pop_back();
//             auto new_cmp = new IcmpInstruction(BasicInstruction::I32, 
//                 new ImmI32Operand(1), new ImmI32Operand(0), 
//                 BasicInstruction::eq, cmp2->GetResult());
//             bb2->InsertInstruction(bb2->Instruction_list.size(), new_cmp);
//             bb2->InsertInstruction(bb2->Instruction_list.size(), br2);
//         } else {
//             br1->SetFalseLabel(br2->GetFalseLabel());
//             bb2->Instruction_list.pop_back();
//             bb2->Instruction_list.pop_back();
//             auto new_cmp = new IcmpInstruction(BasicInstruction::I32, 
//                 new ImmI32Operand(1), new ImmI32Operand(1), 
//                 BasicInstruction::eq, cmp2->GetResult());
//             bb2->InsertInstruction(bb2->Instruction_list.size(), new_cmp);
//             bb2->InsertInstruction(bb2->Instruction_list.size(), br2);
//         }
//         optimized = true;
//     }
//     return optimized;
// }
// 优化空块的条件跳转
bool DomTreeCSEOptimizer::EmptyBlockJumping(CFG *C) {
    bool flag = false;  // 优化发生标志
    for (auto [id, bb] : *C->block_map) {
        if (bb->Instruction_list.size() < 2) continue;  // 跳过指令不足的块
        
        Operand op1_1, op1_2;
        int cond1 = 0;
        auto I = *(bb->Instruction_list.end() - 2);  // 倒数第二条指令（比较指令）
        
        // 提取整型比较信息
        if (I->GetOpcode() == BasicInstruction::ICMP) {
            auto IcmpI = (IcmpInstruction *)I;
            op1_1 = IcmpI->GetOp1();
            op1_2 = IcmpI->GetOp2();
            cond1 = IcmpI->GetCompareCondition();
        } 
        // 提取浮点比较信息
        else if (I->GetOpcode() == BasicInstruction::FCMP) {
            auto FcmpI = (FcmpInstruction *)I;
            op1_1 = FcmpI->GetOp1();
            op1_2 = FcmpI->GetOp2();
            cond1 = FcmpI->GetCompareCondition();
        } else {
            continue;  // 非比较指令跳过
        }
        // 遍历后继块
        for (auto bb2 : C->GetSuccessor(id)) {
            // 支配性检查
            if (!domtrees->GetDomTree(C)->dominates(bb, bb2) || bb2 == bb) continue;
            if (bb2->Instruction_list.size() < 2) continue;
            
            Operand op2_1, op2_2;
            int cond2 = 0;
            auto I2 = *(bb2->Instruction_list.end() - 2);
            // 指令类型匹配检查
            if (I->GetOpcode() != I2->GetOpcode()) continue;
            
            // 提取后继块的比较信息
            if (I2->GetOpcode() == BasicInstruction::ICMP) {
                auto IcmpI2 = (IcmpInstruction *)I2;
                op2_1 = IcmpI2->GetOp1();
                op2_2 = IcmpI2->GetOp2();
                cond2 = IcmpI2->GetCompareCondition();
            } else if (I2->GetOpcode() == BasicInstruction::FCMP) {
                auto FcmpI2 = (FcmpInstruction *)I2;
                op2_1 = FcmpI2->GetOp1();
                op2_2 = FcmpI2->GetOp2();
                cond2 = FcmpI2->GetCompareCondition();
            } else continue;
            
            // 操作数和条件匹配检查
            if (op1_1->GetFullName() != op2_1->GetFullName() || 
                op1_2->GetFullName() != op2_2->GetFullName() || 
                cond1 != cond2) {
                continue;
            }
            
            if(bb->Instruction_list.back()->GetOpcode()!=BasicInstruction::BR_COND ){continue;}
            if(bb2->Instruction_list.back()->GetOpcode()!=BasicInstruction::BR_COND ){continue;}
            auto brI1 = (BrCondInstruction *)(bb->Instruction_list.back());
            auto brI2 = (BrCondInstruction *)(bb2->Instruction_list.back());
           
            // True分支优化
            if (((LabelOperand *)brI1->GetTrueLabel())->GetLabelNo() == bb2->block_id) {
                // 检查目标块是否可优化
                if (!BlockDefNoUseCheck(C, bb2->block_id, ((LabelOperand *)brI2->GetTrueLabel())->GetLabelNo())) {
                    continue;
                }
                // 重定向True分支
                brI1->SetTrueLabel(((LabelOperand *)brI2->GetTrueLabel()));
                // 替换后继块中的指令为常量比较
                bb2->Instruction_list.pop_back();
                bb2->Instruction_list.pop_back();
                auto NIcmpI2 = new IcmpInstruction(BasicInstruction::I32, new ImmI32Operand(1), new ImmI32Operand(0), BasicInstruction::eq, I2->GetResult());
                bb2->InsertInstruction(1, NIcmpI2);  // 插入恒假比较
                bb2->InsertInstruction(1, brI2);     // 放回分支指令
                flag = true;  // 标记优化发生
            } 
            // False分支优化
            else {
                if (!BlockDefNoUseCheck(C, bb2->block_id, ((LabelOperand *)brI2->GetFalseLabel())->GetLabelNo())) {
                    continue;
                }
                brI1->SetFalseLabel(((LabelOperand *)brI2->GetFalseLabel()));
                bb2->Instruction_list.pop_back();
                bb2->Instruction_list.pop_back();
                auto NIcmpI2 = new IcmpInstruction(BasicInstruction::I32, new ImmI32Operand(1), new ImmI32Operand(1), BasicInstruction::eq, I2->GetResult());
                bb2->InsertInstruction(1, NIcmpI2);  // 插入恒真比较
                bb2->InsertInstruction(1, brI2);
                flag = true;
            }
        }
    }
    return flag;  // 返回优化状态
}

// 使用BFS检查基本块bb1是否能到达bb2
static bool CanReach(int bb1_id, int bb2_id, CFG *C) {
    std::vector<int> vis;      // 访问标记数组
    std::queue<int> q;         // BFS队列

    vis.resize(C->max_label + 1); // 根据最大标签初始化
    q.push(bb1_id);            // 起始块入队

    while (!q.empty()) {
        auto x = q.front();
        q.pop();
        if (x == bb2_id) {     // 找到目标块
            return true;
        }
        if (vis[x]) continue;  // 已访问跳过
        vis[x] = true;         // 标记已访问

        // 遍历所有后继块
        for (auto bb : C->GetSuccessor(x)) {
            q.push(bb->block_id);
        }
    }
    return false;  // 未找到路径
}

// 检查条件分支是否可优化（用于CSE）
static bool CanJump(bool isleft, int x1_id, int x2_id, CFG *C) {
    // 前置条件：x1支配x2
    //assert(C->dominates(x1_id, x2_id));
    auto x1 = (*C->block_map)[x1_id];
    auto x2 = (*C->block_map)[x2_id];
    auto BrI1 = (BrCondInstruction *)(*(x1->Instruction_list.end() - 1)); // 获取分支指令
    
    // 解析真假目标块
    auto xT = (*C->block_map)[((LabelOperand *)BrI1->GetTrueLabel())->GetLabelNo()];
    auto xF = (*C->block_map)[((LabelOperand *)BrI1->GetFalseLabel())->GetLabelNo()];

    if (isleft) {  // 处理True分支情况
        // 检查路径存在性
        if (!CanReach(xT->block_id, x2->block_id, C) && CanReach(xF->block_id, x2->block_id, C)) {
            // 修改x2的指令：用常量比较替换原比较
            auto tmpI1 = *(x2->Instruction_list.end() - 1);
            auto tmpI2 = *(x2->Instruction_list.end() - 2);
            x2->Instruction_list.pop_back();
            x2->Instruction_list.pop_back();
            // 创建恒假比较 (0==1)
            auto IcmpI = new IcmpInstruction(BasicInstruction::I32, new ImmI32Operand(0), new ImmI32Operand(1), BasicInstruction::eq, tmpI2->GetResult());
            x2->InsertInstruction(1, IcmpI); // 插入新指令
            x2->InsertInstruction(1, tmpI1);  // 放回原分支指令
            return true;  // 优化成功
        }
    } else {  // 处理False分支情况
        if (CanReach(xT->block_id, x2->block_id, C) && !CanReach(xF->block_id, x2->block_id, C)) {
            // 创建恒真比较 (1==1)
            auto tmpI1 = *(x2->Instruction_list.end() - 1);
            auto tmpI2 = *(x2->Instruction_list.end() - 2);
            x2->Instruction_list.pop_back();
            x2->Instruction_list.pop_back();
            auto IcmpI = new IcmpInstruction(BasicInstruction::I32, new ImmI32Operand(1), new ImmI32Operand(1), BasicInstruction::eq, tmpI2->GetResult());
            x2->InsertInstruction(1, IcmpI);
            x2->InsertInstruction(1, tmpI1);
            return true;
        }
    }
    return false;  // 无优化
}
// // 使用DFS检查基本块可达性
// static bool CanReach(int bb1_id, int bb2_id, CFG *C) {
//     if (bb1_id == bb2_id) return true;
//     std::vector<bool> visited(C->max_label + 1, false);
//     std::stack<int> stack;
//     stack.push(bb1_id);
//     visited[bb1_id] = true;

//     while (!stack.empty()) {
//         int current = stack.top();
//         stack.pop();
//         for (auto bb : C->GetSuccessor(current)) {
//             int next_id = bb->block_id;
//             if (next_id == bb2_id) return true;
//             if (!visited[next_id]) {
//                 visited[next_id] = true;
//                 stack.push(next_id);
//             }
//         }
//     }
//     return false;
// }

// // 封装指令修改操作
// static void ReplaceInstructions(LLVMBlock x2, bool constant_value) {
//     auto& instr_list = x2->Instruction_list;
//     auto last_instr = instr_list.back();
//     auto cmp_instr = *(instr_list.end() - 2);
//     instr_list.pop_back();
//     instr_list.pop_back();

//     auto op1 = new ImmI32Operand(constant_value ? 1 : 0);
//     auto op2 = new ImmI32Operand(1);
//     auto new_cmp = new IcmpInstruction(
//         BasicInstruction::I32, op1, op2, 
//         BasicInstruction::eq, cmp_instr->GetResult()
//     );

//     x2->InsertInstruction(instr_list.size(), new_cmp);
//     x2->InsertInstruction(instr_list.size() + 1, last_instr);
// }

// // 优化条件分支
// static bool CanJump(bool isleft, int x1_id, int x2_id, CFG *C) {
//     auto blk1 = (*C->block_map)[x1_id];
//     auto blk2 = (*C->block_map)[x2_id];
//     auto br_instr = static_cast<BrCondInstruction*>(blk1->Instruction_list.back());
    
//     int true_target = static_cast<LabelOperand*>(br_instr->GetTrueLabel())->GetLabelNo();
//     int false_target = static_cast<LabelOperand*>(br_instr->GetFalseLabel())->GetLabelNo();

//     bool true_path = CanReach(true_target, x2_id, C);
//     bool false_path = CanReach(false_target, x2_id, C);

//     if (isleft && !true_path && false_path) {
//         ReplaceInstructions(blk2, false); // false condition
//         return true;
//     }
//     if (!isleft && true_path && !false_path) {
//         ReplaceInstructions(blk2, true); // true condition
//         return true;
//     }
//     return false;
// }

// DomTreeCSEOptimizer成员函数实现
void DomTreeCSEOptimizer::optimize() {
    // while (changed||branch_changed) {
    //     CmpMap.clear();
    //     changed = false;
    //     branch_changed=false;
    //     dfs(0);
    //     branch_dfs(0);
    //     removeDeadInstructions();
    //     applyRegisterReplacements();
    //     branch_changed |= EmptyBlockJumping(C);  // 合并优化结果
        
    //     // 重建CFG和支配树
    //     C->BuildCFG();
    //     DomInfo[C] = new DominatorTree(C);
    //     DomInfo[C]->BuildDominatorTree(false);
    //     C->DomTree = DomInfo[C]; 
    // }
    while (changed) {
        changed = false;
        dfs(0);
        removeDeadInstructions();
        applyRegisterReplacements();
    }
    // changed=true;
    // while (changed) {
    //     CmpMap.clear();
    //     changed = false;
    //     branch_dfs(0);
    //     changed |= EmptyBlockJumping(C);  // 合并优化结果
        
    //     // 重建CFG和支配树
    //     C->BuildCFG();
    //     DomInfo[C] = new DominatorTree(C);
    //     DomInfo[C]->BuildDominatorTree(false);
    //     C->DomTree = DomInfo[C]; 
    // }

}
void DomTreeCSEOptimizer::branch_optimize()
{
    changed=true;
    while (changed) {
        CmpMap.clear();
        changed = false;
        branch_dfs(0);
        branch_end();
    }

}
void DomTreeCSEOptimizer::branch_end()
{
    changed |= EmptyBlockJumping(C);  // 合并优化结果
    // 重建CFG和支配树
    C->BuildCFG();
    DomInfo[C] = new DominatorTree(C);
    DomInfo[C]->BuildDominatorTree(false);
    C->DomTree = DomInfo[C]; 
}
// void DomTreeCSEOptimizer::dfs(int bbid) {
//     LLVMBlock now = (*C->block_map)[bbid];
//     std::set<InstCSEInfo> regularCseSet;
//     std::map<InstCSEInfo, int> tmpLoadNumMap;
//     std::set<InstCSEInfo> cmpCseSet;
//     int ins_size=now->Instruction_list.size();
//     for(int i=0;i<ins_size-2;++i)
//     {
//         auto I=now->Instruction_list[i];
//         if (!CSENotConsider(I)) continue;

//         switch (I->GetOpcode()) {
//             case BasicInstruction::LOAD:
//                 processLoadInstruction(static_cast<LoadInstruction*>(I), tmpLoadNumMap);
//                 break;
//             case BasicInstruction::STORE:
//                 processStoreInstruction(static_cast<StoreInstruction*>(I), tmpLoadNumMap);
//                 break;
//             case BasicInstruction::CALL:
//                 processCallInstruction(static_cast<CallInstruction*>(I));
//                 break;
//             default:
//                 processRegularInstruction(I, regularCseSet);
//                 break;
//         }
//     }
//     if(ins_size>=2)
//     {
//         for(int i=ins_size-2;i<ins_size;++i)
//         {
//             auto I=now->Instruction_list[i];
//             if (!BranchCSENotConsider(I)) continue;

//             switch (I->GetOpcode()) {
//                 case BasicInstruction::LOAD:
//                     processLoadInstruction(static_cast<LoadInstruction*>(I), tmpLoadNumMap);
//                     break;
//                 case BasicInstruction::STORE:
//                     processStoreInstruction(static_cast<StoreInstruction*>(I), tmpLoadNumMap);
//                     break;
//                 case BasicInstruction::CALL:
//                     processCallInstruction(static_cast<CallInstruction*>(I));
//                     break;
//                 case BasicInstruction::ICMP:
//                     if(i==ins_size-2){processIcmpInstruction(static_cast<IcmpInstruction*>(I),cmpCseSet);}
//                     break;
//                 case BasicInstruction::FCMP:
//                     if(i==ins_size-2){processFcmpInstruction(static_cast<FcmpInstruction*>(I),cmpCseSet);}
//                     break;
//                 default:
//                     processRegularInstruction(I, regularCseSet);
//                     break;
//             }
//         }
//     }
    
    
//     for (auto v : domtrees->GetDomTree(C)->dom_tree[bbid]) {
//         dfs(v->block_id);
//     }

//     //cleanupTemporaryEntries(regularCseSet, tmpLoadNumMap);
//     cleanupTemporaryEntries(regularCseSet,cmpCseSet);
// }
void DomTreeCSEOptimizer::dfs(int bbid) {
    LLVMBlock now = (*C->block_map)[bbid];
    std::set<InstCSEInfo> tmpCSESet;
    std::map<InstCSEInfo, int> tmpLoadNumMap;

    for (auto I : now->Instruction_list) {
        if (!CSENotConsider(I)) continue;

        switch (I->GetOpcode()) {
            case BasicInstruction::LOAD:
                processLoadInstruction(static_cast<LoadInstruction*>(I), tmpLoadNumMap);
                break;
            case BasicInstruction::STORE:
                processStoreInstruction(static_cast<StoreInstruction*>(I), tmpLoadNumMap);
                break;
            case BasicInstruction::CALL:
                processCallInstruction(static_cast<CallInstruction*>(I));
                break;
            default:
                processRegularInstruction(I, tmpCSESet);
                break;
        }
    }
    
    for (auto v : C->getDomTree()->dom_tree[bbid]) {
        dfs(v->block_id);
    }

    //cleanupTemporaryEntries(tmpCSESet, tmpLoadNumMap);
    cleanupTemporaryEntries(tmpCSESet);
}
void DomTreeCSEOptimizer::branch_dfs(int bbid)
{
    LLVMBlock now = (*C->block_map)[bbid];
    std::set<InstCSEInfo> cmpCseSet;  // 临时存储当前块的CSE信息
    
    // 1）检查当前块尾部是否存在比较指令
    if (now->Instruction_list.size() >= 2) {
        auto I = *(now->Instruction_list.end() - 2);
        //2)如果存在比较指令，提取CSE信息
        if (I->GetOpcode() == BasicInstruction::FCMP || I->GetOpcode() == BasicInstruction::ICMP) {
            auto info = GetBranchCSEInfo(I);  // 提取CSE信息
            bool isConstCmp = false;
            
            //3）检查是否为整型常量比较（？不可消除）
            if (I->GetOpcode() == BasicInstruction::ICMP) {
                auto IcmpI = (IcmpInstruction *)I;
                if (IcmpI->GetOp1()->GetOperandType() == BasicOperand::IMMI32 &&
                    IcmpI->GetOp2()->GetOperandType() == BasicOperand::IMMI32) {
                    isConstCmp = true;
                }
            }
            
            bool isCSE = false;
            //4）查找可用的公共表达式（非常量cmp且已经在cmpmap中记录）
            if (!isConstCmp && CmpMap.find(info) != CmpMap.end()) {
                for (auto I2 : CmpMap[info]) {
                    // 检查支配关系下的跳转可能性
                    if (CanJump(1, I2->GetBlockID(), I->GetBlockID(), C) || 
                        CanJump(0, I2->GetBlockID(), I->GetBlockID(), C)) {
                        isCSE = true;
                        break;
                    }
                }
            }
            // 5）非常量比较且无CSE时加入映射
            if (!isCSE && !isConstCmp) {
                cmpCseSet.insert(info);
                CmpMap[info].push_back(I);
            }
        }
    }
    

    // 6）递归处理支配树中的子节点
    for (auto v : domtrees->GetDomTree(C)->dom_tree[bbid]) {
        dfs(v->block_id);
    }

    //7） 回溯：移除当前块添加的CSE信息
    for (auto info : cmpCseSet) {
        CmpMap[info].pop_back();
    }
}


void DomTreeCSEOptimizer::processIcmpInstruction(IcmpInstruction* IcmpI,std::set<InstCSEInfo>& cmpCseSet)
{
    auto info = GetBranchCSEInfo(IcmpI);  // 提取CSE信息
    bool isConstCmp = false;
    //检查是否为整型常量比较（？不可消除）(是否需要扩展成浮点数常量比较？)
    if (IcmpI->GetOp1()->GetOperandType() == BasicOperand::IMMI32 &&
                IcmpI->GetOp2()->GetOperandType() == BasicOperand::IMMI32) {
        isConstCmp = true;
    }  
    bool isCSE = false;
    //4）查找可用的公共表达式（非常量cmp且已经在cmpmap中记录）
    if (!isConstCmp && CmpMap.find(info) != CmpMap.end()) {
        for (auto I2 : CmpMap[info]) {
            // 检查支配关系下的跳转可能性
            if (CanJump(1, I2->GetBlockID(), IcmpI->GetBlockID(), C) || 
                CanJump(0, I2->GetBlockID(), IcmpI->GetBlockID(), C)) {
                isCSE = true;
                break;
            }
        }
    }
    // 5）非常量比较且无CSE时加入映射
    if (!isCSE && !isConstCmp) {
        cmpCseSet.insert(info);
        CmpMap[info].push_back(IcmpI);
    }
}
void DomTreeCSEOptimizer::processFcmpInstruction(FcmpInstruction* FcmpI,std::set<InstCSEInfo>& cmpCseSet)
{
    auto info = GetBranchCSEInfo(FcmpI);  // 提取CSE信息
    bool isConstCmp = false;
    //检查是否为整型常量比较（？不可消除）(是否需要扩展成浮点数常量比较？)
    if (FcmpI->GetOp1()->GetOperandType() == BasicOperand::IMMF32 &&
                FcmpI->GetOp2()->GetOperandType() == BasicOperand::IMMF32) {
        isConstCmp = true;
    }  
    bool isCSE = false;
    //4）查找可用的公共表达式（非常量cmp且已经在cmpmap中记录）
    if (!isConstCmp && CmpMap.find(info) != CmpMap.end()) {
        for (auto I2 : CmpMap[info]) {
            // 检查支配关系下的跳转可能性
            if (CanJump(1, I2->GetBlockID(), FcmpI->GetBlockID(), C) || 
                CanJump(0, I2->GetBlockID(), FcmpI->GetBlockID(), C)) {
                isCSE = true;
                break;
            }
        }
    }
    // 5）非常量比较且无CSE时加入映射
    if (!isCSE && !isConstCmp) {
        cmpCseSet.insert(info);
        CmpMap[info].push_back(FcmpI);
    }

}
void DomTreeCSEOptimizer::processLoadInstruction(LoadInstruction* loadI, std::map<InstCSEInfo, int>& tmpLoadNumMap) {
    // 加载指令处理逻辑（待实现）
}

void DomTreeCSEOptimizer::processStoreInstruction(StoreInstruction* storeI, std::map<InstCSEInfo, int>& tmpLoadNumMap) {
    // 存储指令处理逻辑（待实现）
}

void DomTreeCSEOptimizer::processCallInstruction(CallInstruction* callI) {
    // 调用指令处理逻辑（待实现）
}

void DomTreeCSEOptimizer::processRegularInstruction(BasicInstruction* I, std::set<InstCSEInfo>& regularCseSet) {
    auto info = GetCSEInfo(I);
    auto cseIter = instCSEMap.find(info);

    if (cseIter != instCSEMap.end()) {
        eraseSet.insert(I);
        regReplaceMap[GetResultRegNo(I)] = cseIter->second;
        changed|= true;
    } else {
        instCSEMap[info] = GetResultRegNo(I);
        regularCseSet.insert(info);
    }
}

// void DomTreeCSEOptimizer::cleanupTemporaryEntries(const std::set<InstCSEInfo>& regularCseSet, 
//                                                 const std::map<InstCSEInfo, int>& tmpLoadNumMap) {
//     for (auto info : regularCseSet) {
//         instCSEMap.erase(info);
//     }

//     // for (auto [info, num] : tmpLoadNumMap) {
//     //     auto& loadList = loadCSEMap[info];
//     //     loadList.erase(loadList.end() - num, loadList.end());
//     //     if (loadList.empty()) {
//     //         loadCSEMap.erase(info);
//     //     }
//     // }
// }
// void DomTreeCSEOptimizer::cleanupTemporaryEntries(const std::set<InstCSEInfo>& regularCseSet,const std::set<InstCSEInfo>& cmpCseSet) {
//     for (auto info : regularCseSet) {
//         instCSEMap.erase(info);
//     }
//     for (auto info : cmpCseSet) {
//         CmpMap[info].pop_back();
//     }
// }
void DomTreeCSEOptimizer::cleanupTemporaryEntries(const std::set<InstCSEInfo>& regularCseSet) {
    for (auto info : regularCseSet) {
        instCSEMap.erase(info);
    }
}
void DomTreeCSEOptimizer::removeDeadInstructions() {
    for (auto [id, bb] : *C->block_map) {
        std::deque<Instruction> newInstructions;
        for (auto I : bb->Instruction_list) {
            if (!eraseSet.count(I)) {
                newInstructions.push_back(I);
            }
        }
        bb->Instruction_list = newInstructions;
    }
    eraseSet.clear();
    // for (auto [id, bb] : *C->block_map) {
    //     auto tmp_Instruction_list = bb->Instruction_list;
    //     bb->Instruction_list.clear();
    //     for (auto I : tmp_Instruction_list) {
    //         if (eraseSet.find(I) != eraseSet.end()) {
    //             continue;
    //         }
    //         bb->InsertInstruction(1, I);
    //     }
    // }
    // eraseSet.clear();
}

void DomTreeCSEOptimizer::applyRegisterReplacements() {
    for (auto [id, bb] : *C->block_map) {
        for (auto I : bb->Instruction_list) {
            I->ChangeResult(regReplaceMap);
            I->ChangeReg(regReplaceMap,regReplaceMap);
        }
    }
    regReplaceMap.clear();
}

// 支配树CSE优化入口
void SimpleCSEPass::SimpleDomTreeWalkCSE(CFG* C) {
    DomTreeCSEOptimizer optimizer(C);
    optimizer.optimize();
}

// 整体CSE优化入口
void SimpleCSEPass::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        CSEInit(cfg);
        BasicBlockCSEOptimizer optimizer(cfg);
        optimizer.optimize();
        DomTreeCSEOptimizer optimizer2(cfg);
        optimizer2.optimize();
    }
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        CSEInit(cfg);
        DomTreeCSEOptimizer optimizer2(cfg);
        optimizer2.branch_optimize();
    }
}
void SimpleCSEPass::BlockExecute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        CSEInit(cfg);
        SimpleBlockCSE(cfg);
    }
}
