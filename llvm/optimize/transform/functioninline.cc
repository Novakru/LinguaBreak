#include "functioninline.h"
#include <functional>

const std::unordered_set<std::string> FunctionInlinePass::lib_function_names = {
    "getint", "getch", "getfloat", "getarray", "getfarray",
    "putint", "putch", "putfloat", "putarray", "putfarray",
    "_sysy_starttime", "_sysy_stoptime",
    "llvm.memset.p0.i32", "llvm.umax.i32", "llvm.umin.i32",
    "llvm.smax.i32", "llvm.smin.i32"
};


/*
For : 记录调用图callGraph和函数大小funcSize
    - callGraph构建
    - calleeReturn: block_id --> RetInstruction
    - funcSize
*/
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
                    auto id_ret = std::make_pair(block.first, retInst);
                    calleeReturn[caller] = id_ret;
                }else if(inst->GetOpcode() == BasicInstruction::LLVMIROpcode::PHI){
                    PhiInstruction *phi = (PhiInstruction*)inst;
                    auto phi_list = phi->GetPhiList();
                    for(auto &[label,reg]:phi_list){
                        int label_no=((LabelOperand*)label)->GetLabelNo();
                        phiGraph[caller][label_no].insert(phi);
                    }
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
    if(caller->GetFunctionName()==callee->GetFunctionName()){
        return false;
    }
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

/*
若已存在映射，取
否则，新建
*/
int FunctionInlinePass::renameRegister(FuncDefInstruction caller,int oldReg, std::unordered_map<int, int>& regMapping) {
    if (regMapping.count(oldReg)) {
        return regMapping[oldReg];
    }
    llvmIR->function_max_reg[caller]+=1;//必须超过调用者的regno范围
    int newReg = llvmIR->function_max_reg[caller];
    regMapping[oldReg] = newReg;
    return newReg;
}

int FunctionInlinePass::renameLabel(FuncDefInstruction caller,int oldLabel, std::unordered_map<int, int>& labelMapping) {
    if (labelMapping.count(oldLabel)) {
        return labelMapping[oldLabel];
    }
    llvmIR->function_max_label[caller]+=1;
    int newLabel = llvmIR->function_max_label[caller];//必须超过调用者的label范围
    labelMapping[oldLabel] = newLabel;
    return newLabel;
}

LLVMBlock FunctionInlinePass::copyBasicBlock(FuncDefInstruction caller,LLVMBlock origBlock, std::unordered_map<int, int>& regMapping, std::unordered_map<int, int>& labelMapping) {
    int newLabel = renameLabel(caller,origBlock->block_id, labelMapping);
    LLVMBlock newBlock = new BasicBlock(newLabel);
    
    // 复制指令并重命名寄存器和标签
    for (auto &inst : origBlock->Instruction_list) {
        //std::cout<<"[in copy] copy inst of  type: "<<inst->GetOpcode()<<std::endl;
        Instruction newInst = inst->InstructionClone();
        //std::cout<<"          copy succeed!"<<std::endl;
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
                Operand newOp = GetNewLabelOperand(renameLabel(caller,oldLabel, labelMapping));
                nonResultOps.emplace_back(newOp);
            }
        }
        newInst->SetNonResultOperands(nonResultOps);
        newBlock->Instruction_list.push_back(newInst);
    }
    return newBlock;
}


