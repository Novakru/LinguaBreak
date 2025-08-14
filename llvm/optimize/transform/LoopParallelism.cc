#include "LoopParallelism.h"
#include <iostream>
#include <sstream>
#include <iomanip>

#define LOOP_PARALLELISM_DEBUG 1

#if LOOP_PARALLELISM_DEBUG
#define PARALLEL_DEBUG(x) do { x; } while(0)
#else
#define PARALLEL_DEBUG(x) do {} while(0)
#endif

void LoopParallelismPass::Execute() {
    PARALLEL_DEBUG(std::cout << "=== 开始自动并行化Pass ===" << std::endl);
    
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        PARALLEL_DEBUG(std::cout << "处理函数: " << cfg->function_def->GetFunctionName() << std::endl);
        
        ProcessFunction(cfg);
        
        // 重新构建CFG
        cfg->BuildCFG();
        cfg->getDomTree()->BuildDominatorTree();
    }
    
    PARALLEL_DEBUG(std::cout << "=== 自动并行化Pass完成 ===" << std::endl);
}

void LoopParallelismPass::ProcessFunction(CFG* cfg) {
    auto loop_info = cfg->getLoopInfo();
    if (!loop_info) return;
    
    ScalarEvolution* SE = cfg->getSCEVInfo();
    if (!SE) return;
    
    // 添加运行时库函数声明
    AddRuntimeLibraryDeclarations(cfg);
    
    for (auto loop : loop_info->getTopLevelLoops()) {
        PARALLEL_DEBUG(std::cout << "分析循环，header block_id=" << loop->getHeader()->block_id << std::endl);
        
        ProcessLoop(loop, cfg, SE);
    }
}

void LoopParallelismPass::ProcessLoop(Loop* loop, CFG* cfg, ScalarEvolution* SE) {
    // 递归处理子循环
    for (auto sub_loop : loop->getSubLoops()) {
        ProcessLoop(sub_loop, cfg, SE);
    }
    
    // 检查循环是否可以并行化
    if (!CanParallelizeLoop(loop, cfg, SE)) {
        PARALLEL_DEBUG(std::cout << "循环不可并行化" << std::endl);
        return;
    }
    
    PARALLEL_DEBUG(std::cout << "开始并行化循环" << std::endl);
    
    // 提取循环体到函数
    ExtractLoopBodyToFunction(loop, cfg, SE);
}

bool LoopParallelismPass::CanParallelizeLoop(Loop* loop, CFG* cfg, ScalarEvolution* SE) {
    // 检查是否是简单循环
    if (!IsSimpleLoop(loop, cfg)) {
        PARALLEL_DEBUG(std::cout << "不是简单循环" << std::endl);
        return false;
    }
    
    // 检查是否有循环携带依赖
    if (!HasNoLoopCarriedDependencies(loop, cfg)) {
        PARALLEL_DEBUG(std::cout << "存在循环携带依赖" << std::endl);
        return false;
    }
    
    // 检查迭代次数是否为常数或者循环不变量
    if (!IsConstantIterationCount(loop, cfg, SE)) {
        PARALLEL_DEBUG(std::cout << "迭代次数不是常数" << std::endl);
        return false;
    }
    
    // // 检查循环体大小（避免并行化开销过大）
    // const int MIN_LOOP_BODY_SIZE = 10;
    // const int MAX_LOOP_BODY_SIZE = 500;
    
    // int loop_body_size = 0;
    // for (auto block : loop->getBlocks()) {
    //     loop_body_size += block->Instruction_list.size();
    // }
    
    // if (loop_body_size < MIN_LOOP_BODY_SIZE) {
    //     PARALLEL_DEBUG(std::cout << "循环体过小（" << loop_body_size << "），不值得并行化" << std::endl);
    //     return false;
    // }
    
    // if (loop_body_size > MAX_LOOP_BODY_SIZE) {
    //     PARALLEL_DEBUG(std::cout << "循环体过大（" << loop_body_size << "），并行化开销过大" << std::endl);
    //     return false;
    // }
    
    return true;
}

