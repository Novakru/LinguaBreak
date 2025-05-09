#include"inst_select.h"
#include"machine_instruction.h"

// 所有模板特化声明
template <> void RiscV64Unit::ConvertAndAppend<Instruction>(Instruction inst);
template <> void RiscV64Unit::ConvertAndAppend<ArithmeticInstruction*>(ArithmeticInstruction* inst);
template <> void RiscV64Unit::ConvertAndAppend<LoadInstruction*>(LoadInstruction* inst);
template <> void RiscV64Unit::ConvertAndAppend<StoreInstruction*>(StoreInstruction* inst);
template <> void RiscV64Unit::ConvertAndAppend<IcmpInstruction*>(IcmpInstruction* inst);
template <> void RiscV64Unit::ConvertAndAppend<PhiInstruction*>(PhiInstruction* inst);
template <> void RiscV64Unit::ConvertAndAppend<AllocaInstruction*>(AllocaInstruction* inst);
template <> void RiscV64Unit::ConvertAndAppend<BrCondInstruction*>(BrCondInstruction* inst);
template <> void RiscV64Unit::ConvertAndAppend<BrUncondInstruction*>(BrUncondInstruction* inst);
template <> void RiscV64Unit::ConvertAndAppend<FcmpInstruction*>(FcmpInstruction* inst);
template <> void RiscV64Unit::ConvertAndAppend<RetInstruction*>(RetInstruction* inst);
template <> void RiscV64Unit::ConvertAndAppend<ZextInstruction*>(ZextInstruction* inst);
template <> void RiscV64Unit::ConvertAndAppend<FptosiInstruction*>(FptosiInstruction* inst);
template <> void RiscV64Unit::ConvertAndAppend<GetElementptrInstruction*>(GetElementptrInstruction* inst);
template <> void RiscV64Unit::ConvertAndAppend<CallInstruction*>(CallInstruction* inst);
template <> void RiscV64Unit::ConvertAndAppend<SitofpInstruction*>(SitofpInstruction* inst);

void RiscV64Unit::SelectInstructionAndBuildCFG()
{
    //TODO
    //原架构中dest->xx 直接替换成xx 比如dest->functions直接替换成functions
    LowerFrame();//最后调用LowerFrame
}
void RiscV64Unit::LowerFrame()
{
    //TODO
    //原架构中需要通过unit获取functions等信息，此处直接获取即可
}
void RiscV64Unit::LowerStack()
{
    //TODO
    //原架构中需要通过unit获取functions等信息，此处直接获取即可
}

void RiscV64Unit::ClearFunctionSelectState() { 
    llvmReg_offset_map.clear();
    resReg_cmpInst_map.clear();
    llvm2rv_regmap.clear();
    phi_instr_map.clear();
    terminal_instr_map.clear();
    phi_temp_phiseqmap.clear();
    phi_temp_registermap.clear();
    mem2reg_destruc_set.clear();
    curblockmap.clear();
	cur_offset = 0; 
}

Register RiscV64Unit::GetNewRegister(int llvm_regNo, MachineDataType type) {
	// 不存在llvm_reg到rv_reg的映射，则新建; 否则，直接返回。
	if (llvm2rv_regmap.find(llvm_regNo) == llvm2rv_regmap.end()) {
        llvm2rv_regmap[llvm_regNo] = cur_func->GetNewRegister(type.data_type, type.data_length);
    }
    return llvm2rv_regmap[llvm_regNo];
}

Register RiscV64Unit::GetNewTempRegister(MachineDataType type) {
	return cur_func->GetNewRegister(type.data_type, type.data_length);
}

