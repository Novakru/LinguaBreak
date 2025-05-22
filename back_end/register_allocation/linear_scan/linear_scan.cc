#include"linear_scan.h"
#include"../../inst_process/machine_instruction.h"

void FastLinearScan::VirtualRegisterRewrite() {
    for (auto func : unit->functions) {
        current_func = func;
        RewriteInFunc();
    }
}

void FastLinearScan::RewriteInFunc() {
    auto func = current_func;
    auto mcfg = func->getMachineCFG();
    mcfg->seqscan_open();
    while (mcfg->seqscan_hasNext()) {
        auto block = mcfg->seqscan_next()->Mblock;
        for (auto it = block->begin(); it != block->end(); ++it) {
            auto ins = *it;
            // 根据alloc_result将ins的虚拟寄存器重写为物理寄存器
            //TODO("VirtualRegisterRewrite");
        for(auto reg:ins->GetReadReg())//遍历当前指令中所有读寄存器
            {
                if(reg->is_virtual==false)//如果不是虚拟寄存器
                {
                    //检查分配结果中是否找不到该寄存器
                    Assert(alloc_result.find(func)==alloc_result.end()
                    ||alloc_result.find(func)->second.find(*reg)==alloc_result.find(func)->second.end());
                    continue;
                }
                //当前函数，当前寄存器对应的alloc_result
                auto result=alloc_result.find(func)->second.find(*reg)->second;
                if(result.in_mem==true){ERROR("shoudn't reach here");}//如果已经分配了栈空间，那么不再需要分配寄存器
                else{
                    reg->is_virtual=false;
                    reg->reg_no=result.phy_reg_no;
                }
            }
            for(auto reg:ins->GetWriteReg())//遍历当前指令的所有写寄存器
            {
                if(reg->is_virtual==false)
                {
                    Assert(alloc_result.find(func)==alloc_result.end()
                    ||alloc_result.find(func)->second.find(*reg)==alloc_result.find(func)->second.end());
                    continue;
                }
                auto result=alloc_result.find(func)->second.find(*reg)->second;
                if(result.in_mem==true){ERROR("shoudn't reach here");}
                else{
                    reg->is_virtual=false;//在内存中的为虚拟寄存器，不在内存中的为非虚拟
                    reg->reg_no=result.phy_reg_no;
                }
            }
        }
    }
   mcfg->seqscan_close();
}

bool IntervalsPrioCmp(LiveInterval a, LiveInterval b) { return a.begin()->begin > b.begin()->begin; }