bool LoopParallelismPass::IsSimpleLoop(Loop* loop, CFG* cfg) {
    // 检查循环出口是否唯一
    if (loop->getExits().size() != 1 || loop->getExitings().size() != 1) {
        return false;
    }
    
    // 检查循环头部是否是条件跳转
    LLVMBlock header = loop->getHeader();
    if (header->Instruction_list.empty()) return false;
    
    Instruction last_inst = header->Instruction_list.back();
    if (last_inst->GetOpcode() != BasicInstruction::BR_COND) {
        return false;
    }
    
    // 检查循环条件是否是简单比较
    BrCondInstruction* br_cond = (BrCondInstruction*)last_inst;
    Operand cond = br_cond->GetCond();
    
    Instruction cond_inst = nullptr;
    if (cond->GetOperandType() == BasicOperand::REG) {
        int regno = ((RegOperand*)cond)->GetRegNo();
        cond_inst = cfg->def_map[regno];
    }
    if (!cond_inst || cond_inst->GetOpcode() != BasicInstruction::ICMP) {
        return false;
    }
    
    return true;
}

bool LoopParallelismPass::HasNoLoopCarriedDependencies(Loop* loop, CFG* cfg) {
    // 简化版本：检查是否有数组访问
    // 在实际实现中，这里应该进行更复杂的依赖分析
    
    for (auto block : loop->getBlocks()) {
        for (auto inst : block->Instruction_list) {
            // 检查是否有数组访问（GEP指令）
            if (inst->GetOpcode() == BasicInstruction::GETELEMENTPTR) {
                // 简化处理：如果数组访问的索引是循环变量，可能存在依赖
                GetElementptrInstruction* gep = (GetElementptrInstruction*)inst;
                auto indices = gep->GetIndexes();
                
                for (auto idx : indices) {
                    if (idx->GetOperandType() == BasicOperand::REG) {
                        int regno = ((RegOperand*)idx)->GetRegNo();
                        // 检查这个寄存器是否是循环变量
                        // 这里简化处理，实际应该检查是否是归纳变量
                        return false; // 暂时保守处理
                    }
                }
            }
        }
    }
    
    return true;
}

bool LoopParallelismPass::IsConstantIterationCount(Loop* loop, CFG* cfg, ScalarEvolution* SE) {
    // 使用extractLoopParams来检查循环参数
    auto loop_params = SE->extractLoopParams(loop, cfg);
    
    // 检查是否为简单循环且迭代次数为常数
    return loop_params.is_simple_loop && loop_params.count > 0;
}

void LoopParallelismPass::ExtractLoopBodyToFunction(Loop* loop, CFG* cfg, ScalarEvolution* SE) {
    std::string func_name = GenerateFunctionName(cfg, loop);
    
    // 创建并行化函数
    CreateParallelFunction(loop, cfg, func_name, SE);
    
    // 创建并行化调用
    CreateConditionalParallelization(loop, cfg, func_name, SE);
}

std::string LoopParallelismPass::GenerateFunctionName(CFG* cfg, Loop* loop) {
    std::string base_name = "___parallel_loop_" + cfg->function_def->GetFunctionName() + "_" + 
                           std::to_string(loop->getHeader()->block_id);
    
    return GenerateUniqueName(base_name);
}

std::string LoopParallelismPass::GenerateUniqueName(const std::string& base_name) {
    if (name_counter.find(base_name) == name_counter.end()) {
        name_counter[base_name] = 0;
    }
    
    std::stringstream ss;
    ss << base_name << "_" << std::setw(3) << std::setfill('0') << name_counter[base_name]++;
    
    return ss.str();
}