void RiscV64Unit::InsertImmI32Instruction(Register resultRegister, Operand op, MachineBlock* insert_block, bool isPhiPost){
    // 负数处理: https://blog.csdn.net/qq_52505851/article/details/132085930
    // https://quantaly.github.io/riscv-li/
    // RISC-V handles 32-bit constants and addresses with instructions 
    // that set the upper 20 bits of a 32-bit register. Load upper 
    // immediate lui loads 20 bits into bits 31 through 12. Then a 
    // second instruction such as addi can set the bottom 12 bits. 
    // Small numbers or addresses can be formed by using the zero register instead of lui.
    auto immop = (ImmI32Operand*)op;
    auto imm = immop->GetIntImmVal();
    if(imm >= imm12Min && imm <= imm12Max){
        // addiw        %result, x0, imm
        auto addiw_instr = rvconstructor->ConstructIImm(RISCV_ADDIW, resultRegister, GetPhysicalReg(RISCV_x0), imm);
        if(isPhiPost){
            phi_instr_map[insert_block->getLabelId()].push_back(addiw_instr);
        }else{
            insert_block->push_back(addiw_instr);
        }
    }else{
        // lui          %temp, immHigh20
        // addiw		%result, %temp, immLow12
        auto immLow12 = (imm << 20) >> 20;
        auto immHigh20  = (imm >> 12);
        if(imm < imm12Min){
            immHigh20 = immHigh20 - (immLow12 >= 0);
            immHigh20 = -(immHigh20 ^ 0xFFFFF);
        }else{
            immHigh20 = immHigh20 + (immLow12 < 0);
        }
        auto tempregister = GetNewTempRegister(INT64);
        auto lui_instr = rvconstructor->ConstructUImm(RISCV_LUI, tempregister, immHigh20);
        auto addiw_instr = rvconstructor->ConstructIImm(RISCV_ADDIW, resultRegister, tempregister, immLow12);
        if(isPhiPost){
            phi_instr_map[insert_block->getLabelId()].push_back(lui_instr);
            phi_instr_map[insert_block->getLabelId()].push_back(addiw_instr);
        }else{
            insert_block->push_back(lui_instr);
            insert_block->push_back(addiw_instr);
        }
    }
}

static int double_floatConvert(long long doublebit){
    int origin_E = (doublebit << 1) >> 53;
    int E = (origin_E - ((1 << 10) - 1) + ((1 << 7) - 1)) & EIGHT_BITS;
    int F = (((doublebit << 12) >> 41) + ((1ll << 35) & doublebit)) & TWENTYTHERE_BITS;
    int S = doublebit & (1ll << 63);
    int floatbit = F | (E << 23) | (S << 31);
    // std::cout << "E 十进制: " << E << " 转为十六进制大写: " 
    //           << std::hex << std::uppercase << E << std::endl;
    // std::cout << "F 十进制: " << F << " 转为十六进制大写: " 
    //           << std::hex << std::uppercase << F << std::endl;
    // std::cout << "S 十进制: " << S << " 转为十六进制大写: " 
    //           << std::hex << std::uppercase << S << std::endl;
    // std::cout << "floatbit 十进制: " << floatbit << " 转为十六进制大写: " 
    //           << std::hex << std::uppercase << floatbit << std::endl;
    return floatbit;
}

void RiscV64Unit::InsertImmFloat32Instruction(Register resultRegister, Operand op, MachineBlock* insert_block, bool isPhiPost){
    auto immop = (ImmF32Operand*)op;
    auto imm = immop->GetFloatByteVal();
    if(imm == 0){
        auto fmvwx_instr = rvconstructor->ConstructR2(RISCV_FMV_W_X, resultRegister, GetPhysicalReg(0));
        if(isPhiPost){
            phi_instr_map[insert_block->getLabelId()].push_back(fmvwx_instr);
        }else{
            insert_block->push_back(fmvwx_instr);
        }
        return;
    }
    auto bit_imm = double_floatConvert(immop->GetFloatByteVal());
    // 64位浮点数转32位
    // std::cout << "十进制: " << imm << " 转为十六进制大写: " 
    //           << std::hex << std::uppercase << imm << std::endl;
    auto bit_high32_high20_imm = bit_imm >> 12;
    auto bit_high32_low12_imm = (bit_imm << 20) >> 20;
    if(!bit_high32_low12_imm){
        // lui			%temp, imm
        // fmv.w.x      %ft0, %temp
        auto tempregister = GetNewTempRegister(INT64);
        auto lui_instr = rvconstructor->ConstructUImm(RISCV_LUI, tempregister, bit_high32_high20_imm);
        auto fmvwx_instr = rvconstructor->ConstructR2(RISCV_FMV_W_X, resultRegister, tempregister);
        
        if(isPhiPost){
            phi_instr_map[insert_block->getLabelId()].push_back(lui_instr);
            phi_instr_map[insert_block->getLabelId()].push_back(fmvwx_instr);
        }else{
            insert_block->push_back(lui_instr);
            insert_block->push_back(fmvwx_instr);
        }
    }else{
        // lui          %temp, immHigh20
        // addiw		%temp2, %temp1, immLow12
        // fmv.w.x      %resultRegister, %temp2
        auto immLow12 = bit_high32_low12_imm;
        auto immHigh20  = bit_high32_high20_imm;
        if(imm < imm12Min){
            immHigh20 = immHigh20 - (immLow12 >= 0);
            immHigh20 = -(immHigh20 ^ 0xFFFFF);
        }else{
            immHigh20 = immHigh20 + (immLow12 < 0);
        }
        auto tempregister1 = GetNewTempRegister(INT64);
        auto tempregister2 = GetNewTempRegister(INT64);
        auto lui_instr = rvconstructor->ConstructUImm(RISCV_LUI, tempregister1, immHigh20);
        auto addiw_instr = rvconstructor->ConstructIImm(RISCV_ADDIW, tempregister2, tempregister1, immLow12);
        auto fmvwx_instr = rvconstructor->ConstructR2(RISCV_FMV_W_X, resultRegister, tempregister2);

        if(isPhiPost){
            phi_instr_map[insert_block->getLabelId()].push_back(lui_instr);
            phi_instr_map[insert_block->getLabelId()].push_back(addiw_instr);
            phi_instr_map[insert_block->getLabelId()].push_back(fmvwx_instr);
        }else{
            insert_block->push_back(lui_instr);
            insert_block->push_back(addiw_instr);
            insert_block->push_back(fmvwx_instr);
        }
    }
}