bool FastLinearScan::DoAllocInCurrentFunc() {
    bool spilled = false;
    auto mfun = current_func;
    //MachineFunction* mfun;
    // std::cerr<<"FastLinearScan: "<<mfun->getFunctionName()<<"\n";
    phy_regs_tools->clear();
    for (auto interval : intervals) {
        Assert(interval.first == interval.second.getReg());
        if (interval.first.is_virtual) {
            // 需要分配的虚拟寄存器
            unalloc_queue.push(interval.second);
        } else {
            // Log("Pre Occupy Physical Reg %d",interval.first.reg_no);
            // 预先占用已经存在的物理寄存器
            phy_regs_tools->OccupyReg(interval.first.reg_no, interval.second);
        }
    }
    // TODO: 进行线性扫描寄存器分配, 为每个虚拟寄存器选择合适的物理寄存器或者将其溢出到合适的栈地址中
    // 该函数中只需正确设置alloc_result，并不需要实际生成溢出代码
    //TODO("LinearScan");

     while(!unalloc_queue.empty())
    {
        //A.获取待处理的活跃区间，及其对应的寄存器
        auto interval=unalloc_queue.top();
        unalloc_queue.pop();
        //B.尝试获取空闲物理寄存器（通过活跃区间）
        auto cur_vreg=interval.getReg();
    //     int phy_reg_id=phy_regs_tools->getIdleReg(interval);
    //     //unalloc_queue.pop();
    //     if(phy_reg_id>=0)
    //     {
    //         //C.使用物理寄存器的情况：占用该物理寄存器并记录占用信息
    //         phy_regs_tools->OccupyReg(phy_reg_id,interval);//占用寄存器
    //         AllocPhyReg(mfun,cur_vreg,phy_reg_id);//再分配结果中记录
    //         continue;
    //     }
    //     //D.溢出的情况：获取可用内存并占用，在栈上分配这部分内存
    //     int mem=phy_regs_tools->getIdleMem(interval);//获取可用的内存位置
    //     phy_regs_tools->OccupyMem(mem,interval);//占用内存
    //     AllocStack(mfun,cur_vreg,mem);//在栈上分配
    //     spilled=true;

    //     //E.计算溢出权重,选择冲突区间里溢出权重小的溢出
    //     double spill_weight=CalculateSpillWeight(interval);
    //     auto spill_interval=interval;
    //     for(auto other:phy_regs_tools->getConflictIntervals(interval))
    //     {
    //         double other_weight=CalculateSpillWeight(other);
    //         if(spill_weight>other_weight && other.getReg().is_virtual)
    //         {
    //             spill_weight=other_weight;
    //             spill_interval=other;
    //         }
    //     }
    //     //如果需要交换
    //     auto spill_reg=spill_interval.getReg();
    //     if(!(cur_vreg==spill_reg))
    //     {
    //         //执行寄存器和内存的交换
    //         phy_regs_tools->swapRegspill(getAllocResultInReg(mfun, spill_interval.getReg()), spill_interval, mem,
    //                                 cur_vreg.getDataWidth(), interval);
    //         swapAllocResult(mfun, interval.getReg(), spill_interval.getReg());
    //         int spill_mem = phy_regs_tools->getIdleMem(spill_interval);
    //         phy_regs_tools->OccupyMem(spill_mem,  spill_interval);
    //         AllocStack(mfun, spill_interval.getReg(), spill_mem);
    //     }
    // }
    // // 返回是否发生溢出
    // return spilled;
    int phy_reg_id = phy_regs_tools->getIdleReg(interval);
        if (phy_reg_id >= 0) {
            phy_regs_tools->OccupyReg(phy_reg_id, interval);
            AllocPhyReg(mfun, cur_vreg, phy_reg_id);
        } else {
            spilled = true;

            int mem = phy_regs_tools->getIdleMem(interval);
            phy_regs_tools->OccupyMem(mem, interval);
            // volatile int mem_ = mem;
            // volatile int mem__ = mem_+current_func->GetStackOffset();
            AllocStack(mfun, cur_vreg, mem);

            double spill_weight = CalculateSpillWeight(interval);
            auto spill_interval = interval;
            for (auto other : phy_regs_tools->getConflictIntervals(interval)) {
                double other_weight = CalculateSpillWeight(other);
                if (spill_weight > other_weight && other.getReg().is_virtual) {
                    spill_weight = other_weight;
                    spill_interval = other;
                }
            }
            auto spill_reg=spill_interval.getReg();
            if (!(cur_vreg==spill_reg)) {
                phy_regs_tools->swapRegspill(getAllocResultInReg(mfun, spill_interval.getReg()), spill_interval, mem,
                                       cur_vreg.getDataWidth(), interval);
                swapAllocResult(mfun, interval.getReg(), spill_interval.getReg());
                // alloc_result[mfun].erase(spill_interval.getReg());
                // unalloc_queue.push(spill_interval);
                int spill_mem = phy_regs_tools->getIdleMem(spill_interval);
                phy_regs_tools->OccupyMem(spill_mem, spill_interval);
                AllocStack(mfun, spill_interval.getReg(), spill_mem);
            }
        }
    }
    return spilled;
}

// 计算溢出权重
double FastLinearScan::CalculateSpillWeight(LiveInterval interval) {
    return (double)interval.getReferenceCount() / interval.getIntervalLength();
}

void FastLinearScan::Execute() {
    // 你需要保证此时不存在phi指令
    for (auto func : unit->functions) {
        not_allocated_funcs.push(func);
    }
    //1.逐一处理函数
    while (!not_allocated_funcs.empty()) {
        current_func = not_allocated_funcs.front();
        numbertoins.clear();
        //2.对每条指令进行编号
        InstructionNumber(unit, numbertoins).ExecuteInFunc(current_func);

        //3.清除之前分配的结果
        alloc_result[current_func].clear();
        not_allocated_funcs.pop();

        //4.计算活跃区间
        UpdateIntervalsInCurrentFunc();

        if (DoAllocInCurrentFunc()) {    // 5.尝试进行分配
            // 6.如果发生溢出，插入spill指令后将所有物理寄存器退回到虚拟寄存器，重新分配
            SpillCodeGen(current_func, &alloc_result[current_func]);    // 生成溢出代码
            current_func->AddStackSize(phy_regs_tools->getSpillSize());                 // 调整栈的大小
            not_allocated_funcs.push(current_func);                               // 重新分配直到不再spill
        }
    }
    // 重写虚拟寄存器，全部转换为物理寄存器
    VirtualRegisterRewrite();
}

void InstructionNumber::Execute() {
    for (auto func : unit->functions) {
        ExecuteInFunc(func);
    }
}