void LoopParallelismPass::CreateParallelFunction(Loop* loop, CFG* cfg, const std::string& func_name, ScalarEvolution* SE) {
    // 创建新的函数定义
    auto func_def = new FunctionDefineInstruction(BasicInstruction::LLVMType::VOID, func_name);
    
    // 添加函数参数：void* args
    func_def->InsertFormal(BasicInstruction::LLVMType::PTR);
    
    // 使用IR的NewFunction方法
    llvmIR->NewFunction(func_def);
	llvmIR->function_max_reg[func_def] = 0;  // ptr arg is %r0
	llvmIR->function_max_label[func_def] = -1;
    
    // 全局维护寄存器映射
    std::map<int, int> reg_mapping;
    
    // 添加函数参数处理
    AddFunctionParameters(func_def, reg_mapping);
    std::cout << "AddFunctionParameters" << std::endl;
    // 添加线程范围计算
    AddThreadRangeCalculation(func_def, reg_mapping);
	std::cout << "AddThreadRangeCalculation" << std::endl;

	// 复制循环体指令到基本块中，后续再根据基本块构建 new_cfg
	CopyLoopBodyInstructions(loop, func_def, reg_mapping);
	std::cout << "CopyLoopBodyInstructions" << std::endl;
    
    PARALLEL_DEBUG(std::cout << "创建并行化函数: " << func_name << std::endl);
}

void LoopParallelismPass::AddFunctionParameters(FunctionDefineInstruction* func_def, std::map<int, int>& reg_mapping) {
    // 解析void* args参数
    // args[0] = thread_id, args[1] = start, args[2] = end
    
    // 创建寄存器来存储解析的参数
	int& max_reg = llvmIR->function_max_reg[func_def];
	int& max_label = llvmIR->function_max_label[func_def];
    auto thread_id_gep_reg = GetNewRegOperand(++max_reg);
	auto thread_id_reg = GetNewRegOperand(++max_reg);
	thread_id_regNo = max_reg;
	auto start_gep_reg = GetNewRegOperand(++max_reg);
    auto start_reg = GetNewRegOperand(++max_reg);
	start_regNo = max_reg;
	auto end_gep_reg = GetNewRegOperand(++max_reg);
    auto end_reg = GetNewRegOperand(++max_reg);
	end_regNo = max_reg;
	
	auto func_block = llvmIR->NewBlock(func_def, ++max_label);
    
    // 获取函数参数
    auto args_param = func_def->GetNonResultOperands()[0];
    
    // 创建参数解析指令
    // thread_id = ((int*)args)[0]
    auto thread_id_gep = new GetElementptrInstruction(
        BasicInstruction::LLVMType::I32, thread_id_gep_reg, args_param, BasicInstruction::LLVMType::I32);
    thread_id_gep->push_idx_imm32(0);
    func_block->Instruction_list.push_back(thread_id_gep);
    
    auto thread_id_load = new LoadInstruction(BasicInstruction::LLVMType::I32, thread_id_gep_reg, thread_id_reg);
    func_block->Instruction_list.push_back(thread_id_load);
    
    // start = ((int*)args)[1]
    auto start_gep = new GetElementptrInstruction(
        BasicInstruction::LLVMType::I32, start_gep_reg, args_param, BasicInstruction::LLVMType::I32);
    start_gep->push_idx_imm32(1);
    func_block->Instruction_list.push_back(start_gep);
    
    auto start_load = new LoadInstruction(BasicInstruction::LLVMType::I32, start_gep_reg, start_reg);
    func_block->Instruction_list.push_back(start_load);
    
    // end = ((int*)args)[2]
    auto end_gep = new GetElementptrInstruction(
        BasicInstruction::LLVMType::I32, end_gep_reg, args_param, BasicInstruction::LLVMType::I32);
    end_gep->push_idx_imm32(2);
    func_block->Instruction_list.push_back(end_gep);
    
    auto end_load = new LoadInstruction(BasicInstruction::LLVMType::I32, end_gep_reg, end_reg);
    func_block->Instruction_list.push_back(end_load);
    
    // 处理循环外部变量
    // 根据len1和len2参数加载外部变量
    int bias = 3; // thread_id, start, end 之后
    
    // 加载I32和FLOAT32类型的外部变量
    for (int i = 0; i < 10; i++) { // 假设最多10个变量
        bias += 1;
        auto var_gep_reg = GetNewRegOperand(++max_reg);
        auto var_gep = new GetElementptrInstruction(
            BasicInstruction::LLVMType::I32, var_gep_reg, args_param, BasicInstruction::LLVMType::I32);
        var_gep->push_idx_imm32(bias);
        func_block->Instruction_list.push_back(var_gep);
        
        auto var_reg = GetNewRegOperand(++max_reg);
        // 根据变量类型选择加载类型
        auto var_load = new LoadInstruction(BasicInstruction::LLVMType::I32, var_gep_reg, var_reg);
        func_block->Instruction_list.push_back(var_load);
        
        // 将变量添加到映射中，供后续循环体使用
        reg_mapping[1000 + i] = max_reg; // 使用特殊编号避免冲突
    }
    
    // 加载PTR类型的外部变量
    for (int i = 0; i < 5; i++) { // 假设最多5个PTR变量
        bias += 2; // PTR变量占用8字节
        auto var_gep_reg = GetNewRegOperand(++max_reg);
        auto var_gep = new GetElementptrInstruction(
            BasicInstruction::LLVMType::I32, var_gep_reg, args_param, BasicInstruction::LLVMType::I32);
        var_gep->push_idx_imm32(bias);
        func_block->Instruction_list.push_back(var_gep);
        
        auto var_reg = GetNewRegOperand(++max_reg);
        auto var_load = new LoadInstruction(BasicInstruction::LLVMType::PTR, var_gep_reg, var_reg);
        func_block->Instruction_list.push_back(var_load);
        
        // 将变量添加到映射中
        reg_mapping[2000 + i] = max_reg; // 使用特殊编号避免冲突
    }
}