Register RiscV64Unit::GetI32A(int a_num){
    Register registerop;
    switch(a_num){
        case 0:
            registerop = GetPhysicalReg(RISCV_a0);
            break;
        case 1:
            registerop = GetPhysicalReg(RISCV_a1);
            break;
        case 2:
            registerop = GetPhysicalReg(RISCV_a2);
            break;
        case 3:
            registerop = GetPhysicalReg(RISCV_a3);
            break;
        case 4:
            registerop = GetPhysicalReg(RISCV_a4);
            break;
        case 5:
            registerop = GetPhysicalReg(RISCV_a5);
            break;
        case 6:
            registerop = GetPhysicalReg(RISCV_a6);
            break;
        case 7:
            registerop = GetPhysicalReg(RISCV_a7);
            break;
        default:
            registerop = GetPhysicalReg(RISCV_INVALID);
            break;
    }
    return registerop;
}

Register RiscV64Unit::GetF32A(int a_num){
    Register registerop;
    switch(a_num){
        case 0:
            registerop = GetPhysicalReg(RISCV_fa0);
            break;
        case 1:
            registerop = GetPhysicalReg(RISCV_fa1);
            break;
        case 2:
            registerop = GetPhysicalReg(RISCV_fa2);
            break;
        case 3:
            registerop = GetPhysicalReg(RISCV_fa3);
            break;
        case 4:
            registerop = GetPhysicalReg(RISCV_fa4);
            break;
        case 5:
            registerop = GetPhysicalReg(RISCV_fa5);
            break;
        case 6:
            registerop = GetPhysicalReg(RISCV_fa6);
            break;
        case 7:
            registerop = GetPhysicalReg(RISCV_fa7);
            break;
        default:
            registerop = GetPhysicalReg(RISCV_INVALID);
            break;
    }
    return registerop;
}