void InstructionNumber::ExecuteInFunc(MachineFunction *func) {
    // 对每个指令进行编号(用于计算活跃区间)
    int count_begin = 0;
    //current_func = func;
    // Note: If Change to DFS Iterator, FastLinearScan::UpdateIntervalsInCurrentFunc() Also need to be
    // changed
    auto mcfg = func->getMachineCFG();
    mcfg->bfs_open();
    while (mcfg->bfs_hasNext()) {
        auto mcfg_node = mcfg->bfs_next();
        auto mblock = mcfg_node->Mblock;
        // Update instruction number
        // 每个基本块开头会占据一个编号
        this->numbertoins[count_begin] = InstructionNumberEntry(nullptr, true);
        count_begin++;
        for (auto ins : *mblock) {
            this->numbertoins[count_begin] = InstructionNumberEntry(ins, false);
            ins->setNumber(count_begin++);
        }
    }
    //mcfg->bfs_close();
}

// 更新当前函数中所有寄存器的活跃区间（Live Interval）
void FastLinearScan::UpdateIntervalsInCurrentFunc() {
    //1.清空之前的活跃区间数据（以函数为单位，可能处理过其他函数）
    intervals.clear(); 
    //2.获取当前函数，当前函数对应的控制流图
    auto mfun = current_func; 
    auto mcfg = mfun->getMachineCFG(); 
    //3.对当前函数进行活跃分析
    Liveness liveness(mfun); 

    // 4.准备反向遍历控制流图（逆向数据流分析，从函数出口遍历到函数入口）
    mcfg->bfs_close(); 
    mcfg->reverse_open(); 
    
    // 记录寄存器最后一次定义和使用的位置（指令编号）
    std::map<Register, int> last_def, last_use;

    // 开始反向遍历所有基本块（逆控制流方向）
    while (mcfg->reverse_hasNext()) {
        auto mcfg_node = mcfg->reverse_next(); // 获取下一个基本块节点
        auto mblock = mcfg_node->Mblock; // 当前基本块对象
        auto cur_id = mblock->getLabelId(); // 基本块标签ID

        // 处理基本块的OUT集合（出口处活跃的寄存器）
        for (auto reg : liveness.GetOUT(cur_id)) {
            // 若该寄存器尚未记录，创建新的活跃区间
            if (intervals.find(reg) == intervals.end()) {
                intervals[reg] = LiveInterval(reg); // 初始化区间对象
            }
            
            // 扩展或新增区间段
            if (last_use.find(reg) == last_use.end()) {
                // 情况1：无前驱使用记录，创建新区间段（从块入口到出口）
                intervals[reg].PushFront(
                    mblock->getBlockInNumber(), // 基本块起始指令编号
                    mblock->getBlockOutNumber() // 基本块结束指令编号
                );
            } else {
                // 情况2：已有前驱使用记录，合并区间（仍会创建新区间段）
                intervals[reg].PushFront(
                    mblock->getBlockInNumber(),
                    mblock->getBlockOutNumber()
                );
            }
            last_use[reg] = mblock->getBlockOutNumber(); // 记录最后一次使用位置
        }

        // 反向遍历当前基本块内的指令（从最后一条到第一条）
        for (auto reverse_it = mblock->ReverseBegin(); 
             reverse_it != mblock->ReverseEnd(); 
             ++reverse_it) 
        {
            auto ins = *reverse_it; // 获取当前指令对象

            // 处理指令的写寄存器（定义操作）
            for (auto reg : ins->GetWriteReg()) {
                // 更新最后一次定义位置
                last_def[*reg] = ins->getNumber(); // 记录指令编号

                // 初始化未记录的寄存器区间
                if (intervals.find(*reg) == intervals.end()) {
                    intervals[*reg] = LiveInterval(*reg);
                }

                // 检查是否存在未处理的最后一次使用
                if (last_use.find(*reg) != last_use.end()) {
                    // 情况1：存在后续使用，分割当前区间
                    last_use.erase(*reg); // 清除使用记录
                    intervals[*reg].SetMostBegin(ins->getNumber()); // 设置区间起点为定义位置
                } else {
                    // 情况2：无后续使用，创建单指令区间（仅定义点）
                    intervals[*reg].PushFront(
                        ins->getNumber(), // 起始
                        ins->getNumber()   // 结束（定义即结束）
                    );
                }
                intervals[*reg].IncreaseReferenceCount(1); // 增加引用计数（用于溢出优先级）
            }

            // 处理指令的读寄存器（使用操作）
            for (auto reg : ins->GetReadReg()) {
                // 初始化未记录的寄存器区间
                if (intervals.find(*reg) == intervals.end()) {
                    intervals[*reg] = LiveInterval(*reg);
                }

                // 检查是否存在未处理的最后一次使用
                if (last_use.find(*reg) == last_use.end()) {
                    // 情况：无后续使用，创建从块入口到当前指令的区间
                    intervals[*reg].PushFront(
                        mblock->getBlockInNumber(), // 块起始
                        ins->getNumber()            // 当前指令
                    );
                }
                last_use[*reg] = ins->getNumber(); // 更新最后一次使用位置
                intervals[*reg].IncreaseReferenceCount(1); // 增加引用计数
            }
        } // 结束指令遍历

        // 清空临时记录，避免跨基本块污染
        last_use.clear();
        last_def.clear();
    } // 结束基本块遍历

    // 调试输出：打印合并前的活跃区间信息
    // std::cerr << "Check Intervals " << mfun->getFunctionName().c_str() 
    //           << " Before Coalesce" << std::endl;
    // for (auto interval_pair : intervals) {
    //     auto reg = interval_pair.first;
    //     auto interval = interval_pair.second;
    //     std::cerr << reg.is_virtual << " " << reg.reg_no << " ";
    //     for (auto seg : interval) {
    //         std::cerr << "[" << seg.begin << "," << seg.end << ") "; // 打印区间段
    //     }
    //     std::cerr << "Ref: " << interval.getReferenceCount(); // 引用计数
    //     std::cerr << "\n";
    // }
    // std::cerr << "\n";
}

