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

    // 返回是否发生溢出
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
    while (!not_allocated_funcs.empty()) {
        current_func = not_allocated_funcs.front();
        numbertoins.clear();
        // 对每条指令进行编号
        InstructionNumber(unit, numbertoins).ExecuteInFunc(current_func);

        // 需要清除之前分配的结果
        alloc_result[current_func].clear();
        not_allocated_funcs.pop();

        // 计算活跃区间
        UpdateIntervalsInCurrentFunc();

        if (DoAllocInCurrentFunc()) {    // 尝试进行分配
            // 如果发生溢出，插入spill指令后将所有物理寄存器退回到虚拟寄存器，重新分配
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
    mcfg->bfs_close();
}

void FastLinearScan::UpdateIntervalsInCurrentFunc() {
    intervals.clear();
    auto mfun = current_func;
    auto mcfg = mfun->getMachineCFG();

    Liveness liveness(mfun);

    MachineCFG::MockIterator mock_it(mcfg);
    mcfg->reverse_open(&mock_it);

    std::map<Register, int> last_def, last_use;

    while (mcfg->reverse_hasNext()) {
        auto mcfg_node = mcfg->reverse_next();
        auto mblock = mcfg_node->Mblock;
        auto cur_id = mcfg_node->Mblock->getLabelId();
        // For pseudo code see https://www.cnblogs.com/AANA/p/16311477.html
        for (auto reg : liveness.GetOUT(cur_id)) {
            if (intervals.find(reg) == intervals.end()) {
                intervals[reg] = LiveInterval(reg);
            }
            // Extend or add new Range
            if (last_use.find(reg) == last_use.end()) {
                // No previous Use, New Range
                intervals[reg].PushFront(mblock->getBlockInNumber(), mblock->getBlockOutNumber());
            } else {
                // Have previous Use, No Extend Range
                // intervals[reg].SetMostBegin(mblock->getBlockInNumber());
                intervals[reg].PushFront(mblock->getBlockInNumber(), mblock->getBlockOutNumber());
            }
            last_use[reg] = mblock->getBlockOutNumber();
        }
        for (auto reverse_it = mcfg_node->Mblock->ReverseBegin(); reverse_it != mcfg_node->Mblock->ReverseEnd();
             ++reverse_it) {
            auto ins = *reverse_it;
            for (auto reg : ins->GetWriteReg()) {
                // Update last_def of reg
                last_def[*reg] = ins->getNumber();

                if (intervals.find(*reg) == intervals.end()) {
                    intervals[*reg] = LiveInterval(*reg);
                }

                // Have Last Use, Cut Range
                if (last_use.find(*reg) != last_use.end()) {
                    last_use.erase(*reg);
                    intervals[*reg].SetMostBegin(ins->getNumber());
                } else {
                    // No Last Use, New Range
                    intervals[*reg].PushFront(ins->getNumber(), ins->getNumber());
                }
                intervals[*reg].IncreaseReferenceCount(1);
            }
            for (auto reg : ins->GetReadReg()) {
                // Update last_use of reg
                if (intervals.find(*reg) == intervals.end()) {
                    intervals[*reg] = LiveInterval(*reg);
                }

                if (last_use.find(*reg) != last_use.end() /*|| (last_def[*reg] == last_use[*reg])*/) {
                } else {
                    // No Last Use, New Range
                    intervals[*reg].PushFront(mblock->getBlockInNumber(), ins->getNumber());
                }
                last_use[*reg] = ins->getNumber();

                intervals[*reg].IncreaseReferenceCount(1);
            }
        }
        last_use.clear();
        last_def.clear();
    }
    mcfg->reverse_close();
    mcfg->bfs_close();
    // 你可以在这里输出intervals的值来获得活跃变量分析的结果
    // 观察结果可能对你寄存器分配算法的编写有一定帮助
}


void FastLinearScan::SpillCodeGen(MachineFunction *function, std::map<Register, AllocResult> *alloc_result) {
    //this->function = function;
    //this->alloc_result = alloc_result;
    auto mcfg = function->getMachineCFG();
    mcfg->seqscan_open();
    while (mcfg->seqscan_hasNext()) {
        cur_block = mcfg->seqscan_next()->Mblock;
        for (auto it = cur_block->begin(); it != cur_block->end(); ++it) {
            auto ins = *it;
            // 根据alloc_result对ins溢出的寄存器生成溢出代码
            // 在溢出虚拟寄存器的read前插入load，在write后插入store
            // 注意load的结果寄存器是虚拟寄存器, 因为我们接下来要重新分配直到不再溢出
            //TODO("FastLinearScan");
        }
    }
    mcfg->seqscan_close();
}

// 生成从栈中读取溢出寄存器的指令
Register FastLinearScan::GenerateReadCode(std::list<MachineBaseInstruction *>::iterator &it, int raw_stk_offset,
                                          MachineDataType type) {
    // it为指向发生寄存器溢出的指令的迭代器
    // raw_stk_offset为该寄存器溢出到栈中的位置的偏移，相对于什么位置的偏移可以在调用时自行决定
    auto read_mid_reg = current_func->GetNewRegister(type.data_type, type.data_length);
    //TODO("GenerateReadSpillCode");
    return read_mid_reg;
}

// 生成将溢出寄存器写入栈的指令
Register FastLinearScan::GenerateWriteCode(std::list<MachineBaseInstruction *>::iterator &it, int raw_stk_offset,
                                           MachineDataType type) {
    auto write_mid_reg = current_func->GetNewRegister(type.data_type, type.data_length);
    //TODO("GenerateWriteSpillCode");
    return write_mid_reg;
}
