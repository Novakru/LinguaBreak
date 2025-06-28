#include "functioninline.h"
#include <functional>

//记录调用图callGraph和函数大小funcSize
void FunctionInlinePass::buildCallGraph() {
    for (auto &func : llvmIR->function_block_map) {
        FuncDefInstruction caller = func.first;//调用者
        for (auto &block : func.second) {
            for (auto &inst : block.second->Instruction_list) {
                if (inst->GetOpcode() == BasicInstruction::LLVMIROpcode::CALL) {
                    CallInstruction* callInst = (CallInstruction*)inst;
                    FuncDefInstruction callee = llvmIR->FunctionNameTable[callInst->GetFunctionName()];//被调用者
                    callGraph[caller].insert(callee);
                }else if(inst->GetOpcode() == BasicInstruction::LLVMIROpcode::RET){
                    RetInstruction* retInst = (RetInstruction*)inst;
                    auto id_ret = std::make_pair(block.first, *retInst);
                    calleeReturn[caller] = id_ret;
                }
            }
        }
        // 计算函数大小
        int size = 0;
        for (auto &block : func.second) {
            size += block.second->Instruction_list.size();
        }
        funcSize[caller] = size;
    }
}

bool FunctionInlinePass::shouldInline(FuncDefInstruction caller, FuncDefInstruction callee) {
    // 如果函数太大，不进行内联
    if (funcSize[callee] > INLINE_THRESHOLD) {
        return false;
    }
    // 如果存在递归调用，不进行内联
    std::set<FuncDefInstruction> visited;
    std::function<bool(FuncDefInstruction)> hasRecursion = [&](FuncDefInstruction func) {
        if (visited.count(func)) return true;//直接调用自己
        visited.insert(func);
        for (auto &called : callGraph[func]) {//间接调用自己
            if (hasRecursion(called)) return true;
        }
        visited.erase(func);
        return false;
    };
    return !hasRecursion(callee);
}

int FunctionInlinePass::renameRegister(FuncDefInstruction caller,int oldReg, std::map<int, int>& regMapping) {
    if (regMapping.count(oldReg)) {
        return regMapping[oldReg];
    }
    llvmIR->function_max_reg[caller]+=1;//必须超过调用者的label范围
    int newReg = llvmIR->function_max_reg[caller];
    regMapping[oldReg] = newReg;
    return newReg;
}

int FunctionInlinePass::renameLabel(FuncDefInstruction caller,int oldLabel, std::map<int, int>& labelMapping) {
    if (labelMapping.count(oldLabel)) {
        return labelMapping[oldLabel];
    }
    llvmIR->function_max_label[caller]+=1;
    int newLabel = llvmIR->function_max_label[caller];//必须超过调用者的label范围
    labelMapping[oldLabel] = newLabel;
    return newLabel;
}

LLVMBlock FunctionInlinePass::copyBasicBlock(FuncDefInstruction caller,LLVMBlock origBlock, std::map<int, int>& regMapping, std::map<int, int>& labelMapping) {
    int newLabel = renameLabel(caller,origBlock->block_id, labelMapping);
    LLVMBlock newBlock = new BasicBlock(newLabel);
    
    // 复制指令并重命名寄存器和标签
    for (auto &inst : origBlock->Instruction_list) {
        Instruction newInst = inst->InstructionClone();
        // 重命名结果寄存器
        if (inst->GetResult()&& inst->GetResult()->GetOperandType() == BasicOperand::REG) {
            int ResRegNo =((RegOperand*)(inst->GetResult()))->GetRegNo();
            Operand newResOp = GetNewRegOperand(renameRegister(caller,ResRegNo, regMapping));
            newInst->SetResult(newResOp);
        }
        // 重命名操作数寄存器
        auto OldOperands = newInst->GetNonResultOperands();
        std::vector<Operand> nonResultOps{};
        nonResultOps.reserve(OldOperands.size());
        for (auto &op : OldOperands) {
            if (op->GetOperandType() == BasicOperand::REG) {
                int oldReg = ((RegOperand*)op)->GetRegNo();
                Operand newOp = GetNewRegOperand(renameRegister(caller,oldReg, regMapping));
                nonResultOps.emplace_back(newOp);
            } else if (op->GetOperandType() == BasicOperand::LABEL) {
                int oldLabel = ((LabelOperand*)op)->GetLabelNo();
                Operand newOp = GetNewRegOperand(renameRegister(caller,oldLabel, labelMapping));
                nonResultOps.emplace_back(newOp);
            }
        }
        newInst->SetNonResultOperands(nonResultOps);
        newBlock->Instruction_list.push_back(newInst);
    }
    return newBlock;
}

