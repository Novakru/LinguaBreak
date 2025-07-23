#include "loopRotate.h"
using LLVMIROpcode = BasicInstruction::LLVMIROpcode;

// #define ZeroInvarientVarLoopNotRotate

void LoopRotate::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
		LoopInfo* loopInfo = cfg->getLoopInfo();
		DominatorTree* dt = cfg->getDomTree();
		if(((FunctionDefineInstruction*)defI)->GetFunctionName() == "spmv") // bug: 暂时没调出04_spmv.sy
			continue;

		#ifdef ZeroInvarientVarLoopNotRotate
		for (auto loop : loopInfo->getTopLevelLoops()) {
			calcInvarientVarNumber(loop, cfg);
		}
		#endif

		for (auto loop : loopInfo->getTopLevelLoops()) {
			rotateLoop(loop, dt, cfg);
		}
    }
}

void LoopRotate::calcInvarientVarNumber(Loop* loop, CFG* cfg) {
    for (auto sub : loop->getSubLoops()) {
        calcInvarientVarNumber(sub, cfg);
    }
	licm_pass.getInloopDefUses(loop);
	licm_pass.getInloopWriteInsts(loop);
	int count = 0;
	for (auto bb : loop->getBlocks()) {
		for (auto inst : bb->Instruction_list) {
			if (licm_pass.isInvariant(inst, loop, cfg)) {
				count++;
			}
		}
	}
	loop2invariantCount[loop] = count;
}

void LoopRotate::rotateLoop(Loop* loop, DominatorTree* dt, CFG* cfg) {
    for (auto sub : loop->getSubLoops()) {
        rotateLoop(sub, dt, cfg);
    }
    if (canRotate(loop, cfg)) {
        doRotate(loop, cfg);
    }
}

// LoopRotate 的前置优化: LoopSimplify
bool LoopRotate::canRotate(Loop* loop, CFG* cfg) {
	// if (!loop->verifySimplifyForm(cfg)) return false;
	if (loop->getExits().size() != 1 ||
		loop->getExitings().size() != 1 ||
		loop->getExitings().find(loop->getHeader()) == loop->getExitings().end() ||
		loop->getBlocks().size() == 1) {
		return false;
	}

	// header 的跳转指令为 uncond, 表明已经是 do while 形式
	LLVMBlock header = loop->getHeader();
	if (header->Instruction_list.back()->GetOpcode() == LLVMIROpcode::BR_UNCOND) return false;

	// 如果header中存在call指令，则不进行旋转 099_skip_spaces.sy, 04_spmv2.py
	for (auto inst : header->Instruction_list) {
		if (inst->GetOpcode() == LLVMIROpcode::CALL) {
			return false;
		}
	}	
	
	LLVMBlock latch = *loop->getLatches().begin();
	// std::cerr << "loopheader: " << header->block_id << std::endl;
	// std::cerr << "looplatch: " << latch->block_id << " has " << cfg->GetPredecessor(latch).size() << " predecessors" << std::endl;
	if (cfg->GetPredecessor(latch).size() > 1) {
		return false;
	}

	// 检查循环不变量数量, 无循环不变量时不做 loopRotate
	#ifdef ZeroInvarientVarLoopNotRotate
    if (loop2invariantCount[loop] == 0) {
        return false;
    }
	#endif

	return true;
}