// void FastLinearScan::SpillCodeGen(MachineFunction *function, std::map<Register, AllocResult> *alloc_result) {
//     //this->function = function;
//     //this->alloc_result = alloc_result;
//     std::cout<<"SpillCodeGen\n";
//     auto mcfg = function->getMachineCFG();
//     mcfg->seqscan_open();
//     while (mcfg->seqscan_hasNext()) {
//         cur_block = mcfg->seqscan_next()->Mblock;
//         for (auto it = cur_block->begin(); it != cur_block->end(); ++it) {
//             auto ins = *it;
//             // 根据alloc_result对ins溢出的寄存器生成溢出代码
//             // 在溢出虚拟寄存器的read前插入load，在write后插入store
//             // 注意load的结果寄存器是虚拟寄存器, 因为我们接下来要重新分配直到不再溢出
//             //TODO("FastLinearScan");
//         for(auto reg:ins->GetReadReg())
//             {
//                 if(reg->is_virtual==false){continue;}
//                 auto result=alloc_result->find(*reg)->second;
//                 if(result.in_mem==true)//如果虚拟寄存器，在内存中，生成溢出代码
//                 {
//                     *reg=GenerateReadCode(it,result.stack_offset*4,reg->type);
//                 }
//             }
//             for(auto reg:ins->GetReadReg())
//             {
//                 if(reg->is_virtual==false){continue;}
//                 auto result=alloc_result->find(*reg)->second;
//                 if(result.in_mem==true)
//                 {
//                     *reg=GenerateWriteCode(it,result.stack_offset*4,reg->type);
//                 }
//             }
//         }
//     }
//     //mcfg->seqscan_close();
// }

