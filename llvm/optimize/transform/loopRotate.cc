#include "loopRotate.h"
using LLVMIROpcode = BasicInstruction::LLVMIROpcode;

void LoopRotate::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
		LoopInfo* loopInfo = cfg->getLoopInfo();
		DominatorTree* dt = cfg->getDomTree();
		for (auto loop : loopInfo->getTopLevelLoops()) {
			rotateLoop(loop, dt, cfg);
		}
    }
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
	if (!loop->verifySimplifyForm(cfg)) return false;

	// header 的跳转指令为 uncond, 表明已经是 do while 形式
	LLVMBlock header = loop->getHeader();
	if (header->Instruction_list.back()->GetOpcode() == LLVMIROpcode::BR_UNCOND) return false;
	
	LLVMBlock latch = *loop->getLatches().begin();
	std::cerr << "loopheader: " << header->block_id << std::endl;
	std::cerr << "looplatch: " << latch->block_id << " has " << cfg->GetPredecessor(latch).size() << " predecessors" << std::endl;
	if (cfg->GetPredecessor(latch).size() > 1) {
		return false;
	}

	return true;
}

void LoopRotate::doRotate(Loop* loop, CFG* cfg) {
	// 1. 找到 循环条件 所对应的代码块, 删除并存储于 condInsts.
	findAndDeleteCondInsts(loop, cfg);

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

	// 5. 对exit的来自header的结果寄存器，需要新建phi指令
	addPhi2Exit(loop, cfg);

	cfg->BuildCFG();
	cfg->getDomTree()->BuildDominatorTree();
}

void LoopRotate::addPhi2Exit(Loop* loop, CFG* cfg) {
	LLVMBlock header = loop->getHeader();
	LLVMBlock preheader = loop->getPreheader();
	
	// 遍历所有exit基本块
	for (auto exit_bb : loop->getExits()) {
		std::vector<Instruction> phi_instructions;
		std::vector<Instruction> other_instructions;
		std::map<Operand, Operand> phi_replacements; // 原始寄存器 -> 新PHI结果寄存器
		
		// 第一遍：收集所有需要PHI指令的寄存器
		for (auto inst : exit_bb->Instruction_list) {
			auto operands = inst->GetNonResultOperands();
			
			for (auto op : operands) {
				if (op->GetOperandType() == BasicOperand::REG) {
					auto reg_op = (RegOperand*)op;
					
					// 检查这个寄存器是否是来自header的结果寄存器（在phi_mapping中）
					auto phi_it = phi_mapping.find(reg_op);
					if (phi_it != phi_mapping.end() && phi_replacements.find(reg_op) == phi_replacements.end()) {
						// 从header中找到原始的PHI指令
						std::vector<std::pair<Operand, Operand>> phi_list;
						for (auto header_inst : header->Instruction_list) {
							if (header_inst->isPhi()) {
								auto phi_inst = (PhiInstruction*)header_inst;
								if (phi_inst->GetResult() == reg_op) {
									phi_list = phi_inst->GetPhiList();
									break;
								}
							}
						}
						
						// 创建新的PHI指令，直接继承原始PHI指令的phi_list
						auto phi_result = GetNewRegOperand(++cfg->max_reg);
						auto phi_inst = new PhiInstruction(BasicInstruction::I32, phi_result, phi_list);
						phi_instructions.push_back(phi_inst);
						
						// 记录替换关系
						phi_replacements[reg_op] = phi_result;
					}
				}
			}
		}
		
		// 第二遍：更新所有指令的操作数
		for (auto inst : exit_bb->Instruction_list) {
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
			
			other_instructions.push_back(inst);
		}
		
		// 重新构建exit基本块的指令列表：PHI指令在前，其他指令在后
		exit_bb->Instruction_list.clear();
		for (auto phi_inst : phi_instructions) {
			exit_bb->Instruction_list.push_back(phi_inst);
		}
		for (auto other_inst : other_instructions) {
			exit_bb->Instruction_list.push_back(other_inst);
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
	
	std::deque<Instruction> new_Instruction_list;
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
			
			new_Instruction_list.push_back(inst);
		}
	}
	new_Instruction_list.push_back(br_inst);
	header->Instruction_list = new_Instruction_list; 
}

// 删除 bb 基本块的无条件跳转指令，换成 condInsts 的条件判断指令
/*	%r22 = icmp ne i32 %r36,0  		  ->  %r22_new = icmp ne i32 %r36,0
    br i1 %r22, label %L5, label %L6  ->  br i1 %r22_new, label %true_label, label %false_label */
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

/* Tip: stmt_expr.sy
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

Tip: 1068_accumulate.sy
bug2: 对exit的来自header的结果寄存器，需要新建phi指令
define i32 @main(){
L0:  ;
    br label %L7
L2:  ;
    %r21 = phi i32 [%r16,%L5],[%r22,%L7]
    %r20 = phi i32 [%r13,%L5],[%r23,%L7]
    br label %L5
L5:  ;
    %r13 = add i32 %r20,%r21
    %r16 = add i32 %r21,1
    %r25 = icmp slt i32 %r16,21
    br i1 %r25, label %L2, label %L6
L6:  ;	
	call void @putint(i32 %r20)			   ->  %r26 = phi i32 [%r13,%L5],[%r23,%L7]
										   ->  call void @putint(i32 %r26)
    ret i32 0
L7:  ;
    %r22 = phi i32 [0,%L0]
    %r23 = phi i32 [0,%L0]
    %r24 = icmp slt i32 %r22,21
    br i1 %r24, label %L2, label %L6
}
*/