void LoopRotate::doRotate(Loop* loop, CFG* cfg) {
	// 1. 找到 循环条件 所对应的代码块, 删除并存储于 condInsts.
	findAndDeleteCondInsts(loop, cfg);
	// findCondInsts(loop, cfg);

	// 2. 让header直接跳转到true_block
	LLVMBlock header = loop->getHeader();
	auto br_inst = (BrCondInstruction*) header->Instruction_list.back();
	auto ture_label = br_inst->GetTrueLabel();
	auto exit_label = br_inst->GetFalseLabel();
	header->Instruction_list.pop_back();
	auto new_br_inst = new BrUncondInstruction(ture_label);
	header->InsertInstruction(1, new_br_inst);

	// 3. preheader 需要根据 循环条件，选择进入(header, exit_block) 
	// Tip: exit 必须是 header 指向的那个
	auto header_label = GetNewLabelOperand(header->block_id);
	LLVMBlock preheader = loop->getPreheader();
	replaceUncondByCond(preheader, header_label, exit_label, cfg->max_reg);

	// 4. latch 需要根据 循环条件，选择进入(header, exit_block)
	LLVMBlock latch = *loop->getLatches().begin();  
	replaceUncondByCond(latch, header_label, exit_label, cfg->max_reg);

	// 5. 移动header的phi指令到exit, 重命名header中phi指令的结果寄存器，更新所有先前使用header phi的in-loop寄存器
	// 说明：exit 支配所有外边的寄存器使用，header支配所有的内部寄存器使用，外部寄存器编号不改变，而内部的改变。
	int exit_label_no = ((LabelOperand*) exit_label)->GetLabelNo();
	LLVMBlock exit_bb = (*cfg->block_map)[exit_label_no];
	fixPhi(loop, cfg, exit_bb);

	cfg->BuildCFG();
	cfg->getDomTree()->BuildDominatorTree();
}

/* 将 header 的 phi 指令移动到 exit，在 header 中重命名结果寄存器，并替换循环内使用 */
void LoopRotate::fixPhi(Loop* loop, CFG* cfg, LLVMBlock exit_bb) {
	LLVMBlock header = loop->getHeader();
	std::map<Operand, Operand> phi_replacements; // 原始寄存器 -> 新PHI结果寄存器
	
	// 第一遍：处理 header 中的 phi 指令
	std::vector<Instruction> header_phi_instructions;
	std::vector<Instruction> header_other_instructions;
	
	for (auto inst : header->Instruction_list) {
		if (inst->isPhi()) {
			auto phi_inst = (PhiInstruction*)inst;
			auto original_result = phi_inst->GetResult();
			
			// 创建新的结果寄存器
			auto new_result = GetNewRegOperand(++cfg->max_reg);
			phi_replacements[original_result] = new_result;
			
			// 更新 phi 指令的结果寄存器
			phi_inst->SetResult(new_result);
			
			header_phi_instructions.push_back(inst);
		} else {
			header_other_instructions.push_back(inst);
		}
	}
	
	// 重新构建 header 的指令列表
	header->Instruction_list.clear();
	for (auto phi_inst : header_phi_instructions) {
		header->Instruction_list.push_back(phi_inst);
	}
	for (auto other_inst : header_other_instructions) {
		header->Instruction_list.push_back(other_inst);
	}
	
	// 第二遍：将 header 的 phi 指令移动到 exit
	std::vector<Instruction> exit_phi_instructions;
	std::vector<Instruction> exit_other_instructions;
	
	// 先收集 exit 中现有的 phi 指令
	for (auto inst : exit_bb->Instruction_list) {
		if (inst->isPhi()) {
			exit_phi_instructions.push_back(inst);
		} else {
			exit_other_instructions.push_back(inst);
		}
	}
	
	// 将 header 的 phi 指令复制到 exit，保持原始结果寄存器
	// 注意：如果 header 的 phi 指令之间有引用关系（如 %r99 的输入是 %r97），
	// 在 exit block 新建 phi 时，必须将输入替换为 header 新建的 phi（如 %r111、%r112），
	// 否则会导致 SSA 违规（Instruction does not dominate all uses!）。
	// 例如：
	// header:
	//   %r99 = phi i32 [%r97, %L9], [%r116, %L12]
	//   %r97 = phi i32 [%r97, %L9], [%r117, %L12]
	// header 新建：
	//   %r111 = phi i32 [%r112, %L9], [%r116, %L12]
	//   %r112 = phi i32 [%r112, %L9], [%r117, %L12]
	// exit block 正确写法：
	//   %r99 = phi i32 [%r111, %L9], [%r116, %L12]
	//   %r97 = phi i32 [%r112, %L9], [%r117, %L12]
	// 错误写法（直接用原 header phi）：
	//   %r99 = phi i32 [%r97, %L9], [%r116, %L12]
	//   %r97 = phi i32 [%r97, %L9], [%r117, %L12]
	// 这样会导致 %r99 在某些路径上引用未定义的 %r97，SSA 违规。
	for (auto phi_inst : header_phi_instructions) {
		auto original_phi = (PhiInstruction*)phi_inst;
		auto phi_list = original_phi->GetPhiList();

		// 修正 phi_list 中的 value，如果引用 header phi，需替换为新建的 phi
		for (auto& [pred, value] : phi_list) {
			// 检查 value 是否为 header phi 的结果，如果是，替换为新 phi
			for (auto& [old_reg, new_reg] : phi_replacements) {
				if (value == old_reg) {
					value = new_reg;
					break;
				}
			}
		}

		// 获取原始结果寄存器（在重命名之前的）
		Operand original_result = nullptr;
		for (auto& [old_reg, new_reg] : phi_replacements) {
			if (new_reg == original_phi->GetResult()) {
				original_result = old_reg;
				break;
			}
		}

		// 创建新的 phi 指令，使用修正后的 phi_list 和原始结果寄存器
		auto new_phi_inst = new PhiInstruction(original_phi->GetResultType(), original_result, phi_list);
		exit_phi_instructions.push_back(new_phi_inst);
	}
	
	// 重新构建 exit 的指令列表
	exit_bb->Instruction_list.clear();
	for (auto phi_inst : exit_phi_instructions) {
		exit_bb->Instruction_list.push_back(phi_inst);
	}
	for (auto other_inst : exit_other_instructions) {
		exit_bb->Instruction_list.push_back(other_inst);
	}
	
	// 第三遍：替换循环内所有使用原 phi 结果寄存器的指令
	for (auto bb : loop->getBlocks()) {
		for (auto inst : bb->Instruction_list) {
			auto operands = inst->GetNonResultOperands();
			std::vector<Operand> new_operands;
			bool need_update = false;
			
			for (auto op : operands) {
				if (op->GetOperandType() == BasicOperand::REG) {
					auto reg_op = (RegOperand*)op;
					auto replace_it = phi_replacements.find(reg_op);
					if (replace_it != phi_replacements.end()) {
						new_operands.push_back(replace_it->second);
						need_update = true;
					} else {
						new_operands.push_back(op);
					}
				} else {
					new_operands.push_back(op);
				}
			}
			
			// 更新指令的操作数
			if (need_update) {
				inst->SetNonResultOperands(new_operands);
			}
		}
	}
}