// // 生成从栈中读取溢出寄存器的指令
// Register FastLinearScan::GenerateReadCode(std::list<MachineBaseInstruction *>::iterator &it, int raw_stk_offset,
//                                           MachineDataType type) {
//     // it为指向发生寄存器溢出的指令的迭代器
//     // raw_stk_offset为该寄存器溢出到栈中的位置的偏移，相对于什么位置的偏移可以在调用时自行决定
//     //std::cout<<"GenerateReadCode\n";
//     auto read_mid_reg = current_func->GetNewRegister(type.data_type, type.data_length);
//     //TODO("GenerateReadSpillCode");
//     int offset=raw_stk_offset+current_func->GetParaSize();//函数栈偏移量+当前偏移量
//    if(offset<=2047 &&offset>=-2048)
//    {
//     if(type==INT64)
//     {
//         cur_block->insert(it,rvconstructor->ConstructIImm(RISCV_LD,read_mid_reg,GetPhysicalReg(RISCV_sp),offset));
//     }
//     else if(type==FLOAT64)
//     {
//         cur_block->insert(it,rvconstructor->ConstructIImm(RISCV_FLD,read_mid_reg,GetPhysicalReg(RISCV_sp),offset));
//     }
//    }
//    else//如果偏移量超出可操作范围
//    {
//     //std::cout<<"偏移量超出可操作范围\n";
//     // auto imm_reg=current_func->GetNewRegister(INT64.data_type,INT64.data_length);
//     // auto offset_mid_reg=current_func->GetNewRegister(INT64.data_type,INT64.data_length);
//     // auto addiw_instr1 = rvconstructor->ConstructUImm(RISCV_LUI, imm_reg,  (offset + (1 << 11)) >> 12);//修改//////////////////////////
//     // cur_block->insert(it,addiw_instr1);
//     // auto addiw_instr2 = rvconstructor->ConstructIImm(RISCV_ORI, imm_reg, imm_reg,offset& 0xfff);//修改//////////////////////////
//     // cur_block->insert(it,addiw_instr2);
//     // //cur_block->insert(it,rvconstructor->ConstructUImm(RISCV_LI,imm_reg,offset));//将偏移量加载到imm_reg寄存器
//     // cur_block->insert(it,rvconstructor->ConstructR(RISCV_ADD,offset_mid_reg,GetPhysicalReg(RISCV_sp),imm_reg));//计算基址加偏移到offset_mid_reg
//     // if (type == INT64) { // 如果数据类型是 64 位整数
//     //     cur_block->insert(it, rvconstructor->ConstructIImm(RISCV_LD, read_mid_reg, offset_mid_reg, 0)); // 从计算的地址中加载数据
//     // } else if (type == FLOAT64) { // 如果数据类型是 64 位浮点数
//     //     cur_block->insert(it, rvconstructor->ConstructIImm(RISCV_FLD, read_mid_reg, offset_mid_reg, 0)); // 从计算的地址中加载浮点数据
//     // }
//     auto imm_reg = current_func->GetNewRegister(INT64.data_type, INT64.data_length);
//     auto offset_mid_reg = current_func->GetNewRegister(INT64.data_type, INT64.data_length);
//     cur_block->insert(it, rvconstructor->ConstructUImm(RISCV_LI, imm_reg, offset));
//     cur_block->insert(it, rvconstructor->ConstructR(RISCV_ADD, offset_mid_reg, GetPhysicalReg(RISCV_sp), imm_reg));
//     if (type == INT64) {
//         cur_block->insert(it, rvconstructor->ConstructIImm(RISCV_LD, read_mid_reg, offset_mid_reg, 0));
//     } else if (type == FLOAT64) {
//         cur_block->insert(it, rvconstructor->ConstructIImm(RISCV_FLD, read_mid_reg, offset_mid_reg, 0));
//     }
//    }
//     std::cout<< "read_mid_reg=" << read_mid_reg.reg_no<<"\n";
//     return read_mid_reg;
// }