void LoopParallelismPass::AddThreadRangeCalculation(FunctionDefineInstruction* func_def, std::map<int, int>& reg_mapping) {
    // 计算每个线程的循环范围
    // part = (end - start) / 4
    // my_start = part * thread_id + start
    // my_end = part * (thread_id + 1) + start
    
    int& max_reg = llvmIR->function_max_reg[func_def];
    int& max_label = llvmIR->function_max_label[func_def];
    auto func_block = llvmIR->function_block_map[func_def][0];
    
    // 获取之前解析的参数
	// r0 - ((int*)args), r2 - ((int*)args)[0], r4 - ((int*)args)[1], r6 - ((int*)args)[2]
    auto thread_id_reg = GetNewRegOperand(thread_id_regNo);
    auto start_reg = GetNewRegOperand(start_regNo);
    auto end_reg = GetNewRegOperand(end_regNo);
    
    // part = (end - start) / 4
    auto diff_reg = GetNewRegOperand(++max_reg);
    auto sub_inst = new ArithmeticInstruction(BasicInstruction::SUB, BasicInstruction::LLVMType::I32, end_reg, start_reg, diff_reg);
    func_block->Instruction_list.push_back(sub_inst);
    
	auto part_reg = GetNewRegOperand(++max_reg);
    auto div_inst = new ArithmeticInstruction(BasicInstruction::DIV, BasicInstruction::LLVMType::I32, diff_reg, new ImmI32Operand(4), part_reg);
    func_block->Instruction_list.push_back(div_inst);
    
    // my_start = part * thread_id + start
	auto mid_reg = GetNewRegOperand(++max_reg);
    auto mul_inst = new ArithmeticInstruction(BasicInstruction::MUL, BasicInstruction::LLVMType::I32, part_reg, thread_id_reg, mid_reg);
    func_block->Instruction_list.push_back(mul_inst);
    
	auto my_start_reg = GetNewRegOperand(++max_reg);
	my_start_regNo = max_reg;
    auto add_inst = new ArithmeticInstruction(BasicInstruction::ADD, BasicInstruction::LLVMType::I32, mid_reg, start_reg, my_start_reg);
    func_block->Instruction_list.push_back(add_inst);
    
    // 计算 my_end，需要处理最后一个线程的特殊情况
    // my_end = part * (thread_id + 1) + start = my_start + part
    auto temp_reg = GetNewRegOperand(++max_reg);
    auto add2_inst = new ArithmeticInstruction(BasicInstruction::ADD, BasicInstruction::LLVMType::I32, my_start_reg, part_reg, temp_reg);
    func_block->Instruction_list.push_back(add2_inst);
    
    // 创建条件判断：if (thread_id == 3) my_end = end; else my_end = temp_reg;
    auto cmp_reg = GetNewRegOperand(++max_reg);
    auto cmp_inst = new IcmpInstruction(BasicInstruction::LLVMType::I32, thread_id_reg, new ImmI32Operand(3), IcmpInstruction::eq, cmp_reg);
    func_block->Instruction_list.push_back(cmp_inst);
    
    // 创建分支结构
    auto branch_block = llvmIR->NewBlock(func_def, ++max_label);
    auto merge_block = llvmIR->NewBlock(func_def, ++max_label);
    
    // 条件跳转
    auto br_cond_inst = new BrCondInstruction(cmp_reg, GetNewLabelOperand(branch_block->block_id), GetNewLabelOperand(merge_block->block_id));
    func_block->Instruction_list.push_back(br_cond_inst);
    
    // branch_block: my_end = end
    auto br_uncond1 = new BrUncondInstruction(GetNewLabelOperand(merge_block->block_id));
    branch_block->Instruction_list.push_back(br_uncond1);

    // 在merge_block中使用PHI指令汇总结果
    auto my_end_reg = GetNewRegOperand(++max_reg);
	my_end_regNo = max_reg;
    std::vector<std::pair<Operand, Operand>> phi_list;
    phi_list.push_back({GetNewLabelOperand(branch_block->block_id), end_reg});
    phi_list.push_back({GetNewLabelOperand(func_block->block_id), temp_reg});
    auto phi_inst = new PhiInstruction(BasicInstruction::LLVMType::I32, my_end_reg, phi_list);
    merge_block->Instruction_list.push_back(phi_inst);
}

