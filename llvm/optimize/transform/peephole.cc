#include "peephole.h"
#include <functional>

void ImmResultReplace(CFG *C);

void PeepholePass::Execute() {
	DeadArgElim();
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        // add all implemented pass
		SrcEqResultInstEliminate(cfg);
		ImmResultReplace(cfg);
    }
}

void PeepholePass::SrcEqResultInstEliminateExecute(){
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        SrcEqResultInstEliminate(cfg);
    }
}

void PeepholePass::ImmResultReplaceExecute(){
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        ImmResultReplace(cfg);
    }
}

void ImmResultReplace(CFG *C){
    std::set<Instruction> EraseSet;
    std::map<Operand, Operand> replacemap;
    bool changed = true;
    while(changed){
        changed = false;
        EraseSet.clear();
        replacemap.clear();
        for(auto [id, bb] : *C->block_map){
            for(auto I : bb->Instruction_list){
                auto opcode = I->GetOpcode();
                if(opcode == BasicInstruction::ADD 
                        || opcode == BasicInstruction::SUB 
                        || opcode == BasicInstruction::MUL 
                        || opcode == BasicInstruction::DIV 
                        || opcode == BasicInstruction::MOD 
                        || opcode == BasicInstruction::BITXOR
                        || opcode == BasicInstruction::FADD 
                        || opcode == BasicInstruction::FSUB 
                        || opcode == BasicInstruction::FMUL 
                        || opcode == BasicInstruction::FDIV){
                    auto ArithI = (ArithmeticInstruction*)I;
                    auto resultop = ArithI->GetResult();
                    auto op1 = ArithI->GetOperand1();
                    auto op2 = ArithI->GetOperand2();
                    if(op1->GetOperandType() != BasicOperand::REG && op2->GetOperandType() != BasicOperand::REG){
                        EraseSet.insert(I);
                        auto type = ArithI->GetDataType();
                        changed = 1;
                        if(type == BasicInstruction::I32){
                            auto imm1 = ((ImmI32Operand*)op1)->GetIntImmVal();
                            auto imm2 = ((ImmI32Operand*)op2)->GetIntImmVal();
                            int ans = 0;
                            switch (opcode)
                            {
                            case BasicInstruction::ADD:
                                ans = imm1 + imm2;
                                break;
                            case BasicInstruction::SUB:
                                ans = imm1 - imm2;
                                break;
                            case BasicInstruction::MUL:
                                ans = imm1 * imm2;
                                break;
                            case BasicInstruction::DIV:
                                ans = imm1 / imm2;
                                break;
                            case BasicInstruction::MOD:
                                ans = imm1 % imm2;
                                break;
                            case BasicInstruction::BITXOR:
                                ans = imm1 ^ imm2;
                                break;
                            default:
                                break;
                            }
                            replacemap[resultop] = new ImmI32Operand(ans);
                        }else {
                            auto imm1 = ((ImmF32Operand*)op1)->GetFloatVal();
                            auto imm2 = ((ImmF32Operand*)op2)->GetFloatVal();
                            float ans = 0;
                            switch (opcode)
                            {
                            case BasicInstruction::FADD:
                                ans = imm1 + imm2;
                                break;
                            case BasicInstruction::FSUB:
                                ans = imm1 - imm2;
                                break;
                            case BasicInstruction::FMUL:
                                ans = imm1 * imm2;
                                break;
                            case BasicInstruction::FDIV:
                                ans = imm1 / imm2;
                                break;
                            default:
                                break;
                            }
                            replacemap[resultop] = new ImmF32Operand(ans);
                        }
                    }
                }else if(I->GetOpcode() == BasicInstruction::ICMP){
                    auto icmpI = (IcmpInstruction*)I;
                    auto resultop = icmpI->GetResult();
                    auto op1 = icmpI->GetOp1();
                    auto op2 = icmpI->GetOp2();
                    auto cond = icmpI->GetCond();
                    if(op1->GetOperandType() != BasicOperand::REG && op2->GetOperandType() != BasicOperand::REG){
                        EraseSet.insert(I);
                        changed = 1;
                        int ans = 0;
                        auto imm1 = ((ImmI32Operand*)op1)->GetIntImmVal();
                        auto imm2 = ((ImmI32Operand*)op2)->GetIntImmVal();
                        switch (cond)
                        {
                        case BasicInstruction::eq:
                            ans = imm1 == imm2;
                            break;
                        case BasicInstruction::ne:
                            ans = imm1 != imm2;
                            break;
                        case BasicInstruction::ugt:
                            ans = imm1 > imm2;
                            break;
                        case BasicInstruction::uge:
                            ans = imm1 >= imm2;
                            break;
                        case BasicInstruction::ult:
                            ans = imm1 < imm2;
                            break;
                        case BasicInstruction::ule:
                            ans = imm1 <= imm2;
                            break;
                        case BasicInstruction::sgt:
                            ans = imm1 > imm2;
                            break;
                        case BasicInstruction::sge:
                            ans = imm1 >= imm2;
                            break;
                        case BasicInstruction::slt:
                            ans = imm1 < imm2;
                            break;
                        case BasicInstruction::sle:
                            ans = imm1 <= imm2;
                            break;
                        default:
                            break;
                        }
                        replacemap[resultop] = new ImmI32Operand(ans);
                    }
                }else if(I->GetOpcode() == BasicInstruction::FCMP){
                    auto fcmpI = (FcmpInstruction*)I;
                    auto resultop = fcmpI->GetResult();
                    auto op1 = fcmpI->GetOp1();
                    auto op2 = fcmpI->GetOp2();
                    auto cond = fcmpI->GetCond();
                    if(op1->GetOperandType() != BasicOperand::REG && op2->GetOperandType() != BasicOperand::REG){
                        EraseSet.insert(I);
                        changed = 1;
                        int ans = 0;
                        auto imm1 = ((ImmF32Operand*)op1)->GetFloatVal();
                        auto imm2 = ((ImmF32Operand*)op2)->GetFloatVal();
                        switch (cond)
                        {
                        case BasicInstruction::FALSE:
                            ans = 0;
                            break;
                        case BasicInstruction::OEQ:
                        case BasicInstruction::UEQ:
                            ans = imm1 == imm2;
                            break;
                        case BasicInstruction::OGT:
                        case BasicInstruction::UGT:
                            ans = imm1 > imm2;
                            break;
                        case BasicInstruction::OGE:
                        case BasicInstruction::UGE:
                            ans = imm1 >= imm2;
                            break;
                        case BasicInstruction::OLT:
                        case BasicInstruction::ULT:
                            ans = imm1 < imm2;
                            break;
                        case BasicInstruction::OLE:
                        case BasicInstruction::ULE:
                            ans = imm1 <= imm2;
                            break;
                        case BasicInstruction::ONE:
                        case BasicInstruction::UNE:
                            ans = imm1 != imm2;
                            break;
                        case BasicInstruction::ORD:
                            ans = 1;
                            break;
                        case BasicInstruction::UNO:
                            ans = 1;
                            break;
                        case BasicInstruction::TRUE:
                            ans = 1;
                            break;
                        default:
                            break;
                        }
                        replacemap[resultop] = new ImmI32Operand(ans);
                    }
                }
            }
        }
        for (auto [id, bb] : *C->block_map) {
            auto tmp_Instruction_list = bb->Instruction_list;
            bb->Instruction_list.clear();
            for (auto &I : tmp_Instruction_list) {
                if (EraseSet.find(I) != EraseSet.end()) {
                    // I->PrintIR(std::cerr);
                    continue;
                }
                auto opvec = I->GetNonResultOperands();
                // if(I->GetResult()!=nullptr && I->GetResult()->GetFullName() == "%r45"){
                //     I->PrintIR(std::cerr);
                // }
                for(auto &op : opvec){
                    if(op->GetOperandType() == BasicOperand::REG && replacemap.find(op) != replacemap.end()){
                        op = replacemap[op];
                    }
                }
                I->SetNonResultOperands(opvec);
                
                bb->InsertInstruction(1, I);
            }
        }
    }
}