int RiscV64Unit::InsertArgInStack(BasicInstruction::LLVMType type, Operand arg_op, int &arg_off, MachineBlock* insert_block, int &nr_iarg, int &nr_farg) {
	if (type == BasicInstruction::I32 || type == BasicInstruction::PTR) {
		if (nr_iarg < 8) {
		} else {
			if (arg_op->GetOperandType() == BasicOperand::REG) {
				auto arg_regop = (RegOperand *)arg_op;
				auto arg_reg = GetNewRegister(arg_regop->GetRegNo(), INT64);
				if (llvmReg_offset_map.find(arg_regop->GetRegNo()) == llvmReg_offset_map.end()) {
					insert_block->push_back(
					rvconstructor->ConstructSImm(RISCV_SD, arg_reg, GetPhysicalReg(RISCV_sp), arg_off));
				} else {
					auto sp_offset = llvmReg_offset_map[arg_regop->GetRegNo()];
					auto mid_reg = GetNewTempRegister(INT64);
					insert_block->push_back(
					rvconstructor->ConstructIImm(RISCV_ADDI, mid_reg, GetPhysicalReg(RISCV_sp), sp_offset));
					insert_block->push_back(
					rvconstructor->ConstructSImm(RISCV_SD, mid_reg, GetPhysicalReg(RISCV_sp), arg_off));
				}
			} else if (arg_op->GetOperandType() == BasicOperand::IMMI32) {
				auto arg_immop = (ImmI32Operand *)arg_op;
				auto arg_imm = arg_immop->GetIntImmVal();
				auto imm_reg = GetNewTempRegister(INT64);
				InsertImmI32Instruction(imm_reg, arg_immop, insert_block);
				// insert_block->push_back(rvconstructor->ConstructCopyRegImmI(imm_reg, arg_imm, INT64));
				insert_block->push_back(
				rvconstructor->ConstructSImm(RISCV_SD, imm_reg, GetPhysicalReg(RISCV_sp), arg_off));
			} else if (arg_op->GetOperandType() == BasicOperand::GLOBAL) {
				auto glb_reg1 = GetNewTempRegister(INT64);
				auto glb_reg2 = GetNewTempRegister(INT64);
				auto arg_glbop = (GlobalOperand *)arg_op;
				insert_block->push_back(
				rvconstructor->ConstructULabel(RISCV_LUI, glb_reg1, RiscVLabel(arg_glbop->GetName(), true)));
				insert_block->push_back(rvconstructor->ConstructILabel(RISCV_ADDI, glb_reg2, glb_reg1,
																	RiscVLabel(arg_glbop->GetName(), false)));
				insert_block->push_back(
				rvconstructor->ConstructSImm(RISCV_SD, glb_reg2, GetPhysicalReg(RISCV_sp), arg_off));
			}
			arg_off += 8;
		}
		nr_iarg++;
	} else if (type == BasicInstruction::FLOAT32) {
		if (nr_farg < 8) {
		} else {
			if (arg_op->GetOperandType() == BasicOperand::REG) {
				auto arg_regop = (RegOperand *)arg_op;
				auto arg_reg = GetNewRegister(arg_regop->GetRegNo(), FLOAT64);
				insert_block->push_back(
				rvconstructor->ConstructSImm(RISCV_FSD, arg_reg, GetPhysicalReg(RISCV_sp), arg_off));
			} else if (arg_op->GetOperandType() == BasicOperand::IMMF32) {
				auto arg_immop = (ImmF32Operand *)arg_op;
				auto arg_imm = arg_immop->GetFloatVal();
				auto imm_reg = GetNewTempRegister(INT64);
				InsertImmI32Instruction(imm_reg, arg_immop, insert_block);
				insert_block->push_back(
				rvconstructor->ConstructSImm(RISCV_SD, imm_reg, GetPhysicalReg(RISCV_sp), arg_off));
			} else {
				ERROR("Unexpected Operand type");
			}
			arg_off += 8;
		}
		nr_farg++;
	} else if (type == BasicInstruction::DOUBLE) {
		if (nr_iarg < 8) {
		} else {
			if (arg_op->GetOperandType() == BasicOperand::REG) {
				auto arg_regop = (RegOperand *)arg_op;
				auto arg_reg = GetNewRegister(arg_regop->GetRegNo(), FLOAT64);
				insert_block->push_back(
				rvconstructor->ConstructSImm(RISCV_FSD, arg_reg, GetPhysicalReg(RISCV_sp), arg_off));
			} else {
				ERROR("Unexpected Operand type");
			}
			arg_off += 8;
		}
		nr_iarg++;
	} else {
		ERROR("Unexpected parameter type %d", type);
		return -1;
	}
	return 0;
}

void RiscV64Unit::DomtreeDfs(BasicBlock* ubb, CFG *C){
    auto ubbid = ubb->block_id;
	auto DomTree = (DominatorTree*)(C->DomTree);
	auto domtree = DomTree->dom_tree;
    cur_block = curblockmap[ubbid];
    for (auto instruction : ubb->Instruction_list) {
		// instruction->PrintIR(std::cerr);
        ConvertAndAppend<Instruction>(instruction);
    }
	for (auto vbb : domtree[ubbid]) {
		DomtreeDfs(vbb, C);
	}
}