void FunctionInlinePass::inlineFunction(int callerBlockId, LLVMBlock callerBlock,FuncDefInstruction caller, FuncDefInstruction callee, CallInstruction* callInst) {
    std::unordered_map<int, int> regMapping;
    std::unordered_map<int, int> labelMapping;
    
    // 【2】校正首尾对应关系
    // 【2.1】将被调函数形参的regno替换为调用者实参中的regno
    std::deque<Instruction> fparams_insts;//若call时用imm传实参，则通过add指令先放到寄存器里
    auto args = callInst->GetNonResultOperands();
    for (int i=0;i< args.size(); i++) {
        if (args[i]->GetOperandType() == BasicOperand::REG) {
            int paramReg = ((RegOperand*)args[i])->GetRegNo();
            regMapping[i] = paramReg;
        }else{
            Instruction add_inst;
            llvmIR->function_max_reg[caller]+=1;//必须超过调用者的regno范围
            int result_regno=llvmIR->function_max_reg[caller];
            RegOperand* paramReg= GetNewRegOperand(result_regno);
            if(args[i]->GetOperandType() == BasicOperand::IMMI32){
                add_inst=new ArithmeticInstruction(BasicInstruction::LLVMIROpcode::ADD,BasicInstruction::LLVMType::I32,
                         args[i],new ImmI32Operand(0),paramReg);
            }else if(args[i]->GetOperandType() == BasicOperand::IMMF32){
                add_inst=new ArithmeticInstruction(BasicInstruction::LLVMIROpcode::FADD,BasicInstruction::LLVMType::FLOAT32,
                         args[i],new ImmF32Operand(0),paramReg);
            }
            fparams_insts.push_back(add_inst);
            regMapping[i] = result_regno;
        }
    }

    // 【1】复制被调用函数的所有基本块
    for (auto &block : llvmIR->function_block_map[callee]){
        LLVMBlock newblock =copyBasicBlock(caller,block.second, regMapping, labelMapping);
        llvmIR->function_block_map[caller][newblock->block_id]=newblock;
    }
    
    // 【2】校正首尾对应关系
    // 【2.2】修改调用指令所在基本块的跳转关系  caller_block ---> Inline_Function_EntryBlock
    auto temp_list = callerBlock->Instruction_list;
    callerBlock->Instruction_list.clear();
    std::deque<Instruction> caller_back_list;//callInstruction所在block的后半段
    for(int i=0;i< temp_list.size(); i++) {
        Instruction inst = temp_list[i];
        if (inst == callInst) {
            // (1)保存结果寄存器，删除调用指令
            RegOperand * resultReg ;
            if(callInst->GetResult() && callInst->GetResult()->GetOperandType() == BasicOperand::REG) {
                resultReg = (RegOperand*)callInst->GetResult();
            } else {
                resultReg = nullptr;
            }
            // (2)添加将call中实参转换为reg类型的指令
            for(auto &add_inst:fparams_insts){
                callerBlock->Instruction_list.push_back(add_inst);
            }fparams_insts.clear();

            // (3)添加跳转到内联函数入口块的指令
            BrUncondInstruction* brInst = new BrUncondInstruction(GetNewLabelOperand(labelMapping[0]));// 0是入口块
            callerBlock->Instruction_list.push_back(brInst);
            // (4)保存后续指令，结束此块
            for(int j = i + 1; j < temp_list.size(); j++) {
                caller_back_list.push_back(temp_list[j]);
            }
            break;
        }else{
            callerBlock->Instruction_list.push_back(inst);
        }
    }


    // 【2.3】处理内联函数的返回值，合并尾块  Inline_Function_ExitBlock ---> caller_block
    auto [return_blockid,ret] = calleeReturn[caller];
    LLVMBlock retBlock = llvmIR->function_block_map[caller][return_blockid];
    auto tailInst_list=retBlock->Instruction_list;//assert:最后一条指令必为ret
    tailInst_list.pop_back();
    if(callInst->GetRetType()!=BasicInstruction::LLVMType::VOID){
        Operand RetValue = ret->GetRetVal();
        Operand CallResult = callInst->GetResult();
        assert(CallResult!=nullptr);
        assert(CallResult->GetOperandType()==BasicOperand::REG);
        int Callres_regno=((RegOperand*)CallResult)->GetRegNo();
        Instruction newAddInstr;
        if(ret->GetType()==BasicInstruction::LLVMType::I32){
            newAddInstr=new ArithmeticInstruction(BasicInstruction::LLVMIROpcode::ADD,
                        BasicInstruction::LLVMType::I32,RetValue,new ImmI32Operand(0),CallResult);
        }else if(ret->GetType()==BasicInstruction::LLVMType::FLOAT32){
            newAddInstr=new ArithmeticInstruction(BasicInstruction::LLVMIROpcode::FADD,
                        BasicInstruction::LLVMType::FLOAT32,RetValue,new ImmF32Operand(0),CallResult);
        }
        tailInst_list.push_back(newAddInstr);
    }
    for(auto &inst:caller_back_list){
        tailInst_list.push_back(inst);
    }

    // 【2.4】处理后续phi指令的 源block_id
    int old_phi_ori=callerBlock->block_id;
    int new_phi_ori=return_blockid;
    for(auto &[id,phis]:phiGraph[caller]){
        if(id==old_phi_ori){
            for(auto &phi:phis){
                //将source_blockid 改为新的
                auto old_phi_list=phi->GetPhiList();
                std::vector<std::pair<Operand, Operand>> new_phi_list;
                new_phi_list.reserve(old_phi_list.size());
                for(auto &phi_pair:phi->GetPhiList()){
                    if(((LabelOperand*)phi_pair.first)->GetLabelNo()==old_phi_ori){
                        new_phi_list.emplace_back(std::make_pair(GetNewLabelOperand(new_phi_ori),phi_pair.second));
                    }else{
                        new_phi_list.emplace_back(phi_pair);
                    }
                }
                phi->SetPhiList(new_phi_list);
            }
        }
    }
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
                        if(lib_function_names.count(callInst->GetFunctionName())){//非自定义函数不Inline
                            ++it;   continue;
                        }
                        FuncDefInstruction callee = llvmIR->FunctionNameTable[callInst->GetFunctionName()];//被调用者
                        if (shouldInline(caller, callee)) {
                            //std::cout<<"Inlining function: " << callee->GetFunctionName() << " into " << caller->GetFunctionName() << std::endl;
                            int caller_blockId= block.first;
                            LLVMBlock callerBlock = block.second;
                            inlineFunction(caller_blockId,callerBlock,caller, callee, callInst);

                            changed = true;
                            inlined_function_names.insert(callee);
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

    //清理llvmIR->function_block_map中被内联的函数
    auto it = llvmIR->function_block_map.begin();
    while (it != llvmIR->function_block_map.end()) {
        if (inlined_function_names.count(it->first)) {
            //std::cout<<"delete function: "<<it->first->GetFunctionName()<<std::endl;
            auto this_def=it->first;
            it = llvmIR->function_block_map.erase(it);
            llvmIR->llvm_cfg.erase(this_def);
            llvmIR->FunctionNameTable.erase(this_def->GetFunctionName());
            llvmIR->function_max_label.erase(this_def);
            llvmIR->function_max_reg.erase(this_def);
            delete this_def;
        } else {
            //std::cout<<"donot delete this function!"<<std::endl;
            ++it;
        }
    }
    
}

/*
（1）在function_block_map中删除被inline的函数记录 【检查是否还有其它map】
（2）有必要统一维护、更新 def_use
*/