/*eliminate the instructions like
%rx = %ry + 0(replace all the use of %rx with %ry) %ry can be i32 or float
%rx = %ry - 0(replace all the use of %rx with %ry) %ry can be i32 or float
%rx = %ry * 1(replace all the use of %rx with %ry) %ry can be i32 or float
%rx = %ry / 1(replace all the use of %rx with %ry) %ry can be i32 or float

I->ReplaceRegByMap(), I->GetNonResultOperands(), I->SetNonResultOperands() is Useful
*/
void PeepholePass::SrcEqResultInstEliminate(CFG* C) {
	std::unordered_map<int,int> regno_map; // rx_regno -> ry_regno
    //冗余指令删除与信息记录
    for(auto &[id,block]:*(C->block_map)){
        for(auto it=block->Instruction_list.begin(); it!=block->Instruction_list.end();){
            auto inst=*it;
            if(inst->GetOpcode() == BasicInstruction::ADD){
                auto arith_inst = (ArithmeticInstruction*)inst;
                auto result_op = arith_inst->GetResult();
                auto op1 = arith_inst->GetOperand1();
                auto op2 = arith_inst->GetOperand2();
                if(op1->GetOperandType() == BasicOperand::REG && op2->GetOperandType() == BasicOperand::IMMI32 && ((ImmI32Operand*)op2)->GetIntImmVal() == 0){
                    regno_map[((RegOperand*)result_op)->GetRegNo()] = ((RegOperand*)op1)->GetRegNo();
                    it = block->Instruction_list.erase(it);
                    continue;
                }else if(op1->GetOperandType() == BasicOperand::IMMI32 && ((ImmI32Operand*)op1)->GetIntImmVal() == 0 && op2->GetOperandType() == BasicOperand::REG){
                    regno_map[((RegOperand*)result_op)->GetRegNo()] = ((RegOperand*)op2)->GetRegNo();
                    it = block->Instruction_list.erase(it);
                    continue;
                }
                else{
                    ++it;
                    continue;
                }
            }else if(inst->GetOpcode() == BasicInstruction::FADD){
                auto farith_inst = (ArithmeticInstruction*)inst;
                auto result_op = farith_inst->GetResult();
                auto op1 = farith_inst->GetOperand1();
                auto op2 = farith_inst->GetOperand2();
                if(op1->GetOperandType() == BasicOperand::REG && op2->GetOperandType() == BasicOperand::IMMF32 && ((ImmF32Operand*)op2)->GetFloatVal() == 0.0f){
                    regno_map[((RegOperand*)result_op)->GetRegNo()] = ((RegOperand*)op1)->GetRegNo();
                    it = block->Instruction_list.erase(it);
                    continue;
                }else if(op1->GetOperandType() == BasicOperand::IMMF32 && ((ImmF32Operand*)op1)->GetFloatVal() == 0.0f && op2->GetOperandType() == BasicOperand::REG){
                    regno_map[((RegOperand*)result_op)->GetRegNo()] = ((RegOperand*)op2)->GetRegNo();
                    it = block->Instruction_list.erase(it);
                    continue;
                }
                else{
                    ++it;
                    continue;
                }
            }else if(inst->GetOpcode() == BasicInstruction::SUB){
                auto arith_inst = (ArithmeticInstruction*)inst;
                auto result_op = arith_inst->GetResult();
                auto op1 = arith_inst->GetOperand1();
                auto op2 = arith_inst->GetOperand2();
                if(op1->GetOperandType() == BasicOperand::REG && op2->GetOperandType() == BasicOperand::IMMI32 && ((ImmI32Operand*)op2)->GetIntImmVal() == 0){
                    regno_map[((RegOperand*)result_op)->GetRegNo()] = ((RegOperand*)op1)->GetRegNo();
                    it = block->Instruction_list.erase(it);
                    continue;
                }else{
                    ++it;
                    continue;
                }
            }else if(inst->GetOpcode() == BasicInstruction::FSUB){
                auto farith_inst = (ArithmeticInstruction*)inst;
                auto result_op = farith_inst->GetResult();
                auto op1 = farith_inst->GetOperand1();
                auto op2 = farith_inst->GetOperand2();
                if(op1->GetOperandType() == BasicOperand::REG && op2->GetOperandType() == BasicOperand::IMMF32 && ((ImmF32Operand*)op2)->GetFloatVal() == 0.0f){
                    regno_map[((RegOperand*)result_op)->GetRegNo()] = ((RegOperand*)op1)->GetRegNo();
                    it = block->Instruction_list.erase(it);
                    continue;
                }else{
                    ++it;
                    continue;
                }
            }else if(inst->GetOpcode() == BasicInstruction::MUL){
                auto arith_inst = (ArithmeticInstruction*)inst;
                auto result_op = arith_inst->GetResult();   
                auto op1 = arith_inst->GetOperand1();
                auto op2 = arith_inst->GetOperand2();
                if(op1->GetOperandType() == BasicOperand::REG && op2->GetOperandType() == BasicOperand::IMMI32 && ((ImmI32Operand*)op2)->GetIntImmVal() == 1){
                    regno_map[((RegOperand*)result_op)->GetRegNo()] = ((RegOperand*)op1)->GetRegNo();
                    it = block->Instruction_list.erase(it);
                    continue;
                }else if(op1->GetOperandType() == BasicOperand::IMMI32 && ((ImmI32Operand*)op1)->GetIntImmVal() == 1 && op2->GetOperandType() == BasicOperand::REG){
                    regno_map[((RegOperand*)result_op)->GetRegNo()] = ((RegOperand*)op2)->GetRegNo();
                    it = block->Instruction_list.erase(it);
                    continue;
                }else{  
                    ++it;
                    continue;
                }
            }else if(inst->GetOpcode() == BasicInstruction::FMUL){
                auto farith_inst = (ArithmeticInstruction*)inst;
                auto result_op = farith_inst->GetResult();   
                auto op1 = farith_inst->GetOperand1();
                auto op2 = farith_inst->GetOperand2();
                if(op1->GetOperandType() == BasicOperand::REG && op2->GetOperandType() == BasicOperand::IMMF32 && ((ImmF32Operand*)op2)->GetFloatVal() == 1.0f){
                    regno_map[((RegOperand*)result_op)->GetRegNo()] = ((RegOperand*)op1)->GetRegNo();
                    it = block->Instruction_list.erase(it);
                    continue;
                }else if(op1->GetOperandType() == BasicOperand::IMMF32 && ((ImmF32Operand*)op1)->GetFloatVal() == 1.0f && op2->GetOperandType() == BasicOperand::REG){
                    regno_map[((RegOperand*)result_op)->GetRegNo()] = ((RegOperand*)op2)->GetRegNo();
                    it = block->Instruction_list.erase(it);
                    continue;
                }else{  
                    ++it;
                    continue;
                }
            }else if(inst->GetOpcode() == BasicInstruction::DIV){
                auto arith_inst = (ArithmeticInstruction*)inst;
                auto result_op = arith_inst->GetResult();
                auto op1 = arith_inst->GetOperand1();
                auto op2 = arith_inst->GetOperand2();
                if(op1->GetOperandType() == BasicOperand::REG && op2->GetOperandType() == BasicOperand::IMMI32 && ((ImmI32Operand*)op2)->GetIntImmVal() == 1){
                    regno_map[((RegOperand*)result_op)->GetRegNo()] = ((RegOperand*)op1)->GetRegNo();
                    it = block->Instruction_list.erase(it);
                    continue;
                }else{  
                    ++it;
                    continue;
                }
            }else if(inst->GetOpcode() == BasicInstruction::FDIV){
                auto farith_inst = (ArithmeticInstruction*)inst;
                auto result_op = farith_inst->GetResult();
                auto op1 = farith_inst->GetOperand1();
                auto op2 = farith_inst->GetOperand2();
                if(op1->GetOperandType() == BasicOperand::REG && op2->GetOperandType() == BasicOperand::IMMF32 && ((ImmF32Operand*)op2)->GetFloatVal() == 1.0f){
                    regno_map[((RegOperand*)result_op)->GetRegNo()] = ((RegOperand*)op1)->GetRegNo();
                    it = block->Instruction_list.erase(it);
                    continue;
                }else{
                    ++it;
                    continue;
                }
            }
            ++it;
        }   
    }

    //后续指令的reg替换
    for(auto &[id, block]:*(C->block_map)){
        for(auto &inst : block->Instruction_list){
            auto operands = inst->GetNonResultOperands();
            for(auto &op : operands){
                if(op->GetOperandType() == BasicOperand::REG){
                    int regno = ((RegOperand*)op)->GetRegNo();
                    if(regno_map.count(regno)){
                        int new_regno = regno_map[regno];
                        while(regno_map.count(new_regno)) {
                            new_regno = regno_map[new_regno];
                        }
                        op = GetNewRegOperand(new_regno);
                    }
                }
            }
            inst->SetNonResultOperands(operands);
        }
    }
}



