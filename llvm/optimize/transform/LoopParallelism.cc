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
    
    // 检查迭代次数是否为常数
    if (!IsConstantIterationCount(loop, cfg, SE)) {
        PARALLEL_DEBUG(std::cout << "迭代次数不是常数" << std::endl);
        return false;
    }
    
    // 检查循环体大小（避免并行化开销过大）
    const int MIN_LOOP_BODY_SIZE = 10;
    const int MAX_LOOP_BODY_SIZE = 500;
    
    int loop_body_size = 0;
    for (auto block : loop->getBlocks()) {
        loop_body_size += block->Instruction_list.size();
    }
    
    if (loop_body_size < MIN_LOOP_BODY_SIZE) {
        PARALLEL_DEBUG(std::cout << "循环体过小（" << loop_body_size << "），不值得并行化" << std::endl);
        return false;
    }
    
    if (loop_body_size > MAX_LOOP_BODY_SIZE) {
        PARALLEL_DEBUG(std::cout << "循环体过大（" << loop_body_size << "），并行化开销过大" << std::endl);
        return false;
    }
    
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
    
    // 创建新的CFG
    auto new_cfg = new CFG();
	new_cfg->function_def = func_def;
    llvmIR->llvm_cfg[func_def] = new_cfg;
    
    // 创建函数体基本块
    LLVMBlock func_block = new BasicBlock(0);
    (*new_cfg->block_map)[0] = func_block;
    
    // 添加函数参数处理
    AddFunctionParameters(func_block, new_cfg);
    
    // 添加线程范围计算
    AddThreadRangeCalculation(func_block, new_cfg);
    
    // 复制循环体指令
    std::map<int, int> reg_mapping;
    CopyLoopBodyInstructions(loop, new_cfg, func_block, reg_mapping);
    
    // 添加返回指令
    auto ret_inst = new RetInstruction(BasicInstruction::LLVMType::VOID, nullptr);
    func_block->Instruction_list.push_back(ret_inst);
    
    PARALLEL_DEBUG(std::cout << "创建并行化函数: " << func_name << std::endl);
}

void LoopParallelismPass::AddFunctionParameters(LLVMBlock& func_block, CFG* cfg) {
    // 解析void* args参数
    // args[0] = thread_id, args[1] = start, args[2] = end
    
    // 创建寄存器来存储解析的参数
    auto thread_id_reg = GetNewRegOperand(++cfg->max_reg);
    auto start_reg = GetNewRegOperand(++cfg->max_reg);
    auto end_reg = GetNewRegOperand(++cfg->max_reg);
    
    // 获取函数参数
    auto args_param = cfg->function_def->GetNonResultOperands()[0];
    
    // 创建参数解析指令
    // thread_id = ((int*)args)[0]
    auto thread_id_gep = new GetElementptrInstruction(
        BasicInstruction::LLVMType::I32, thread_id_reg, args_param, 
        std::vector<int>{}, std::vector<Operand>{new ImmI32Operand(0)}, BasicInstruction::LLVMType::I32);
    func_block->Instruction_list.push_back(thread_id_gep);
    
    auto thread_id_load = new LoadInstruction(BasicInstruction::LLVMType::I32, thread_id_gep->GetResult(), thread_id_reg);
    func_block->Instruction_list.push_back(thread_id_load);
    
    // start = ((int*)args)[1]
    auto start_gep = new GetElementptrInstruction(
        BasicInstruction::LLVMType::I32, start_reg, args_param,
        std::vector<int>{}, std::vector<Operand>{new ImmI32Operand(1)}, BasicInstruction::LLVMType::I32);
    func_block->Instruction_list.push_back(start_gep);
    
    auto start_load = new LoadInstruction(BasicInstruction::LLVMType::I32, start_gep->GetResult(), start_reg);
    func_block->Instruction_list.push_back(start_load);
    
    // end = ((int*)args)[2]
    auto end_gep = new GetElementptrInstruction(
        BasicInstruction::LLVMType::I32, end_reg, args_param,
        std::vector<int>{}, std::vector<Operand>{new ImmI32Operand(2)}, BasicInstruction::LLVMType::I32);
    func_block->Instruction_list.push_back(end_gep);
    
    auto end_load = new LoadInstruction(BasicInstruction::LLVMType::I32, end_gep->GetResult(), end_reg);
    func_block->Instruction_list.push_back(end_load);
}

void LoopParallelismPass::AddThreadRangeCalculation(LLVMBlock& func_block, CFG* cfg) {
    // 简化处理：直接使用固定的线程范围计算
    // 这里可以后续优化为动态计算
}

void LoopParallelismPass::CopyLoopBodyInstructions(Loop* loop, CFG* cfg, LLVMBlock& new_func_block, 
                                                  std::map<int, int>& reg_mapping) {
    // 复制循环体中的指令，但跳过循环控制指令
    for (auto block : loop->getBlocks()) {
        for (auto inst : block->Instruction_list) {
            // 跳过循环控制指令
            if (inst->GetOpcode() == BasicInstruction::BR_COND) {
                continue;
            }
            
            // 跳过循环条件比较指令
            if (inst->GetOpcode() == BasicInstruction::ICMP) {
                continue;
            }
            
            // 跳过PHI指令（在函数中不需要）
            if (inst->GetOpcode() == BasicInstruction::PHI) {
                continue;
            }
            
            // 克隆指令
            Instruction cloned_inst = CloneInstruction(inst, cfg, reg_mapping);
            if (cloned_inst) {
                new_func_block->Instruction_list.push_back(cloned_inst);
            }
        }
    }
}

Instruction LoopParallelismPass::CloneInstruction(Instruction inst, CFG* cfg, std::map<int, int>& reg_mapping) {
    // 使用BasicInstruction的Clone方法
    Instruction cloned_inst = inst->Clone();
    
    if (!cloned_inst) {
        return nullptr;
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
        int def_reg = inst->GetDefRegno();
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
    // parallel_loop_execute(func_name, start, end, num_threads)
    
    // 使用extractLoopParams获取循环参数
    auto loop_params = SE->extractLoopParams(loop, cfg);
    
    // 创建参数列表
    std::vector<std::pair<BasicInstruction::LLVMType, Operand>> params;
    
    // 函数指针参数
    auto func_ptr = GetNewGlobalOperand(func_name.c_str());
    params.push_back({BasicInstruction::LLVMType::PTR, func_ptr});
    params.push_back({BasicInstruction::LLVMType::I32, new ImmI32Operand(loop_params.start_val)});
    params.push_back({BasicInstruction::LLVMType::I32, new ImmI32Operand(loop_params.bound_val)});
    params.push_back({BasicInstruction::LLVMType::I32, new ImmI32Operand(4)});

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
    // void parallel_loop_execute(void* func_ptr, int start, int end, int num_threads);
    
    // 检查是否已经声明过
    static bool declared = false;
    if (declared) return;
    
    // 创建函数声明
    auto decl = new FunctionDefineInstruction(BasicInstruction::LLVMType::VOID, "parallel_loop_execute");
    decl->InsertFormal(BasicInstruction::LLVMType::PTR);  // func_ptr
    decl->InsertFormal(BasicInstruction::LLVMType::I32);  // start
    decl->InsertFormal(BasicInstruction::LLVMType::I32);  // end
    decl->InsertFormal(BasicInstruction::LLVMType::I32);  // num_threads
    
    // 添加到全局函数表
    llvmIR->FunctionNameTable["parallel_loop_execute"] = decl;
    
    declared = true;
    
    PARALLEL_DEBUG(std::cout << "添加运行时库函数声明" << std::endl);
}
