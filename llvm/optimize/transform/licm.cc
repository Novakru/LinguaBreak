#include "licm.h"

void LoopInvariantCodeMotionPass::Execute() {
    for(auto [defI, cfg] : llvmIR->llvm_cfg){
        LoopInfo *loopInfo = cfg->getLoopInfo();
        DominatorTree *dt = cfg->getDomTree();
        for (auto loop : loopInfo->getTopLevelLoops()) {
            RunLICMOnLoop(loop, dt);
        }
    }
}

void LoopInvariantCodeMotionPass::getInloopDefUses(Loop* loop) {
    inloop_defs.clear();
    inloop_uses.clear(); 

    for (auto loop_bb : loop->getBlocks()) {
        for (auto inst : loop_bb->Instruction_list) {
            // 1. 记录def（指令定义的变量）
            auto resOp = inst->GetResult();
            if (resOp != nullptr) {
                inloop_defs[resOp] = inst;
            }

            // 2. 记录use（指令使用的操作数）
            for (auto op : inst->GetNonResultOperands()) {
                if (op->GetOperandType() == BasicOperand::REG) {
                    inloop_uses[op].push_back(inst);
                }
            }
        }
    }
}

void LoopInvariantCodeMotionPass::eraseBlockOnLoop(Loop* loop) {
	for(auto loop_bb : loop->getBlocks()) {
		auto tmp_Instruction_list = loop_bb->Instruction_list;
        loop_bb->Instruction_list.clear();
        for (auto I : tmp_Instruction_list) {
            if (eraseInsts.find(I) == eraseInsts.end()) 
				loop_bb->InsertInstruction(1, I);    
        }
	}

	eraseInsts.clear();
}

void LoopInvariantCodeMotionPass::RunLICMOnLoop(Loop* loop, DominatorTree* dt) {
    for (auto sub : loop->getSubLoops()) {
        RunLICMOnLoop(sub, dt);
    }
	
	getInloopDefUses(loop);

    LLVMBlock preheader = loop->getPreheader();
    if (preheader == nullptr) { return; }

    // 1. 收集循环不变量候选
    std::vector<Instruction> candidates;
    for (auto bb : loop->getBlocks()) {
        bool dflag = dominatesAllExits(bb, loop, dt);
        for (auto inst : bb->Instruction_list) {
            if (isInvariant(inst, loop) && dflag) {
                candidates.push_back(inst);
            }
        }
    }
	
	#ifdef debug
		std::cerr << "candidate inst list: " << std::endl;
		for(auto inst : candidates) {
			inst->PrintIR(std::cerr);
		}
		
		// std::vector<Instruction> isInvariant_insts, dominatesAllExits_insts;
		// for (auto bb : loop->getBlocks()) {
		// 	bool dflag = dominatesAllExits(bb, loop, dt);
		// 	for (auto inst : bb->Instruction_list) {
		// 		if (isInvariant(inst, loop)) { isInvariant_insts.push_back(inst); } 
		// 		if (dflag) { dominatesAllExits_insts.push_back(inst); } 
		// 	}
		// }
		// std::cerr << "isInvariant inst list: " << std::endl;
		// for(auto inst : isInvariant_insts) {
		// 	inst->PrintIR(std::cerr);
		// }
		// std::cerr << "dominatesAllExits inst list: " << std::endl;
		// for(auto inst : dominatesAllExits_insts) {
		// 	inst->PrintIR(std::cerr);
		// }
	#endif

    std::stack<Instruction, std::vector<Instruction>> work(candidates);
	auto br_inst = preheader->Instruction_list.back();
	preheader->Instruction_list.pop_back();

    while (!work.empty()) {
        Instruction inst = work.top();
        work.pop();

        if (eraseInsts.count(inst)) continue;
        if (!isInvariant(inst, loop)) continue; 
		if (!dominatesAllExits(dt->getBlockByID(inst->GetBlockID()), loop, dt)) continue;

        // 处理内存操作
        // if (inst->mayReadFromMemory() || inst->mayWriteToMemory()) {
        //     if (!canHoistWithAlias(inst, loop)) {
        //         continue;
        //     }
        // }

        // 实际移动指令到 preheader, 提前把 br 指令弹出
		// std::cerr << "insert to prheader[" << preheader->block_id << "]: ";
		// inst->PrintIR(std::cerr);
        preheader->InsertInstruction(1, inst);
        eraseInsts.insert(inst);  // same to insts that moved.

        // 可能有新的候选可以处理，重新加入工作列表
        for (auto userInst : inloop_uses[inst->GetResult()]) {
			work.push(userInst);
        }
    }

	preheader->Instruction_list.push_back(br_inst);
	eraseBlockOnLoop(loop);
}

bool LoopInvariantCodeMotionPass::isInvariant(Instruction inst, Loop* loop) {
    // 1. 忽略非计算指令
    switch(inst->GetOpcode()) {
        case BasicInstruction::LLVMIROpcode::BR_COND:
        case BasicInstruction::LLVMIROpcode::BR_UNCOND:
        case BasicInstruction::LLVMIROpcode::RET:
        case BasicInstruction::LLVMIROpcode::PHI:
            return false;
		// 这些指令暂时忽略，待到 AliasAnalysis 支持后加入分析
		case BasicInstruction::LLVMIROpcode::LOAD:
		case BasicInstruction::LLVMIROpcode::CALL:
		case BasicInstruction::LLVMIROpcode::STORE:
			return false;
        default:
            break;
    }

    // 2. 检查操作数是否都来自循环外或循环不变量
    for (auto op : inst->GetNonResultOperands()) {
		if (op->GetOperandType() != BasicOperand::REG) { continue; }
        if (inloop_defs.count(op) && !eraseInsts.count(inloop_defs[op])) { return false; }
    }

    // 3. 内存操作需要额外检查
    // if (inst->mayReadFromMemory()) {
    //     // 必须是只读且访问地址是循环不变量
    //     if (!isSafeToSpeculativelyExecute(inst)) {
    //         return false;
    //     }
    // }

    return true;
}

bool LoopInvariantCodeMotionPass::dominatesAllExits(LLVMBlock bb, Loop* loop, DominatorTree* dt) {
    for (auto exit_bb : loop->getExits()) {
        if (!dt->dominates(bb, exit_bb)) {
            return false;
        }
    }
    return true;
}

bool LoopInvariantCodeMotionPass::canHoistWithAlias(Instruction inst, Loop* loop) {
    // 简化版实现 - 实际应该使用别名分析
    // 只允许加载(load)操作，且地址是循环不变量
    // if (LoadInst* load = dyn_cast<LoadInst>(inst)) {
    //     Value* addr = load->getPointerOperand();
    //     if (Instruction addrInst = dyn_cast<Instruction>(addr)) {
	// 		return false;
    //     }
    //     return true;
    // }
    return false;
}