void FunctionInlinePass::inlineFunction(int callerBlockId, LLVMBlock callerBlock,FuncDefInstruction caller, FuncDefInstruction callee, Instruction callInst) {
    std::map<int, int> regMapping;
    std::map<int, int> labelMapping;
    
    // 将被调用函数的参数映射到调用指令的参数
    auto args = callInst->GetNonResultOperands();
    for (int i=0;i< args.size(); i++) {
        if (args[i]->GetOperandType() == BasicOperand::REG) {
            int paramReg = ((RegOperand*)args[i])->GetRegNo();
            regMapping[i] = paramReg;
        }
    }
    
    // 复制被调用函数的所有基本块
    std::map<int, LLVMBlock> inlinedBlocks;
    for (auto &block : llvmIR->function_block_map[callee]) {
        //            原block_id     新block,新id
        inlinedBlocks[block.first] = copyBasicBlock(caller,block.second, regMapping, labelMapping);
    }
    
    // 将内联的基本块添加到调用函数中
    for (auto &block : inlinedBlocks) {
        //                                        新id    新block
        llvmIR->function_block_map[caller][block.first] = block.second;
    }
    
    // 修改调用指令所在基本块的跳转关系  caller_block ---> Inline_Function_EntryBlock
    auto temp_list = callerBlock->Instruction_list;
    callerBlock->Instruction_list.clear();
    std::deque<Instruction> caller_back_list;
    for(int i=0;i< temp_list.size(); i++) {
        Instruction inst = temp_list[i];
        if (inst == callInst) {
            // 保存结果寄存器，删除调用指令
            RegOperand * resultReg ;
            if(callInst->GetResult() && callInst->GetResult()->GetOperandType() == BasicOperand::REG) {
                resultReg = (RegOperand*)callInst->GetResult();
            } else {
                resultReg = nullptr;
            }
            // 添加跳转到内联函数入口块的指令
            BrUncondInstruction* brInst = new BrUncondInstruction(GetNewLabelOperand(labelMapping[0]));// 0是入口块
            callerBlock->Instruction_list.push_back(brInst);
            // 保存后续指令，结束此块
            for(int j = i + 1; j < temp_list.size(); j++) {
                caller_back_list.push_back(temp_list[j]);
            }
            break;
        }else{
            callerBlock->Instruction_list.push_back(inst);
        }
    }
    //处理内联函数的返回值，合并尾块  Inline_Function_ExitBlock ---> caller_block


    // 内联函数的返回值赋给调用指令的结果寄存器


    // 尾块合并

    //后续的phi呢？？？？？

    
    // 更新CFG
    llvmIR->CFGInit();
}

void FunctionInlinePass::Execute() {
    buildCallGraph();
    
    bool changed;
    do {
        changed = false;
        for (auto &func : llvmIR->function_block_map) {
            FuncDefInstruction caller = func.first;
            for (auto &block : func.second) {
                auto it = block.second->Instruction_list.begin();
                while (it != block.second->Instruction_list.end()) {
                    // 如果是函数调用指令
                    if ((*it)->GetOpcode() == BasicInstruction::LLVMIROpcode::CALL) {
                        CallInstruction* callInst = (CallInstruction*)(*it);
                        FuncDefInstruction callee = llvmIR->FunctionNameTable[callInst->GetFunctionName()];//被调用者
                        if (shouldInline(caller, callee)) {
                            int caller_blockId= block.first;
                            LLVMBlock callerBlock = block.second;
                            inlineFunction(caller_blockId,callerBlock,caller, callee, *it);
                            std::cout<<"Inlining function: " << callee->GetFunctionName() << " into " << caller->GetFunctionName() << std::endl;
                            changed = true;
                            break;
                        }
                    }
                    ++it;
                }
                if (changed) break;
            }
            if (changed) break;
        }
    } while (changed);
}

/*
（1）在function_block_map中删除被inline的函数记录 【检查是否还有其它map】
（2）有必要统一维护、更新 def_use
*/