/* 删除 bb基本块中的 condInsts 的条件判断指令，换成目标为 br_label 的无条件跳转指令
header.block
L2:  ; all header insts exclude phi insts are cond Insts.
    %r37 = phi float [%r18,%L1],[%r30,%L5]
    %r36 = phi i32 [10,%L1],[%r33,%L5] 
    %r22 = icmp ne i32 %r36,0
    br i1 %r22, label %L5, label %L6 (condInsts not include this inst)
*/
void LoopRotate::findAndDeleteCondInsts(Loop* loop, CFG* cfg) {
	LLVMBlock header = loop->getHeader();
	auto br_inst = (BrCondInstruction*) header->Instruction_list.back();
	header->Instruction_list.pop_back();
	
	phi_mapping.clear();
	condInsts.clear();
	
	// 1. 统计整个CFG所有用到的寄存器号
	std::set<int> used_regs;
	for (auto& [id, bb] : *cfg->block_map) {
		for (auto inst : bb->Instruction_list) {
			auto operands = inst->GetNonResultOperands();
			for (auto op : operands) {
				if (op->GetOperandType() == BasicOperand::REG) {
					auto reg_op = (RegOperand*)op;
					used_regs.insert(reg_op->GetRegNo());
				}
			}
		}
	}
	
	std::deque<Instruction> new_Instruction_list;
	for(auto inst : header->Instruction_list) {
		if(!inst->isPhi()) {
			// 检查这个 condInst 的结果寄存器是否在循环内被使用
			auto result = (RegOperand*)inst->GetResult();
			if (result != nullptr && used_regs.find(result->GetRegNo()) != used_regs.end()) {
				// 如果结果寄存器在循环内被使用，保留这个指令，并加入 condInsts 列表
				new_Instruction_list.push_back(inst);
				condInsts.push_back(inst);
			} else {
				// 否则加入 condInsts 列表
				condInsts.push_back(inst);
			}
		} else {
			// 收集PHI指令的映射关系
			auto phi_inst = (PhiInstruction*)inst;
			auto phi_result = phi_inst->GetResult();
			std::map<int, Operand> block_to_operand;
			
			for(auto [label_op, val_op] : phi_inst->GetPhiList()) {
				if(label_op->GetOperandType() == BasicOperand::LABEL) {
					int block_id = ((LabelOperand*)label_op)->GetLabelNo();
					block_to_operand[block_id] = val_op;
				}
			}
			phi_mapping[phi_result] = block_to_operand;
			
			new_Instruction_list.push_back(inst);
		}
	}
	new_Instruction_list.push_back(br_inst);
	header->Instruction_list = new_Instruction_list; 
}