// // 生成将溢出寄存器写入栈的指令
// Register FastLinearScan::GenerateWriteCode(std::list<MachineBaseInstruction *>::iterator &it, int raw_stk_offset,
//                                            MachineDataType type) {
//     //std::cout<<"GenerateWriteCode\n";
//     auto write_mid_reg = current_func->GetNewRegister(type.data_type, type.data_length);
//     //TODO("GenerateWriteSpillCode");
//     int offset=raw_stk_offset+current_func->GetStackOffset();
//     ++it;//STORE指令需要加载在写指令之后；而LOAD指令需要加载在读指令之前
//     if(offset<=2047 && offset>=-2048)
//     {
//         if(type==INT64)
//         {
//             cur_block->insert(it,rvconstructor->ConstructSImm(RISCV_SD,write_mid_reg,GetPhysicalReg(RISCV_sp),offset));
//         }
//         else if(type==FLOAT64)
//         {
//             cur_block->insert(it,rvconstructor->ConstructSImm(RISCV_FSD,write_mid_reg,GetPhysicalReg(RISCV_sp),offset));
//         }
//     }
//     else
//     {
//         //std::cout<<"偏移量超出可操作范围\n";
//         // auto imm_reg=current_func->GetNewRegister(INT64.data_type,INT64.data_length);
//         // auto offset_mid_reg=current_func->GetNewRegister(INT64.data_type,INT64.data_length);
//         // auto addiw_instr1 = rvconstructor->ConstructUImm(RISCV_LUI, imm_reg,  (offset + (1 << 11)) >> 12);//修改//////////////////////////
//         // cur_block->insert(it,addiw_instr1);
//         // auto addiw_instr2 = rvconstructor->ConstructIImm(RISCV_ORI, imm_reg, imm_reg,offset& 0xfff);//修改//////////////////////////
//         // cur_block->insert(it,addiw_instr2);
//         // //cur_block->insert(it,rvconstructor->ConstructUImm(RISCV_LI,imm_reg,offset));
//         // cur_block->insert(it,rvconstructor->ConstructR(RISCV_ADD,offset_mid_reg,GetPhysicalReg(RISCV_sp),imm_reg));
//         //  if (type == INT64) { // 如果数据类型是 64 位整数
//         //     cur_block->insert(it, rvconstructor->ConstructSImm(RISCV_SD, write_mid_reg, offset_mid_reg, 0)); // 从中间寄存器存储到计算的地址
//         // } else if (type == FLOAT64) { // 如果数据类型是 64 位浮点数
//         //     cur_block->insert(it, rvconstructor->ConstructSImm(RISCV_FSD, write_mid_reg, offset_mid_reg, 0)); // 从中间寄存器存储浮点数据到计算的地址
//         // }
//         auto imm_reg = current_func->GetNewRegister(INT64.data_type, INT64.data_length);
//         auto offset_mid_reg = current_func->GetNewRegister(INT64.data_type, INT64.data_length);
//         cur_block->insert(it, rvconstructor->ConstructUImm(RISCV_LI, imm_reg, offset));
//         cur_block->insert(it, rvconstructor->ConstructR(RISCV_ADD, offset_mid_reg, GetPhysicalReg(RISCV_sp), imm_reg));
//         if (type == INT64) {
//             cur_block->insert(it, rvconstructor->ConstructSImm(RISCV_SD, write_mid_reg, offset_mid_reg, 0));
//         } else if (type == FLOAT64) {
//             cur_block->insert(it, rvconstructor->ConstructSImm(RISCV_FSD, write_mid_reg, offset_mid_reg, 0));
//         }
//     }
//     --it;
//     std::cout<<"write_mid_reg=" << write_mid_reg.reg_no<<"\n";
//     return write_mid_reg;
// }
const int MAX_TEMP_REGS=11;
void FastLinearScan::SpillCodeGen(MachineFunction *function, std::map<Register, AllocResult> *alloc_result) {
    //std::cout<<"Optimized SpillCodeGen\n";
    auto mcfg = function->getMachineCFG();
    mcfg->seqscan_open();
    
    // 跟踪当前使用的临时寄存器数量
    int temp_reg_count = 0;
    // 用于存储频繁访问的栈值的临时寄存器
    std::map<int, Register> temp_reg_cache;
    
    while (mcfg->seqscan_hasNext()) {
        cur_block = mcfg->seqscan_next()->Mblock;
        for (auto it = cur_block->begin(); it != cur_block->end(); ++it) {
            auto ins = *it;
            
            // 合并读取和写入的处理逻辑
            std::set<Register> processed_regs;
            
            // 处理读取寄存器
            for(auto reg:ins->GetReadReg()) {
                if(reg->is_virtual==false || processed_regs.count(*reg)) continue;
                
                auto result=alloc_result->find(*reg)->second;
                if(result.in_mem==true) {
                    // 检查是否可以复用临时寄存器
                    if(temp_reg_cache.find(result.stack_offset) != temp_reg_cache.end()) {
                        *reg = temp_reg_cache[result.stack_offset];
                        processed_regs.insert(*reg);
                        continue;
                    }
                    
                    // 生成读取代码
                    *reg=GenerateReadCode(it,result.stack_offset*4,reg->type);
                    processed_regs.insert(*reg);
                    
                    // 缓存临时寄存器，避免重复读取
                    if(temp_reg_count < MAX_TEMP_REGS) {
                        temp_reg_cache[result.stack_offset] = *reg;
                        temp_reg_count++;
                    }
                }
            }
            
            // 处理写入寄存器
            for(auto reg:ins->GetWriteReg()) {
                if(reg->is_virtual==false || processed_regs.count(*reg)) continue;
                
                auto result=alloc_result->find(*reg)->second;
                if(result.in_mem==true) {
                    // 生成写入代码
                    *reg=GenerateWriteCode(it,result.stack_offset*4,reg->type);
                    processed_regs.insert(*reg);
                    
                    // 写入后从缓存中移除
                    temp_reg_cache.erase(result.stack_offset);
                    temp_reg_count = std::max(0, temp_reg_count-1);
                }
            }
        }
        
        // 块结束时清空临时寄存器缓存
        temp_reg_cache.clear();
        temp_reg_count = 0;
    }
}
// 显示alloc_result的全部内容（allocres）
void FastLinearScan::ShowAllAllocResult() {
	std::cout << "======== alloc_result 全部内容 ========" << std::endl;
	for (auto &func_pair : alloc_result) {
		auto *mfun = func_pair.first;
		std::cout << "函数: " << mfun->getFunctionName() << std::endl;
		for (auto &reg_pair : func_pair.second) {
			auto vreg = reg_pair.first;
			auto res = reg_pair.second;
			std::cout << "  虚拟寄存器: " << vreg.reg_no << " -> ";
			if (res.in_mem) {
				std::cout << "栈偏移: " << res.stack_offset << std::endl;
			} else {
				std::cout << "物理寄存器: " << res.phy_reg_no << std::endl;
			}
		}
	}
	std::cout << "======================================" << std::endl;
}
// Register FastLinearScan::GenerateReadCode(std::list<MachineBaseInstruction *>::iterator &it, int raw_stk_offset,
//                                           MachineDataType type) {
//     auto read_mid_reg = current_func->GetNewRegister(type.data_type, type.data_length);
//     int offset=raw_stk_offset+current_func->GetParaSize();
    
