#include "basic_cse.h"
#include <algorithm>
#include <cassert>
#include<functional>
int GetResultRegNo(Instruction inst)
{
    auto result = (RegOperand*)inst->GetResult();
    if(result!=nullptr){return result->GetRegNo();}
    return -1;
}

// InstCSEInfo的比较运算符实现
bool InstCSEInfo::operator<(const InstCSEInfo &other) const {
    if (opcode != other.opcode) 
        return opcode < other.opcode;
    
    if (operand_list.size() != other.operand_list.size())
        return operand_list.size() < other.operand_list.size();
    
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

// DomTreeCSEOptimizer成员函数实现
void DomTreeCSEOptimizer::optimize() {
    while (changed) {
        changed = false;
        dfs(0);
        removeDeadInstructions();
        applyRegisterReplacements();
    }
}

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

void DomTreeCSEOptimizer::processLoadInstruction(LoadInstruction* loadI, std::map<InstCSEInfo, int>& tmpLoadNumMap) {
    // 加载指令处理逻辑（待实现）
}

void DomTreeCSEOptimizer::processStoreInstruction(StoreInstruction* storeI, std::map<InstCSEInfo, int>& tmpLoadNumMap) {
    // 存储指令处理逻辑（待实现）
}

void DomTreeCSEOptimizer::processCallInstruction(CallInstruction* callI) {
    // 调用指令处理逻辑（待实现）
}

void DomTreeCSEOptimizer::processRegularInstruction(BasicInstruction* I, std::set<InstCSEInfo>& tmpCSESet) {
    auto info = GetCSEInfo(I);
    auto cseIter = instCSEMap.find(info);

    if (cseIter != instCSEMap.end()) {
        eraseSet.insert(I);
        regReplaceMap[GetResultRegNo(I)] = cseIter->second;
        changed|= true;
    } else {
        instCSEMap[info] = GetResultRegNo(I);
        tmpCSESet.insert(info);
    }
}

// void DomTreeCSEOptimizer::cleanupTemporaryEntries(const std::set<InstCSEInfo>& tmpCSESet, 
//                                                 const std::map<InstCSEInfo, int>& tmpLoadNumMap) {
//     for (auto info : tmpCSESet) {
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
void DomTreeCSEOptimizer::cleanupTemporaryEntries(const std::set<InstCSEInfo>& tmpCSESet) {
    for (auto info : tmpCSESet) {
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
}
void SimpleCSEPass::BlockExecute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        CSEInit(cfg);
        SimpleBlockCSE(cfg);
    }
}