// 模板特化实现
template <> void RiscV64Unit::ConvertAndAppend<Instruction>(Instruction inst) {
    switch (inst->GetOpcode()) {
    case BasicInstruction::LOAD:
        ConvertAndAppend<LoadInstruction *>((LoadInstruction *)inst);
        break;
    case BasicInstruction::STORE:
        ConvertAndAppend<StoreInstruction *>((StoreInstruction *)inst);
        break;
    case BasicInstruction::ADD:
    case BasicInstruction::SUB:
    case BasicInstruction::MUL:
    case BasicInstruction::DIV:
    case BasicInstruction::FADD:
    case BasicInstruction::FSUB:
    case BasicInstruction::FMUL:
    case BasicInstruction::FDIV:
    case BasicInstruction::MOD:
    case BasicInstruction::SHL:
    case BasicInstruction::BITXOR:
        ConvertAndAppend<ArithmeticInstruction *>((ArithmeticInstruction *)inst);
        break;
    case BasicInstruction::ICMP:
        ConvertAndAppend<IcmpInstruction *>((IcmpInstruction *)inst);
        break;
    case BasicInstruction::FCMP:
        ConvertAndAppend<FcmpInstruction *>((FcmpInstruction *)inst);
        break;
    case BasicInstruction::ALLOCA:
        ConvertAndAppend<AllocaInstruction *>((AllocaInstruction *)inst);
        break;
    case BasicInstruction::BR_COND:
        ConvertAndAppend<BrCondInstruction *>((BrCondInstruction *)inst);
        break;
    case BasicInstruction::BR_UNCOND:
        ConvertAndAppend<BrUncondInstruction *>((BrUncondInstruction *)inst);
        break;
    case BasicInstruction::RET:
        ConvertAndAppend<RetInstruction *>((RetInstruction *)inst);
        break;
    case BasicInstruction::ZEXT:
        ConvertAndAppend<ZextInstruction *>((ZextInstruction *)inst);
        break;
    case BasicInstruction::FPTOSI:
        ConvertAndAppend<FptosiInstruction *>((FptosiInstruction *)inst);
        break;
    case BasicInstruction::SITOFP:
        ConvertAndAppend<SitofpInstruction *>((SitofpInstruction *)inst);
        break;
    case BasicInstruction::GETELEMENTPTR:
        ConvertAndAppend<GetElementptrInstruction *>((GetElementptrInstruction *)inst);
        break;
    case BasicInstruction::CALL:
        ConvertAndAppend<CallInstruction *>((CallInstruction *)inst);
        break;
    case BasicInstruction::PHI:
        ConvertAndAppend<PhiInstruction *>((PhiInstruction *)inst);
        break;
    default:
        ERROR("Unknown LLVM IR instruction");
    }
}

template <> void RiscV64Unit::ConvertAndAppend<ArithmeticInstruction*>(ArithmeticInstruction* inst) {
    // TODO: 实现算术指令的转换
}

template <> void RiscV64Unit::ConvertAndAppend<LoadInstruction*>(LoadInstruction* inst) {
    // TODO: 实现加载指令的转换
}

template <> void RiscV64Unit::ConvertAndAppend<StoreInstruction*>(StoreInstruction* inst) {
    // TODO: 实现存储指令的转换
}

template <> void RiscV64Unit::ConvertAndAppend<IcmpInstruction*>(IcmpInstruction* inst) {
    // TODO: 实现整数比较指令的转换
}

template <> void RiscV64Unit::ConvertAndAppend<PhiInstruction*>(PhiInstruction* inst) {
    // TODO: 实现Phi指令的转换
}

template <> void RiscV64Unit::ConvertAndAppend<AllocaInstruction*>(AllocaInstruction* inst) {
    // TODO: 实现内存分配指令的转换
}

template <> void RiscV64Unit::ConvertAndAppend<BrCondInstruction*>(BrCondInstruction* inst) {
    // TODO: 实现条件分支指令的转换
}

template <> void RiscV64Unit::ConvertAndAppend<BrUncondInstruction*>(BrUncondInstruction* inst) {
    // TODO: 实现无条件分支指令的转换
}

template <> void RiscV64Unit::ConvertAndAppend<FcmpInstruction*>(FcmpInstruction* inst) {
    // TODO: 实现浮点比较指令的转换
}

template <> void RiscV64Unit::ConvertAndAppend<RetInstruction*>(RetInstruction* inst) {
    // TODO: 实现返回指令的转换
}

template <> void RiscV64Unit::ConvertAndAppend<ZextInstruction*>(ZextInstruction* inst) {
    // TODO: 实现零扩展指令的转换
}

template <> void RiscV64Unit::ConvertAndAppend<FptosiInstruction*>(FptosiInstruction* inst) {
    // TODO: 实现浮点到整数转换指令的转换
}

template <> void RiscV64Unit::ConvertAndAppend<GetElementptrInstruction*>(GetElementptrInstruction* inst) {
    // TODO: 实现获取元素指针指令的转换
}

template <> void RiscV64Unit::ConvertAndAppend<CallInstruction*>(CallInstruction* inst) {
    // TODO: 实现函数调用指令的转换
}

template <> void RiscV64Unit::ConvertAndAppend<SitofpInstruction*>(SitofpInstruction* inst) {
    // TODO: 实现整数到浮点转换指令的转换
}