//     // 优化大偏移量处理
//     if(offset > 2047 || offset < -2048) {
//         // 使用LUI+ADD策略，避免ORI限制
//         auto imm_reg = current_func->GetNewRegister(INT64.data_type, INT64.data_length);
//         auto offset_mid_reg = current_func->GetNewRegister(INT64.data_type, INT64.data_length);
        
//         // 计算高20位和低12位
//         int high20 = (offset + 0x800) >> 12;
//         int low12 = offset & 0xFFF;
        
//         cur_block->insert(it, rvconstructor->ConstructUImm(RISCV_LUI, imm_reg, high20));
        
//         // 根据低12位的值选择最佳指令
//         if(low12 != 0) {
//             if(low12 < 2048) {
//                 cur_block->insert(it, rvconstructor->ConstructIImm(RISCV_ADDI, imm_reg, imm_reg, low12));
//             } else {
//                 // 处理负数偏移
//                 cur_block->insert(it, rvconstructor->ConstructIImm(RISCV_ADDI, imm_reg, imm_reg, low12 - 4096));
//             }
//         }
        
//         //cur_block->insert(it, rvconstructor->ConstructR(RISCV_ADD, offset_mid_reg, GetPhysicalReg(RISCV_sp), imm_reg));
        
//         if (type == INT64) {
//             cur_block->insert(it, rvconstructor->ConstructR(RISCV_ADD, offset_mid_reg, GetPhysicalReg(RISCV_sp), imm_reg));
//             cur_block->insert(it, rvconstructor->ConstructIImm(RISCV_LD, read_mid_reg, offset_mid_reg, 0));
//         } else if (type == FLOAT64) {
//             cur_block->insert(it, rvconstructor->ConstructR(RISCV_ADD, offset_mid_reg, GetPhysicalReg(RISCV_fp), imm_reg));
//             cur_block->insert(it, rvconstructor->ConstructIImm(RISCV_FLD, read_mid_reg, offset_mid_reg, 0));
//         }
//     } else {
//         // 小偏移量直接使用基址+偏移
//         if(type==INT64) {
//             cur_block->insert(it,rvconstructor->ConstructIImm(RISCV_LD,read_mid_reg,GetPhysicalReg(RISCV_sp),offset));
//         } else if(type==FLOAT64) {
//             cur_block->insert(it,rvconstructor->ConstructIImm(RISCV_FLD,read_mid_reg,GetPhysicalReg(RISCV_fp),offset));
//         }
//     }
    
//     return read_mid_reg;
// }

// Register FastLinearScan::GenerateWriteCode(std::list<MachineBaseInstruction *>::iterator &it, int raw_stk_offset,
//                                            MachineDataType type) {
//     auto write_mid_reg = current_func->GetNewRegister(type.data_type, type.data_length);
//     int offset=raw_stk_offset+current_func->GetStackOffset();
    
//     ++it; // STORE指令需要加载在写指令之后
    
//     // 优化大偏移量处理
//     if(offset > 2047 || offset < -2048) {
//         auto imm_reg = current_func->GetNewRegister(INT64.data_type, INT64.data_length);
//         auto offset_mid_reg = current_func->GetNewRegister(INT64.data_type, INT64.data_length);
        
//         // 计算高20位和低12位
//         int high20 = (offset + 0x800) >> 12;
//         int low12 = offset & 0xFFF;
        
//         cur_block->insert(it, rvconstructor->ConstructUImm(RISCV_LUI, imm_reg, high20));
        
//         // 根据低12位的值选择最佳指令
//         if(low12 != 0) {
//             if(low12 < 2048) {
//                 cur_block->insert(it, rvconstructor->ConstructIImm(RISCV_ADDI, imm_reg, imm_reg, low12));
//             } else {
//                 // 处理负数偏移
//                 cur_block->insert(it, rvconstructor->ConstructIImm(RISCV_ADDI, imm_reg, imm_reg, low12 - 4096));
//             }
//         }
        
//        // cur_block->insert(it, rvconstructor->ConstructR(RISCV_ADD, offset_mid_reg, GetPhysicalReg(RISCV_sp), imm_reg));
        