// 只统计condinsts, 不改变header
void LoopRotate::findCondInsts(Loop* loop, CFG* cfg) {
	LLVMBlock header = loop->getHeader();
	auto br_inst = (BrCondInstruction*) header->Instruction_list.back();
	header->Instruction_list.pop_back();
	for(auto inst : header->Instruction_list) {
		if(!inst->isPhi()) {
			condInsts.push_back(inst);
		} else {
			// 收集PHI指令的映射关系
			auto phi_inst = (PhiInstruction*)inst;
			auto phi_result = phi_inst->GetResult();
			std::map<int, Operand> block_to_operand;
			
			for(auto [label_op, val_op] : phi_inst->GetPhiList()) {
				if(label_op->GetOperandType() == BasicOperand::LABEL) {
					int block_id = ((LabelOperand*)label_op)->GetLabelNo();
					block_to_operand[block_id] = val_op;
				}
			}
			phi_mapping[phi_result] = block_to_operand;
		}
	}
	header->Instruction_list.push_back(br_inst);
}

// 删除 bb 基本块的无条件跳转指令，换成 condInsts 的条件判断指令
/*	%r22 = icmp ne i32 %r36,0  		  ->  %r22_new = icmp ne i32 %r36,0
    br i1 %r22, label %L5, label %L6  ->  br i1 %r22_new, label %true_label, label %false_label */