void LoopParallelismPass::CopyLoopBodyInstructions(Loop* loop, FunctionDefineInstruction* func_def, std::map<int, int>& reg_mapping) {
    // 识别循环中使用但在循环外定义的变量，并添加到映射中
    for (auto bb : loop->getBlocks()) {
        for (auto I : bb->Instruction_list) {
            for (auto op : I->GetNonResultOperands()) {
                if (op->GetOperandType() == BasicOperand::REG) {
                    auto r = (RegOperand*)op;
                    int regno = r->GetRegNo();
                    if (reg_mapping.find(regno) == reg_mapping.end()) {
                        // 为循环外部变量创建新的寄存器
                        int& max_reg = llvmIR->function_max_reg[func_def];
                        int new_regno = ++max_reg;
                        reg_mapping[regno] = new_regno;
                    }
                }
            }
        }
    }
    // 创建新的基本块映射
    std::map<int, int> labelreplace_map;
    
    // 为循环中的每个基本块创建新的基本块
	int& max_label = llvmIR->function_max_label[func_def];
    for (auto block : loop->getBlocks()) {
        auto new_block = llvmIR->NewBlock(func_def, ++max_label);
        labelreplace_map[block->block_id] = new_block->block_id;
        
        // 复制基本块中的指令
        for (auto inst : block->Instruction_list) {
            // 克隆指令
            Instruction cloned_inst = CloneInstruction(inst, func_def, reg_mapping);
            if (cloned_inst) {
                new_block->Instruction_list.push_back(cloned_inst);
            }
        }
    }
    
    // 更新跳转指令中的标签
    for (auto [id, bb] : llvmIR->function_block_map[func_def]) {
        for (auto inst : bb->Instruction_list) {
            if (inst->GetOpcode() == BasicInstruction::BR_UNCOND) {
                BrUncondInstruction* br_inst = (BrUncondInstruction*)inst;
                Operand dest_label = br_inst->GetDestLabel();
                if (dest_label->GetOperandType() == BasicOperand::LABEL) {
                    int old_label = ((LabelOperand*)dest_label)->GetLabelNo();
                    if (labelreplace_map.find(old_label) != labelreplace_map.end()) {
                        int new_label = labelreplace_map[old_label];
                        br_inst->SetTarget(GetNewLabelOperand(new_label));
                    }
                }
            } else if (inst->GetOpcode() == BasicInstruction::BR_COND) {
                BrCondInstruction* br_inst = (BrCondInstruction*)inst;
                Operand true_label = br_inst->GetTrueLabel();
                Operand false_label = br_inst->GetFalseLabel();
                
                if (true_label->GetOperandType() == BasicOperand::LABEL) {
                    int old_label = ((LabelOperand*)true_label)->GetLabelNo();
                    if (labelreplace_map.find(old_label) != labelreplace_map.end()) {
                        int new_label = labelreplace_map[old_label];
                        br_inst->SetTrueLabel(GetNewLabelOperand(new_label));
                    }
                }
                
                if (false_label->GetOperandType() == BasicOperand::LABEL) {
                    int old_label = ((LabelOperand*)false_label)->GetLabelNo();
                    if (labelreplace_map.find(old_label) != labelreplace_map.end()) {
                        int new_label = labelreplace_map[old_label];
                        br_inst->SetFalseLabel(GetNewLabelOperand(new_label));
                    }
                }
            }
        }
    }
}