//         if (type == INT64) {
//             cur_block->insert(it, rvconstructor->ConstructR(RISCV_ADD, offset_mid_reg, GetPhysicalReg(RISCV_sp), imm_reg));
//             cur_block->insert(it, rvconstructor->ConstructSImm(RISCV_SD, write_mid_reg, offset_mid_reg, 0));
//         } else if (type == FLOAT64) {
//             cur_block->insert(it, rvconstructor->ConstructR(RISCV_ADD, offset_mid_reg, GetPhysicalReg(RISCV_fp), imm_reg));
//             cur_block->insert(it, rvconstructor->ConstructSImm(RISCV_FSD, write_mid_reg, offset_mid_reg, 0));
//         }
//     } else {
//         if(type==INT64) {
//             cur_block->insert(it,rvconstructor->ConstructSImm(RISCV_SD,write_mid_reg,GetPhysicalReg(RISCV_sp),offset));
//         } else if(type==FLOAT64) {
//             cur_block->insert(it,rvconstructor->ConstructSImm(RISCV_FSD,write_mid_reg,GetPhysicalReg(RISCV_fp),offset));
//         }
//     }
    
//     --it;
//     return write_mid_reg;
// }
//使用助教的两个函数后，正确数从84变成了85
Register FastLinearScan::GenerateReadCode(std::list<MachineBaseInstruction *>::iterator &it, int raw_stk_offset,
                                          MachineDataType type) {
    auto read_mid_reg = current_func->GetNewRegister(type.data_type, type.data_length);
    // missing lowerimm
    // missing stack size adjust
    int offset = raw_stk_offset + current_func->GetStackOffset();
    // cur_block->insert(it, rvconstructor->ConstructComment("Read Spill\n"));
    if (offset <= 2047 && offset >= -2048) {
        if (type == INT64) {
            cur_block->insert(it, rvconstructor->ConstructIImm(RISCV_LD, read_mid_reg, GetPhysicalReg(RISCV_sp),
                                                               offset));    // insert load
        } else if (type == FLOAT64) {
            cur_block->insert(it,
                              rvconstructor->ConstructIImm(RISCV_FLD, read_mid_reg, GetPhysicalReg(RISCV_sp), offset));
        }
    } else {
        auto imm_reg = current_func->GetNewRegister(INT64.data_type, INT64.data_length);
        auto offset_mid_reg = current_func->GetNewRegister(INT64.data_type, INT64.data_length);
        cur_block->insert(it, rvconstructor->ConstructUImm(RISCV_LI, imm_reg, offset));
        cur_block->insert(it, rvconstructor->ConstructR(RISCV_ADD, offset_mid_reg, GetPhysicalReg(RISCV_sp), imm_reg));
        if (type == INT64) {
            cur_block->insert(it, rvconstructor->ConstructIImm(RISCV_LD, read_mid_reg, offset_mid_reg, 0));
        } else if (type == FLOAT64) {
            cur_block->insert(it, rvconstructor->ConstructIImm(RISCV_FLD, read_mid_reg, offset_mid_reg, 0));
        }
    }
    return read_mid_reg;
}

Register FastLinearScan::GenerateWriteCode(std::list<MachineBaseInstruction *>::iterator &it, int raw_stk_offset,
                                           MachineDataType type) {
    auto write_mid_reg = current_func->GetNewRegister(type.data_type, type.data_length);
    int offset = raw_stk_offset + current_func->GetStackOffset();
    ++it;
    // cur_block->insert(it, rvconstructor->ConstructComment("Write Spill\n"));
    if (offset <= 2047 && offset >= -2048) {
        if (type == INT64) {
            cur_block->insert(it, rvconstructor->ConstructSImm(RISCV_SD, write_mid_reg, GetPhysicalReg(RISCV_sp),
                                                               offset));    // insert store
        } else if (type == FLOAT64) {
            cur_block->insert(it, rvconstructor->ConstructSImm(RISCV_FSD, write_mid_reg, GetPhysicalReg(RISCV_sp),
                                                               offset));    // insert store
        }
    } else {
        auto imm_reg = current_func->GetNewRegister(INT64.data_type, INT64.data_length);
        auto offset_mid_reg = current_func->GetNewRegister(INT64.data_type, INT64.data_length);
        cur_block->insert(it, rvconstructor->ConstructUImm(RISCV_LI, imm_reg, offset));
        cur_block->insert(it, rvconstructor->ConstructR(RISCV_ADD, offset_mid_reg, GetPhysicalReg(RISCV_sp), imm_reg));
        if (type == INT64) {
            cur_block->insert(it, rvconstructor->ConstructSImm(RISCV_SD, write_mid_reg, offset_mid_reg, 0));
        } else if (type == FLOAT64) {
            cur_block->insert(it, rvconstructor->ConstructSImm(RISCV_FSD, write_mid_reg, offset_mid_reg, 0));
        }
    }
    --it;
    return write_mid_reg;
}