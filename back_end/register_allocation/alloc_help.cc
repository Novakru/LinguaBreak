#include"alloc_help.h"
// 为了实现方便，这里直接使用set进行活跃变量分析，如果你不满意，可以自行更换更高效的数据结构(例如bitset)
template <class T> std::set<T> SetIntersect(const std::set<T> &a, const std::set<T> &b) {
    std::set<T> ret;
    for (auto x : b) {
        if (a.count(x) != 0) {
            ret.insert(x);
        }
    }
    return ret;
}

template <class T> std::set<T> SetUnion(const std::set<T> &a, const std::set<T> &b) {
    std::set<T> ret(a);
    for (auto x : b) {
        ret.insert(x);
    }
    return ret;
}

// a-b
template <class T> std::set<T> SetDiff(const std::set<T> &a, const std::set<T> &b) {
    std::set<T> ret(a);
    for (auto x : b) {
        ret.erase(x);
    }
    return ret;
}

void Liveness::UpdateDefUse() {

    DEF.clear();
    USE.clear();

    auto mcfg = current_func->getMachineCFG();
    // 顺序遍历每个基本块
    mcfg->seqscan_open();
    while (mcfg->seqscan_hasNext()) {
        auto node = mcfg->seqscan_next();

        // DEF[B]: 在基本块B中定义，并且定义前在B中没有被使用的变量集合
        // USE[B]: 在基本块B中使用，并且使用前在B中没有被定义的变量集合
        DEF[node->Mblock->getLabelId()].clear();
        USE[node->Mblock->getLabelId()].clear();

        auto &cur_def = DEF[node->Mblock->getLabelId()];
        auto &cur_use = USE[node->Mblock->getLabelId()];

        for (auto ins : *(node->Mblock)) {
            for (auto reg_r : ins->GetReadReg()) {
                if (cur_def.find(*reg_r) == cur_def.end()) {
                    cur_use.insert(*reg_r);
                }
            }
            for (auto reg_w : ins->GetWriteReg()) {
                if (cur_use.find(*reg_w) == cur_use.end()) {
                    cur_def.insert(*reg_w);
                }
            }
        }
    }
    mcfg->seqscan_close();
}

void Liveness::Execute() {
    UpdateDefUse();

    OUT.clear();
    IN.clear();

    auto mcfg = current_func->getMachineCFG();
    bool changed = 1;
    // 基于数据流分析的活跃变量分析
    while (changed) {
        changed = 0;
        // 顺序遍历每个基本块
        mcfg->seqscan_open();
        while (mcfg->seqscan_hasNext()) {
            auto node = mcfg->seqscan_next();
            std::set<Register> out;
            int cur_id = node->Mblock->getLabelId();
            for (auto succ : mcfg->GetSuccessorsByBlockId(cur_id)) {
                out = SetUnion<Register>(out, IN[succ->Mblock->getLabelId()]);
            }
            if (out != OUT[cur_id]) {
                OUT[cur_id] = out;
            }
            std::set<Register> in = SetUnion<Register>(USE[cur_id], SetDiff<Register>(OUT[cur_id], DEF[cur_id]));
            if (in != IN[cur_id]) {
                changed = 1;
                IN[cur_id] = in;
            }
        }
        mcfg->seqscan_close();
    }
}
bool PhysicalRegistersAllocTools::OccupyReg(int phy_id, LiveInterval interval) {
    // 你需要保证interval不与phy_id已有的冲突
    // 或者增加判断分配失败返回false的代码
    phy_occupied[phy_id].push_back(interval);
    return true;
}

bool PhysicalRegistersAllocTools::ReleaseReg(int phy_id, LiveInterval interval) { 
    //TODO("ReleaseReg"); 
    for(auto it=phy_occupied[phy_id].begin();it!=phy_occupied[phy_id].end();++it)
    {
        if((*it).getReg() == interval.getReg())//找到要释放的活跃区间
        {
            phy_occupied[phy_id].erase(it);
            return true;
        }
    }
    return false; 
}

bool PhysicalRegistersAllocTools::OccupyMem(int offset, LiveInterval interval) {
    //TODO("OccupyMem");
    auto cur_vreg=interval.getReg();
    auto size=cur_vreg.getDataWidth()/4;
    for(int i=offset;i<offset+size;i++)
    {
        while(i>=mem_occupied.size()){mem_occupied.push_back({});}
        mem_occupied[i].push_back(interval);
    }
    return true;
}
bool PhysicalRegistersAllocTools::ReleaseMem(int offset, LiveInterval interval) {
    //TODO("ReleaseMem");
    auto cur_vreg=interval.getReg();
    auto size=cur_vreg.getDataWidth()/4;
    for(int i=offset;i<offset+size;i++)
    { 
        for(auto it=mem_occupied[i].begin();it!=mem_occupied[i].end();++it)
        {
            if((*it).getReg() == interval.getReg())
            {
                mem_occupied[i].erase(it);
                break;
            }
        }
    }
     return true;
}