Instruction LoopParallelismPass::CloneInstruction(Instruction inst, FunctionDefineInstruction* func_def, std::map<int, int>& reg_mapping) {
    // 使用CopyInstruction方法
    Instruction cloned_inst = inst->Clone();
    
    if (!cloned_inst) {
        return nullptr;
    }
    
    // 为指令定义的新寄存器创建映射
    int def_reg = inst->GetDefRegno();
    if (def_reg != -1 && reg_mapping.find(def_reg) == reg_mapping.end()) {
        // 为新的定义寄存器创建映射
        int& max_reg = llvmIR->function_max_reg[func_def];
        int new_regno = ++max_reg;
        reg_mapping[def_reg] = new_regno;
    }
    
    // 应用寄存器映射
    if (!reg_mapping.empty()) {
        // 获取指令使用的寄存器
        auto use_regs = inst->GetUseRegno();
        std::map<int, int> use_map;
        
        for (int reg : use_regs) {
            if (reg_mapping.find(reg) != reg_mapping.end()) {
                use_map[reg] = reg_mapping[reg];
            }
        }
        
        // 获取指令定义的寄存器
        std::map<int, int> def_map;
        if (def_reg != -1 && reg_mapping.find(def_reg) != reg_mapping.end()) {
            def_map[def_reg] = reg_mapping[def_reg];
        }
        
        // 应用寄存器替换
        cloned_inst->ChangeReg(def_map, use_map);
    }
    
    return cloned_inst;
}

void LoopParallelismPass::CreateConditionalParallelization(Loop* loop, CFG* cfg, const std::string& func_name, 
                                                          ScalarEvolution* SE) {
    // 方案二：创建分支结构
    // if (iterations < threshold) {
    //     // 执行原循环（串行）
    // } else {
    //     // 执行并行化版本
    // }
    
    // 使用extractLoopParams获取循环参数
    auto loop_params = SE->extractLoopParams(loop, cfg);
    
    const int PARALLEL_THRESHOLD = 100;
    
    if (loop_params.is_simple_loop && loop_params.count >= PARALLEL_THRESHOLD) {
        // 创建并行化调用
        CreateParallelCall(loop, cfg, func_name, SE);
    }
}