void PeepholePass::DeadArgElim() {
	// find function with dead args
	// deadArg_funcs.name() -> deadArg_funcs.left_arg_id
	std::unordered_map<std::string, std::vector<int>> deadArg_funcs;
	for (auto [defI, cfg] : llvmIR->llvm_cfg) {
		// formals_regNo -> formal_id = arg_id
		std::unordered_map<int, int> formals_reg_map;
		std::unordered_set<int> argReg_occured;
		for(int i = 0; i < defI->formals.size(); i++) {
			int curr_regNo = ((RegOperand*)defI->formals_reg[i])->GetRegNo();
			formals_reg_map[curr_regNo] = i;
		}
		for(auto [bbid, bb] : *(cfg->block_map)) {
			for(auto inst : bb->Instruction_list) {
				for(int useRegno : inst->GetUseRegno()) {
					if(formals_reg_map.empty()) break;
					if(formals_reg_map.count(useRegno)) {
						argReg_occured.insert(useRegno);
						formals_reg_map.erase(useRegno);
					}
				}
			}
		}

		if(!formals_reg_map.empty()) { // not all args have been used.
			std::string func_name = defI->GetFunctionName();
			// std::cerr << "fucntion: " + func_name + "not all args have been used.\n";
			deadArg_funcs[func_name] = std::vector<int> {};
			std::vector<BasicInstruction::LLVMType> new_formals;
    		std::vector<Operand> new_formals_reg;

			for(int i = 0; i < defI->formals.size(); i++) {
				int curr_regNo = ((RegOperand*)defI->formals_reg[i])->GetRegNo();
				if(argReg_occured.count(curr_regNo)) {
					new_formals.push_back(defI->formals[i]);
					new_formals_reg.push_back(defI->formals_reg[i]);
					deadArg_funcs[func_name].push_back(i);
				}
			}

			defI->formals = new_formals;
			defI->formals_reg = new_formals_reg;
		}
    }

	// change all deadArg_funcs call
	for (auto [defI, cfg] : llvmIR->llvm_cfg) {
		for(auto [bbid, bb] : *(cfg->block_map)) {
			for(auto inst : bb->Instruction_list) {
				if (auto call_inst = dynamic_cast<CallInstruction*>(inst)) {
					std::string call_func_name = call_inst->GetFunctionName();
					if(deadArg_funcs.count(call_func_name)) {
						typedef std::vector<std::pair<BasicInstruction::LLVMType, Operand>> argList;
						argList new_args;
						argList args = call_inst->GetParameterList();
						for(int left_arg_id : deadArg_funcs[call_func_name]) {
							new_args.push_back(args[left_arg_id]);
						}
						call_inst->SetParameterList(new_args);
					}
				}
			}
		}
	}
}