int PhysicalRegistersAllocTools::getIdleReg(LiveInterval interval) {
    //TODO("getIdleReg");
    for(auto i:getValidRegs(interval))
    {
        int flag=true;
        for(auto conflict_j:getAliasRegs(i))//其实只是获取自己
        {
            for(auto other_interval:phy_occupied[conflict_j])//自己的活跃区间不会重叠
            {
                if(interval&other_interval)//检查活跃区间是否重叠
                {
                    flag=false;
                    break;
                }
            }
        }
        if(flag){return i;}
    }

    return -1;
    // PRINT("\nVreg: ");
    // interval.Print();
    // for(auto i:getValidRegs(interval))
    // {
    //     int ok=true;
    //     for(auto conflict_j:getAliasRegs(i))
    //     {
    //         for(auto other_interval:phy_occupied[conflict_j])
    //         {
    //             PRINT("\nTry Phy %d",i);
    //             PRINT("Other:");
    //             other_interval.Print();
    //             if(interval&other_interval)//检查活跃区间是否重叠
    //             {
    //                 PRINT("\n->Fail\n");
    //                 ok=false;
    //                 break;
    //             }
    //             else{PRINT("\n->Success\n");}
    //         }
    //     }
    //     if(ok){return i;}
    // }

    // return -1;
}

//reference:https://github.com/yuhuifishash/SysY/target/common/machine_passes/register_alloc/physical_register.cc line128-line152
int PhysicalRegistersAllocTools::getIdleMem(LiveInterval interval) //{ TODO("getIdleMem"); }
{
    std::vector<bool> ok;
    ok.resize(mem_occupied.size(),true);
    for(int i=0;i<mem_occupied.size();i++)
    {
        ok[i]=true;
        for(auto other_interval:mem_occupied[i])
        {
            if(interval&other_interval)
            {
                ok[i]=false;//该内存处不可使用
                break;
            }
        }
    }
    int free_cnt=0;
    for(int offset=0;offset<ok.size();offset++)
    {
        if(ok[offset]){free_cnt++;}
        else{free_cnt=0;}
        if(free_cnt==interval.getReg().getDataWidth()/4)
        {return offset-free_cnt+1;}//返回可用块的起始偏移量
    }
    return mem_occupied.size()-free_cnt;//返回最后的可用偏移
}

int PhysicalRegistersAllocTools::swapRegspill(int p_reg1, LiveInterval interval1, int offset_spill2, int size,
                                              LiveInterval interval2) {

    //TODO("swapRegspill");
    ReleaseReg(p_reg1,interval1);
    ReleaseMem(offset_spill2,interval2);
    OccupyReg(p_reg1,interval2);
    return 0;
}
std::vector<LiveInterval> PhysicalRegistersAllocTools::getConflictIntervals(LiveInterval interval) {
    std::vector<LiveInterval> result;
    for (auto phy_intervals : phy_occupied) {
        for (auto other_interval : phy_intervals) {
            if (interval.getReg().type == other_interval.getReg().type && (interval & other_interval)) {
                result.push_back(other_interval);
            }
        }
    }
    return result;
}
// 获取可分配的寄存器列表（不考虑区间冲突）
std::vector<int> RiscV64RegisterAllocTools::getValidRegs(LiveInterval interval) {
    if (interval.getReg().type.data_type == MachineDataType::INT) {
        return std::vector<int>({
        RISCV_t0, RISCV_t1, RISCV_t2, RISCV_t3, RISCV_t4, RISCV_t5,  RISCV_t6,  RISCV_a0, RISCV_a1, RISCV_a2,
        RISCV_a3, RISCV_a4, RISCV_a5, RISCV_a6, RISCV_a7, RISCV_s0,  RISCV_s1,  RISCV_s2, RISCV_s3, RISCV_s4,
        RISCV_s5, RISCV_s6, RISCV_s7, RISCV_s8, RISCV_s9, RISCV_s10, RISCV_s11, RISCV_ra,
        });
    } else if (interval.getReg().type.data_type == MachineDataType::FLOAT) {
        return std::vector<int>({
        RISCV_f0,  RISCV_f1,  RISCV_f2,  RISCV_f3,  RISCV_f4,  RISCV_f5,  RISCV_f6,  RISCV_f7,
        RISCV_f8,  RISCV_f9,  RISCV_f10, RISCV_f11, RISCV_f12, RISCV_f13, RISCV_f14, RISCV_f15,
        RISCV_f16, RISCV_f17, RISCV_f18, RISCV_f19, RISCV_f20, RISCV_f21, RISCV_f22, RISCV_f23,
        RISCV_f24, RISCV_f25, RISCV_f26, RISCV_f27, RISCV_f28, RISCV_f29, RISCV_f30, RISCV_f31,
        });
    } else {
        //ERROR("Unsupported Reg data type");
        return std::vector<int>();
    }
}    