void LoopParallelismPass::CreateParallelCall(Loop* loop, CFG* cfg, const std::string& func_name, ScalarEvolution* SE) {
    // 创建调用运行时库函数的指令
    // parallel_loop_execute(func_name, start, end, len1, len2, ...)
    
    // 使用extractLoopParams获取循环参数
    auto loop_params = SE->extractLoopParams(loop, cfg);
    
    // 收集循环外部变量
    std::set<int> i32set, i64set;
    std::set<int> float32set;
    
    // 获取循环中使用的寄存器映射
    std::map<int, Instruction> ResultMap;
    for (auto [id, bb] : *cfg->block_map) {
        for (auto I : bb->Instruction_list) {
            Operand v = I->GetResult();
            if (auto v_reg = dynamic_cast<RegOperand*>(v)) {
                ResultMap[v_reg->GetRegNo()] = I;
            }
        }
    }
    
    // 识别循环中使用但在循环外定义的变量
    for (auto bb : loop->getBlocks()) {
        for (auto I : bb->Instruction_list) {
            for (auto op : I->GetNonResultOperands()) {
                if (op->GetOperandType() == BasicOperand::REG) {
                    auto r = (RegOperand*)op;
                    int regno = r->GetRegNo();
                    if (ResultMap.find(regno) != ResultMap.end()) {
                        auto outloop_defI = ResultMap[regno];
                        auto def_bbid = ResultMap[regno]->GetBlockID();
                        auto def_bb = (*cfg->block_map)[def_bbid];
                        if (!loop->contains(def_bb)) {
                            auto type = outloop_defI->GetType();
                            if (type == BasicInstruction::LLVMType::I32) {
                                i32set.insert(regno);
                            } else if (type == BasicInstruction::LLVMType::PTR) {
                                i64set.insert(regno);
                            } else if (type == BasicInstruction::LLVMType::FLOAT32) {
                                float32set.insert(regno);
                                i32set.insert(regno);
                            }
                        }
                    }
                }
            }
        }
    }
    
    // 创建参数列表
    std::vector<std::pair<BasicInstruction::LLVMType, Operand>> params;
    
    // 函数指针参数
    auto func_ptr = GetNewGlobalOperand(func_name);
    params.push_back({BasicInstruction::LLVMType::PTR, func_ptr});
    
    // 循环起始和结束
    params.push_back({BasicInstruction::LLVMType::I32, new ImmI32Operand(loop_params.start_val)});
    params.push_back({BasicInstruction::LLVMType::I32, new ImmI32Operand(loop_params.bound_val)});
    
    // I32变量数量和PTR变量数量
    params.push_back({BasicInstruction::LLVMType::I32, new ImmI32Operand(i32set.size())});
    params.push_back({BasicInstruction::LLVMType::I32, new ImmI32Operand(i64set.size())});
    
    // 添加I32类型的外部变量
    for (auto rn : i32set) {
        if (float32set.find(rn) != float32set.end()) {
            // FLOAT32直接传递，不需要转换
            params.push_back({BasicInstruction::LLVMType::FLOAT32, GetNewRegOperand(rn)});
        } else {
            // I32类型直接传递
            params.push_back({BasicInstruction::LLVMType::I32, GetNewRegOperand(rn)});
        }
    }
    
    // 添加PTR类型的外部变量
    for (auto rn : i64set) {
        params.push_back({BasicInstruction::LLVMType::PTR, GetNewRegOperand(rn)});
    }

    auto call_inst = new CallInstruction(BasicInstruction::LLVMType::VOID, nullptr, "parallel_loop_execute", params);
    
    // 将调用指令插入到循环头部之前
    LLVMBlock preheader = loop->getPreheader();
    if (preheader) {
        preheader->Instruction_list.push_back(call_inst);
    }
    
    PARALLEL_DEBUG(std::cout << "创建并行化调用: " << func_name << std::endl);
}

void LoopParallelismPass::AddRuntimeLibraryDeclarations(CFG* cfg) {
    // 添加运行时库函数声明
    // void parallel_loop_execute(void* func_ptr, int start, int end, int len1, int len2, ...);
    
    // 检查是否已经声明过
    static bool declared = false;
    if (declared) return;
    
    // 创建函数声明
    auto decl = new FunctionDefineInstruction(BasicInstruction::LLVMType::VOID, "parallel_loop_execute");
    decl->InsertFormal(BasicInstruction::LLVMType::PTR);  // func_ptr
    decl->InsertFormal(BasicInstruction::LLVMType::I32);  // start
    decl->InsertFormal(BasicInstruction::LLVMType::I32);  // end
    decl->InsertFormal(BasicInstruction::LLVMType::I32);  // len1 (I32变量数量)
    decl->InsertFormal(BasicInstruction::LLVMType::I32);  // len2 (PTR变量数量)
    
    // 添加到全局函数表
    llvmIR->FunctionNameTable["parallel_loop_execute"] = decl;
    
    declared = true;
    
    PARALLEL_DEBUG(std::cout << "添加运行时库函数声明" << std::endl);
}