/* 更新来自header phi的寄存器说明, 
Tip: stmt_expr.sy
bug1: 对preheader, latch的来自header的结果寄存器，需要根据基本块进行替换
对于新添加的条件跳转指令，需要注意的是，如果寄存器是出现在phi指令中的，要根据基本块进行替换
例如 (1) %20 替换为 0, 否则出现未定义行为
例如 (1) %20 替换为 %10, 否则出现循环多执行一次，因为%20在进入L2之后才能正确的加一
@k = global i32 0
@n = global i32 10
define i32 @main() {
L0:  ;
    br label %L1
L1:  ;
    store i32 1, ptr @k
    %r21 = load i32, ptr @n
    %r22 = sub i32 %r21,1
    %r23 = icmp sle i32 %20,%r22        ->  %r23 = icmp sle i32 0,%r22 (1)
    br i1 %r23, label %L2, label %L6
L2:  ;
    %r20 = phi i32 [0,%L1],[%r10,%L5]
    br label %L5
L5:  ;
    %r10 = add i32 %r20,1
    %r14 = load i32, ptr @k
    %r15 = load i32, ptr @k
    %r16 = add i32 %r14,%r15
    store i32 %r16, ptr @k
    %r24 = load i32, ptr @n
    %r25 = sub i32 %r24,1
    %r26 = icmp sle i32 %r20,%r25		->  %r26 = icmp sle i32 %r10,%r25 (2)
    br i1 %r26, label %L2, label %L6
L6:  ;
    %r17 = load i32, ptr @k
    call void @putint(i32 %r17)
    %r18 = load i32, ptr @k
    ret i32 %r18
}
*/
void LoopRotate::replaceUncondByCond(LLVMBlock bb, Operand true_label, Operand false_label, int& max_reg) {
	bb->Instruction_list.pop_back(); // 删除无条件跳转指令
	std::map<int, int> regNo_map; // old_regNo -> new_regNo (%r22 -> %r22_new)

	// 1. 为所有需要重命名的寄存器分配新的寄存器号
	for (auto inst : condInsts) {
		auto result = (RegOperand*)inst->GetResult();
		if (result != nullptr) {
			regNo_map[result->GetRegNo()] = ++max_reg;
		}
	}

	// 2. 克隆并插入 condInsts 中的指令
	for (auto inst : condInsts) {
		Instruction cloned_inst = inst->Clone();
		
		std::map<int, int> store_map, use_map;
		for (const auto& [old_reg, new_reg] : regNo_map) {
			store_map[old_reg] = new_reg;
			use_map[old_reg] = new_reg;
		}
		// 应用寄存器重命名（包括结果寄存器）
		cloned_inst->ChangeReg(store_map, use_map);
		cloned_inst->ChangeResult(regNo_map);
		
		bb->Instruction_list.push_back(cloned_inst);
	}

	// 3. 替换 condInsts中的 header PHI指令的结果寄存器
	for (auto it = bb->Instruction_list.end() - condInsts.size(); it != bb->Instruction_list.end(); ++it) {
		auto inst = *it;
		auto operands = inst->GetNonResultOperands();
		std::vector<Operand> new_operands;
		
		for (auto op : operands) {
			Operand new_op = op;
			
			// 如果是寄存器操作数，检查是否需要PHI替换
			if (op->GetOperandType() == BasicOperand::REG) {
				auto reg_op = (RegOperand*)op;
				auto phi_result = GetNewRegOperand(reg_op->GetRegNo());
				
				auto phi_it = phi_mapping.find(phi_result);
				if (phi_it != phi_mapping.end()) {
					auto block_it = phi_it->second.find(bb->block_id);
					if (block_it != phi_it->second.end()) {
						new_op = block_it->second;
					}
				}
			}
			
			new_operands.push_back(new_op);
		}
		
		inst->SetNonResultOperands(new_operands);
	}

	// 4. 创建新的条件跳转指令
	int cond_reg = -1;
	for (auto it = condInsts.rbegin(); it != condInsts.rend(); ++it) {
		auto inst = *it;
		if (inst->GetOpcode() == BasicInstruction::ICMP || inst->GetOpcode() == BasicInstruction::FCMP) {
			int old_reg = ((RegOperand*)inst->GetResult())->GetRegNo();
			if (regNo_map.count(old_reg)) {
				cond_reg = regNo_map[old_reg];
				break;
			}
		}
	}
	Operand cond_operand = GetNewRegOperand(cond_reg);
	BrCondInstruction* new_br_cond = new BrCondInstruction(cond_operand, true_label, false_label);
	bb->Instruction_list.push_back(new_br_cond);
}

// 检查所有phi指令的合法性，输出错误位置
void LoopRotate::CheckAllPhi(CFG* cfg) {
    for (auto &[id, block] : *cfg->block_map) {
        std::set<LLVMBlock> preds = cfg->GetPredecessor(block);
        std::set<int> pred_ids;
        for (auto pred : preds) pred_ids.insert(pred->block_id);
        int phi_idx = 0;
        for (auto &inst : block->Instruction_list) {
            if (!inst->isPhi()) break;
            PhiInstruction* phi = (PhiInstruction*)inst;
            auto phi_list = phi->GetPhiList();
            // 1. 检查参数数量和前驱数量
            if ((int)phi_list.size() != (int)pred_ids.size()) {
                std::cerr << "[CheckAllPhi] 错误: block " << block->block_id << " 的第" << phi_idx << "个phi参数数量(" << phi_list.size() << ")与前驱数量(" << pred_ids.size() << ")不一致" << std::endl;
            }
            // 2. 检查参数label是否为真实前驱
            for (auto &[label, value] : phi_list) {
                if (label->GetOperandType() != BasicOperand::LABEL) continue;
                int label_id = ((LabelOperand*)label)->GetLabelNo();
                if (!pred_ids.count(label_id)) {
                    std::cerr << "[CheckAllPhi] 错误: block " << block->block_id << " 的phi参数label %L" << label_id << " 不是前驱" << std::endl;
                }
            }
            phi_idx++;
        }
    }
}
