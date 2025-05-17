#include"inst_select.h"
#include"machine_instruction.h"
#include <functional>

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
    // 与中间代码生成一样, 如果你完全无从下手, 可以先看看输出是怎么写的
    // 即riscv64gc/instruction_print/*  common/machine_passes/machine_printer.h
    // 指令选择除了一些函数调用约定必须遵守的情况需要物理寄存器，其余情况必须均为虚拟寄存器
    global_def = IR->global_def;

    // 遍历每个LLVM IR函数    
    for (auto [defI,cfg] : IR->llvm_cfg) {
        cfg->BuildCFG();
        if (cfg && cfg->DomTree) {
            ((DominatorTree*)cfg->DomTree)->BuildDominatorTree();
        }
        if(cfg == nullptr){
            ERROR("LLVMIR CFG is Empty, you should implement BuildCFG in MidEnd first");
        }
        std::string name = cfg->function_def->GetFunctionName();

        cur_func = new RiscV64Function(name);
        // cur_func->SetParent(dest);
        cur_func->SetParent(this);
        // 你可以使用cur_func->GetNewRegister来获取新的虚拟寄存器
        functions.push_back(cur_func);

        auto cur_mcfg = new MachineCFG;
        cur_func->SetMachineCFG(cur_mcfg);

        cur_cfg = cfg;

        // 清空指令选择状态(可能需要自行添加初始化操作)
        ClearFunctionSelectState();

        // TODO: 添加函数参数(推荐先阅读一下riscv64_lowerframe.cc中的代码和注释)
        // See MachineFunction::AddParameter()
        auto funcdefI = (FunctionDefineInstruction*)defI;
        int i32_cnt = 0;
        int f32_cnt = 0;

        for(int i = 0; i < funcdefI->formals_reg.size(); ++i){
            auto para = funcdefI->formals_reg[i];
            auto paratype = funcdefI->formals[i];
            // 添加参数寄存器Register, 方便riscv64_lowerframe.cc使用
            if(paratype == BasicInstruction::LLVMType::I32 || paratype == BasicInstruction::LLVMType::PTR){
				auto newpara = GetNewRegister(((RegOperand *)funcdefI->formals_reg[i])->GetRegNo(), INT64);
				cur_func->AddParameter(newpara);
            }else if(paratype == BasicInstruction::LLVMType::FLOAT32){
				auto newpara = GetNewRegister(((RegOperand *)funcdefI->formals_reg[i])->GetRegNo(), FLOAT64);
				cur_func->AddParameter(newpara);
            }else{
                ERROR("Unknown type");
            }
        }
        // std::cerr<<cur_offset<<'\n';
        // TODO("Add function parameter if you need");
		// std::cerr << "defI->formals.size():" <<  defI->formals.size() << std::endl;

        BuildPhiWeb(cfg);
        
        for (auto [id, block] : *(cfg->block_map)) {
            cur_block = new RiscV64Block(id);
            curblockmap[id] = cur_block;
            // 将新块添加到Machine CFG中
            cur_mcfg->AssignEmptyNode(id, cur_block);
            cur_func->UpdateMaxLabel(id);

            cur_block->setParent(cur_func);
            cur_func->blocks.push_back(cur_block);
        }

        // 遍历每个LLVM IR基本块

		// icmp / fcmp not found error 
		// slove method1: select icmp_inst/fcmp_inst previously (no use)
		// try : 1. typeid(*instruction).name(), 2. dynamic_cast<>
		// slove method2: search the blocks in domtree

		DomtreeDfs((*cfg->block_map)[0], cfg);

        // visset.clear();
        // DFS((*cfg->block_map)[0], cfg, cur_mcfg, cur_func);
        // for (auto [id, block] : *(cfg->block_map)) {
        //     cur_block = curblockmap[id];
        //     // 指令选择主要函数, 请注意指令选择时需要维护变量cur_offset
	    //     for (auto instruction : block->Instruction_list) {
		// 		ConvertAndAppend<Instruction>(instruction);
        //     }
        // }

        // RISCV 8字节对齐（）
        if (cur_offset % 8 != 0) {
            cur_offset = ((cur_offset + 7) / 8) * 8;
        }
		// 设置临时变量开辟的栈空间
        cur_func->SetStackSize(cur_offset + cur_func->GetParaSize());

		// 当参数溢出的时候，会影响局部变量的位置
		// 难点.遍历完被调用的所有函数才能知道参数栈的偏移, 此时所有的指令都已经选择
		// 解决方案：先预存所有的局部变量alloca指令，在最后的时候加上参数栈偏移
		// cite from sysY
 		((RiscV64Function *)cur_func)->AddParameterSize(cur_func->GetParaSize());

		// std::cerr << "stackSz: " << cur_offset << std::endl;
        // 控制流图连边
        for (int i = 0; i < cfg->G.size(); i++) {
            const auto &arcs = cfg->G[i];
            for (auto arc : arcs) {
                cur_mcfg->MakeEdge(i, arc->block_id);
            }
        }

		// 打印CFG, 方便调试
		// cur_mcfg->display();

        // 遍历完所有基本块后插入phi生成的额外指令
        for (auto [id, block] : *(cfg->block_map)) {
            cur_block = curblockmap[id];
            if(!phi_instr_map[id].empty()){
                std::deque<MachineBaseInstruction *> terminalI_que;
                auto nowI = cur_block->GetInsList().back();
                while(nowI != terminal_instr_map[id]){
                    terminalI_que.push_back(nowI);
                    cur_block->pop_back();
                    nowI = cur_block->GetInsList().back();
                }
                for(auto I : phi_instr_map[id]){
                    cur_block->push_back(I);
                }
                while(!terminalI_que.empty()){
                    cur_block->push_back(terminalI_que.back());
                    terminalI_que.pop_back();
                }
            }
        }
    }
	
    LowerFrame(); //最后调用LowerFrame
}
void RiscV64Unit::LowerFrame()
{
    // 在每个函数的开头处插入获取参数的指令
    for (auto func : functions) {
        cur_func = func;
        for (auto &b : func->blocks) {
            cur_block = b;
            if (b->getLabelId() == 0) {    // 函数入口，需要插入获取参数的指令
                int i32_cnt = 0;
				int f32_cnt = 0;
				int arg_off = 0;
                for (auto para : func->GetParameters()) {    // 你需要在指令选择阶段正确设置parameters的值
                    // std::cerr<<"asd : "<<para.get_reg_no()<<'\n';
					if (para.type.data_type == INT64.data_type) {	
                        if (i32_cnt < 8) {    // 插入使用寄存器传参的指令
                            b->push_front(rvconstructor->ConstructR(RISCV_ADD, para, GetPhysicalReg(RISCV_a0 + i32_cnt),
                                                                    GetPhysicalReg(RISCV_x0)));
                        }
                        if (i32_cnt >= 8) {    // 插入使用内存传参的指令
							b->push_front(
                            rvconstructor->ConstructIImm(RISCV_LD, para, GetPhysicalReg(RISCV_fp), arg_off));  // fp是上一个栈帧的栈顶
                            arg_off += 8;
                        }
                        i32_cnt++;
                    } else if (para.type.data_type == FLOAT64.data_type) {    // 处理浮点数
                        // TODO("Implement this if you need");
						if (f32_cnt < 8) {    // 插入使用寄存器传参的指令
							b->push_front(rvconstructor->ConstructR2(RISCV_FMV_S, para, GetPhysicalReg(RISCV_fa0 + f32_cnt)));
                        }
                        if (f32_cnt >= 8) {    // 插入使用内存传参的指令
							b->push_front(
                            rvconstructor->ConstructIImm(RISCV_FLD, para, GetPhysicalReg(RISCV_fp), arg_off));
                            arg_off += 8;
                        }
                        f32_cnt++;
                    } else {
                        ERROR("Unknown type");
                    }
                }

				if (arg_off != 0) {
					// std::cerr << "arg_off: " << arg_off << std::endl;
                    cur_func->setIsParaInStack(true);
                }
            }
        }
    }
}

int CalFirLasAncestor(std::vector<int> &reg_occurblockids, MachineDominatorTree *domtree, int domroot) {
    std::vector<int> dfsorder; // 存储深度优先遍历的顺序
    std::map<int, int> vsd; // 存储每个节点在遍历中的状态
    std::map<int, int> dph; // 存储每个节点的深度
   //1. 获取从 domroot 开始的深度优先遍历顺序,存储在vsd中
    std::stack<int> stack;
    stack.push(domroot);
    vsd[domroot] = 1;
    
    while (!stack.empty()) {
        int node = stack.top();
        stack.pop();
        dfsorder.push_back(node);

        // 获取当前节点的子节点列表，并将未访问过的子节点压入栈
        for (auto child : domtree->dom_tree[node]) {
            int child_id = child->getLabelId();
            if (vsd.find(child_id) == vsd.end()) {
                vsd[child_id] = 1;
                stack.push(child_id);
            }
        }
    }
    
    //2.获取从 domroot 开始的每个节点的深度,存储在dph中
    std::stack<std::pair<int, int>> stack_pair; // 存放节点 ID 和其深度的栈
    stack_pair.push(std::make_pair(domroot, 1)); // 初始化根节点深度为 1
    dph[domroot] = 1;

    while (!stack_pair.empty()) {
        auto [cur, depth] = stack_pair.top();
        stack_pair.pop();
        
        // 更新当前节点的深度（如有需要）
        if (dph[cur] < depth) {
            dph[cur] = depth;
        }

        // 遍历子节点
        for (auto child : domtree->dom_tree[cur]) {
            int child_id = child->getLabelId();
            if (dph.find(child_id) == dph.end() || dph[child_id] < depth + 1) {
                dph[child_id] = depth + 1;
                stack_pair.push(std::make_pair(child_id, depth + 1)); // 将子节点压入栈，深度增加
            }
        }
    }
    //3.寻找公共祖先
    //1)标记包含寄存器的块
    int x = -1, y = -1; // 初始化 x 和 y，用于存储找到的块 ID
    std::map<int, int> blockhasreg; // 存储每个块是否包含寄存器的映射
    for (auto b : reg_occurblockids) { // 遍历所有寄存器出现的块 ID
        blockhasreg[b] = 1; // 将包含寄存器的块标记为 1
    }
    //2）找到第一个包含寄存器的块ID
    for (auto it = dfsorder.begin(); it != dfsorder.end(); ++it) { // 从前向后遍历 dfsorder
        if (blockhasreg[*it]) { // 如果当前块包含寄存器
            x = *it; // 记录第一个包含寄存器的块 ID
            break; // 找到后退出循环
        }
    }
    //3)找到最后一个包含寄存器的块ID
    for (auto it = dfsorder.rbegin(); it != dfsorder.rend(); ++it) { // 从后向前遍历 dfsorder
        if (blockhasreg[*it]) { // 如果当前块包含寄存器
            y = *it; // 记录最后一个包含寄存器的块 ID
            break; // 找到后退出循环
        }
    }
    Assert(x != -1 && y != -1); // 确保找到了有效的 x 和 y 块 ID
    //4)计算x和y最近的公共祖先
    //return CalculatePairLCA(x, y, domtree, dph); // 计算并返回 x 和 y 块的最近公共祖先
    std::map<int, int> parent;
    std::map<int, int> rank;
    
    // 初始化每个节点的父节点是自己，rank为0
    for (auto& entry : dph) {
        parent[entry.first] = entry.first;
        rank[entry.first] = 0;
    }

    // 将节点根据其深度初始化，确保深度较大的节点先被处理
    while (dph[x] > dph[y]) {
        x = domtree->idom[x]->getLabelId();
    }
    while (dph[y] > dph[x]) {
        y = domtree->idom[y]->getLabelId();
    }

    // 利用并查集算法寻找到共同祖先
    while (x != y) {
        x = domtree->idom[x]->getLabelId();
        y = domtree->idom[y]->getLabelId();
    }

    // 找到初步的最近公共祖先后，我们还需要检查循环深度
    while (domtree->C->GetNodeByBlockId(x)->Mblock->loop_depth != 0) {
        x = domtree->idom[x]->getLabelId();
    }

    return x;
}
const int TotalRegs = 64;
extern bool optimize_flag;//在main.cc中定义
void RiscV64Unit::LowerStack()
{
    //TODO
    //原架构中需要通过unit获取functions等信息，此处直接获取即可
    for(auto func:functions)
    {
        cur_func=func;
        std::vector<std::vector<int>> saveregs_occurblockids,saveregs_rwblockids;
        //GatherUseSregs(func,saveregs_occurblockids,saveregs_rwblockids);
        //1.搜集使用的寄存器信息
        saveregs_occurblockids.resize(TotalRegs);
        saveregs_rwblockids.resize(TotalRegs);

        for(auto &b:func->blocks)
        {
            std::bitset<TotalRegs> RegNeedSaved;
            RegNeedSaved.set(RISCV_s0);
            RegNeedSaved.set(RISCV_s1);
            RegNeedSaved.set(RISCV_s2);
            RegNeedSaved.set(RISCV_s3);
            RegNeedSaved.set(RISCV_s4);
            RegNeedSaved.set(RISCV_s5);
            RegNeedSaved.set(RISCV_s6);
            RegNeedSaved.set(RISCV_s7);
            RegNeedSaved.set(RISCV_s8);
            RegNeedSaved.set(RISCV_s9);
            RegNeedSaved.set(RISCV_s10);
            RegNeedSaved.set(RISCV_s11);
            RegNeedSaved.set(RISCV_fs0);
            RegNeedSaved.set(RISCV_fs1);
            RegNeedSaved.set(RISCV_fs2);
            RegNeedSaved.set(RISCV_fs3);
            RegNeedSaved.set(RISCV_fs4);
            RegNeedSaved.set(RISCV_fs5);
            RegNeedSaved.set(RISCV_fs6);
            RegNeedSaved.set(RISCV_fs7);
            RegNeedSaved.set(RISCV_fs8);
            RegNeedSaved.set(RISCV_fs9);
            RegNeedSaved.set(RISCV_fs10);
            RegNeedSaved.set(RISCV_fs11);
            RegNeedSaved.set(RISCV_ra);
            std::bitset<TotalRegs> RegVisited; // 用于记录当前基本块中访问过的寄存器
            for(auto ins:*b)
            {
                for (auto reg : ins->GetWriteReg()) 
                {
                    if (!reg->is_virtual && RegNeedSaved[reg->reg_no] && !RegVisited[reg->reg_no]) 
                    {
                        RegVisited[reg->reg_no] = true;
                        saveregs_occurblockids[reg->reg_no].push_back(b->getLabelId());
                        saveregs_rwblockids[reg->reg_no].push_back(b->getLabelId());
                    }
                }
                for (auto reg : ins->GetReadReg()) 
                {
                    if (!reg->is_virtual && RegNeedSaved[reg->reg_no] && !RegVisited[reg->reg_no]) 
                    {
                        saveregs_rwblockids[reg->reg_no].push_back(b->getLabelId());
                    }
                }
            }
        }
        
        if (func->HasInParaInStack()) 
        {
            saveregs_occurblockids[RISCV_fp].push_back(0);
            saveregs_rwblockids[RISCV_fp].push_back(0);
        }

        //2.分配栈空间
        std::vector<int> sd_blocks;
        std::vector<int> ld_blocks;
        std::vector<int> restore_offset;
        sd_blocks.resize(64);
        ld_blocks.resize(64);
        restore_offset.resize(64);
        int saveregnum = 0, cur_restore_offset = 0;
         for (int i = 0; i < saveregs_occurblockids.size(); i++) { // 遍历保存寄存器的出现块 ID
            auto &vld = saveregs_rwblockids[i]; // 获取与当前保存寄存器相关的读写块 ID
            if (!vld.empty()) { // 如果读写块不为空
                saveregnum++; // 保存寄存器数量加1
            }
        }
        func->AddStackSize(saveregnum*8);

        //2.恢复栈空间
        auto mcfg = func->getMachineCFG(); // 获取函数的机器控制流图
        bool restore_at_beginning = (-8 + func->GetStackSize()) >= 2048; // 如果函数栈空间太大，需要在开始时恢复寄存器
        if (!optimize_flag) { // 如果未启用优化标志
            restore_at_beginning = true; // 强制在开始时恢复寄存器
        }
        //1）如果无需在开始时恢复寄存器
        if(!restore_at_beginning)
        {
            func->getMachineCFG()->BuildDominatoorTree();
            for(int i=0;i<saveregs_occurblockids.size();i++)
            {
                auto &vld = saveregs_rwblockids[i]; // 获取与当前保存寄存器相关的读写块 ID
                auto &vsd = saveregs_rwblockids[i]; // 获取当前保存寄存器的读写块 ID（重复）
                if (!vld.empty()) { // 如果读写块不为空
                    cur_restore_offset -= 8; // 当前恢复偏移量减去8
                    restore_offset[i] = cur_restore_offset; // 更新恢复偏移量
                    ld_blocks[i] = CalFirLasAncestor(vld, &func->getMachineCFG()->PostDomTree,
                                                     func->getMachineCFG()->ret_block->Mblock->getLabelId()); // 计算恢复块
                    if (ld_blocks[i] != func->getMachineCFG()->ret_block->Mblock->getLabelId()) {
                        ld_blocks[i] = func->getMachineCFG()->PostDomTree.idom[ld_blocks[i]]->getLabelId(); // 更新恢复块
                    }
                    vsd.push_back(ld_blocks[i]); // 将恢复块添加到 vds 中
                    sd_blocks[i] = CalFirLasAncestor(vsd, &func->getMachineCFG()->DomTree, 0); // 计算保存块
                }
            }
              for (int i = 0; i < saveregs_occurblockids.size(); i++) { // 再次遍历保存寄存器的出现块
                if (!saveregs_occurblockids[i].empty()) { // 如果当前保存寄存器的出现块不为空
                    int regno = i; // 当前寄存器号
                    int sp_offset = restore_offset[i] + func->GetStackSize(); // 计算栈偏移量
                    if (!func->HasInParaInStack() || i != RISCV_fp) { // 如果没有栈参数或者当前寄存器不是 fp
                        int saveb = sd_blocks[i]; // 获取保存块相关的 ID
                        auto block = mcfg->GetNodeByBlockId(saveb)->Mblock; // 获取保存块对应的基本块
                        int sd_op = 0; // 初始化保存操作的操作码
                        if (regno >= RISCV_x0 && regno <= RISCV_x31) { // 如果寄存器是整型
                            sd_op = RISCV_SD; // 设置操作码为 SD
                        } else {
                            sd_op = RISCV_FSD; // 设置操作码为 FSD
                        }
                        block->push_front(
                        rvconstructor->ConstructSImm(sd_op, GetPhysicalReg(i), GetPhysicalReg(RISCV_sp), sp_offset)); // 在基本块前插入保存寄存器的指令
                    }
                    int restoreb = ld_blocks[i]; // 获取恢复块相关的 ID
                    auto block = mcfg->GetNodeByBlockId(restoreb)->Mblock; // 获取恢复块对应的基本块
                    auto it = block->getInsertBeforeBrIt(); // 获取插入位置

                    int ld_op = 0; // 初始化恢复操作的操作码
                    if (regno >= RISCV_x0 && regno <= RISCV_x31) { // 如果寄存器是整型
                        ld_op = RISCV_LD; // 设置操作码为 LD
                    } else {
                        ld_op = RISCV_FLD; // 设置操作码为 FLD
                    }
                    block->insert(
                    it, rvconstructor->ConstructIImm(ld_op, GetPhysicalReg(i), GetPhysicalReg(RISCV_sp), sp_offset)); // 在基本块中插入恢复寄存器的指令
                }
            }
        }

         for (auto &b : func->blocks) {
            cur_block = b;
            if (b->getLabelId() == 0) {
                if (func->GetStackSize() <= 2032) {
                    b->push_front(rvconstructor->ConstructIImm(RISCV_ADDI, GetPhysicalReg(RISCV_sp),
                                                               GetPhysicalReg(RISCV_sp),
                                                               -func->GetStackSize()));    // sub sp
                } else {
                    auto stacksz_reg = GetPhysicalReg(RISCV_t0);
                    b->push_front(rvconstructor->ConstructR(RISCV_SUB, GetPhysicalReg(RISCV_sp),
                                                            GetPhysicalReg(RISCV_sp), stacksz_reg));
                    auto addiw_instr1 = rvconstructor->ConstructUImm(RISCV_LUI, stacksz_reg,  (func->GetStackSize() + (1 << 11)) >> 12);//修改//////////////////////////
                    b->push_front(addiw_instr1);
                    auto addiw_instr2 = rvconstructor->ConstructIImm(RISCV_ORI, stacksz_reg, stacksz_reg,func->GetStackSize()& 0xfff);//修改//////////////////////////
                    b->push_front(addiw_instr2);
                    //b->push_front(rvconstructor->ConstructUImm(RISCV_LI, stacksz_reg, func->GetStackSize()));
                }
                if (func->HasInParaInStack()) {
                    b->push_front(rvconstructor->ConstructR(RISCV_ADD, GetPhysicalReg(RISCV_fp),
                                                            GetPhysicalReg(RISCV_sp), GetPhysicalReg(RISCV_x0)));
                }
                // fp should always be restored at beginning now
                if (restore_at_beginning) {
                    int offset = 0;
                    for (int i = 0; i < 64; i++) {
                        if (!saveregs_occurblockids[i].empty()) {
                            int regno = i;
                            offset -= 8;
                            if (regno >= RISCV_x0 && regno <= RISCV_x31) {
                                b->push_front(rvconstructor->ConstructSImm(RISCV_SD, GetPhysicalReg(regno),
                                                                           GetPhysicalReg(RISCV_sp), offset));
                            } else {
                                b->push_front(rvconstructor->ConstructSImm(RISCV_FSD, GetPhysicalReg(regno),
                                                                           GetPhysicalReg(RISCV_sp), offset));
                            }
                        }
                    }
                } else if (func->HasInParaInStack()) {
                    b->push_front(rvconstructor->ConstructSImm(RISCV_SD, GetPhysicalReg(RISCV_fp),
                                                               GetPhysicalReg(RISCV_sp), restore_offset[RISCV_fp]));
                }
            }
            auto y_ins = *(b->ReverseBegin());
            Assert(y_ins->arch == MachineBaseInstruction::RiscV);
            auto riscv_y_ins = (RiscV64Instruction *)y_ins;
            if (riscv_y_ins->getOpcode() == RISCV_JALR) {
                if (riscv_y_ins->getRd() == GetPhysicalReg(RISCV_x0)) {
                    if (riscv_y_ins->getRs1() == GetPhysicalReg(RISCV_ra)) {
                        Assert(riscv_y_ins->getImm() == 0);
                        b->pop_back();
                        // b->push_back(rvconstructor->ConstructComment("Lowerstack: add sp\n"));
                        if (func->GetStackSize() <= 2032) {
                            b->push_back(rvconstructor->ConstructIImm(RISCV_ADDI, GetPhysicalReg(RISCV_sp),
                                                                      GetPhysicalReg(RISCV_sp), func->GetStackSize()));
                        } else {
                            auto stacksz_reg = GetPhysicalReg(RISCV_t0);
                            auto addiw_instr1 = rvconstructor->ConstructUImm(RISCV_LUI, stacksz_reg,  (func->GetStackSize() + (1 << 11)) >> 12);//修改//////////////////////////
                            b->push_front(addiw_instr1);
                            auto addiw_instr2 = rvconstructor->ConstructIImm(RISCV_ORI, stacksz_reg, stacksz_reg,func->GetStackSize()& 0xfff);//修改//////////////////////////
                            b->push_front(addiw_instr2);
                            //b->push_back(rvconstructor->ConstructUImm(RISCV_LI, stacksz_reg, func->GetStackSize()));
                            b->push_back(rvconstructor->ConstructR(RISCV_ADD, GetPhysicalReg(RISCV_sp),
                                                                   GetPhysicalReg(RISCV_sp), stacksz_reg));
                        }
                        if (restore_at_beginning) {
                            int offset = 0;
                            for (int i = 0; i < 64; i++) {
                                if (!saveregs_occurblockids[i].empty()) {
                                    int regno = i;
                                    offset -= 8;
                                    if (regno >= RISCV_x0 && regno <= RISCV_x31) {
                                        b->push_back(rvconstructor->ConstructIImm(RISCV_LD, GetPhysicalReg(regno),
                                                                                  GetPhysicalReg(RISCV_sp), offset));
                                    } else {
                                        b->push_back(rvconstructor->ConstructIImm(RISCV_FLD, GetPhysicalReg(regno),
                                                                                  GetPhysicalReg(RISCV_sp), offset));
                                    }
                                }
                            }
                        }
                        b->push_back(riscv_y_ins);
                    }
                }
            }
         }
    }

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
    if (!ubb || !C || !C->DomTree) {
        ERROR("Invalid parameters in DomtreeDfs");
        return;
    }
    auto ubbid = ubb->block_id;
	auto DomTree = (DominatorTree*)C->DomTree;
	auto domtree = DomTree->dom_tree;
	// DomTree->display();
    cur_block = curblockmap[ubbid];
    
    // std::cerr << "\n[指令选择] 开始处理基本块 B" << ubbid << std::endl;
    for (auto instruction : ubb->Instruction_list) {
        // std::cerr << "[指令选择] 处理指令: ";
        // instruction->PrintIR(std::cerr);
        ConvertAndAppend<Instruction>(instruction);
    }
    // std::cerr << "[指令选择] 完成基本块 B" << ubbid << " 的处理\n" << std::endl;

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

/* (1) global var or const var
*  %t1 = load i32* @a ====> lui %1, %hi(a)
* 							lw %2,%lo(a)(%1)  #riscv
*
*  (2) Load a temporary variable from the stack into a register
*  %t1 = load i32* %t2 ====> lw %1, 0(%2) #riscv
*
*  where assume that the value in the %t2 register corresponds to the virtual register %2
*  (3) Load an array element (PTR)
* 	...
*/
template <> void RiscV64Unit::ConvertAndAppend<LoadInstruction*>(LoadInstruction* inst) {
	int op_code = (inst->GetDataType() == BasicInstruction::LLVMType::FLOAT32) ? RISCV_FLW :
				  (inst->GetDataType() == BasicInstruction::LLVMType::I32    ) ? RISCV_LW  :
				  (inst->GetDataType() == BasicInstruction::LLVMType::PTR    ) ? RISCV_LD  : -1 ;

	MachineDataType target_type = (inst->GetDataType() == BasicInstruction::LLVMType::FLOAT32) ? FLOAT64 :
								  (inst->GetDataType() == BasicInstruction::LLVMType::I32    ) ? INT64  :
								  (inst->GetDataType() == BasicInstruction::LLVMType::PTR    ) ? INT64  : INT64;
	if (inst->GetPointer()->GetOperandType() == BasicOperand::REG) {

        auto pointer_op = (RegOperand *)inst->GetPointer();
        auto rd_op = (RegOperand *)inst->GetResult();
		Register rd = GetNewRegister(rd_op->GetRegNo(), target_type);

		if (llvmReg_offset_map.find(pointer_op->GetRegNo()) == llvmReg_offset_map.end()) {
			auto ptr_reg = GetNewRegister(pointer_op->GetRegNo(), INT64);
			
			// lw rd, 0(ptr_reg)
			auto lw_inst = rvconstructor->ConstructIImm(op_code, rd, ptr_reg, 0);
			cur_block->push_back(lw_inst);
		} else { // case 2. pointer_op is a alloca_reg that pointed to stack
			auto stack_offset = llvmReg_offset_map[pointer_op->GetRegNo()];
            // do right here
			// lw rd, stack_offset(sp)
			auto lw_inst = rvconstructor->ConstructIImm(op_code, rd, GetPhysicalReg(RISCV_sp), stack_offset);
			cur_block->push_back(lw_inst);
			((RiscV64Function *)cur_func)->AddAllocaInst(lw_inst);
		}

    } else if (inst->GetPointer()->GetOperandType() == BasicOperand::GLOBAL) {
		// lui %1, %hi(a)
		// lw %2,%lo(a)(%1)  #riscv
        auto global_op = (GlobalOperand *)inst->GetPointer();
        auto rd_op = (RegOperand *)inst->GetResult();
		Register rd = GetNewRegister(rd_op->GetRegNo(), target_type);

        Register ptr_reg = GetNewTempRegister(INT64);
		auto lui_inst = rvconstructor->ConstructULabel(RISCV_LUI, ptr_reg, RiscVLabel(global_op->GetName(), true));
		auto lw_inst = rvconstructor->ConstructILabel(op_code, rd, ptr_reg, RiscVLabel(global_op->GetName(), false));

		cur_block->push_back(lui_inst);
		cur_block->push_back(lw_inst);
    }
}

// StoreInstruction is same as LoadInstruction
template <> void RiscV64Unit::ConvertAndAppend<StoreInstruction*>(StoreInstruction* inst) {
    // TODO("Implement this if you need");
	Operand value_op = inst->GetValue();
	Register value_reg;

	// solve value (update msg in value_reg)
	if (value_op->GetOperandType() == BasicOperand::IMMI32){
		// create a tmp reg to store imm
		value_reg = GetNewTempRegister(INT64);
		InsertImmI32Instruction(value_reg, (ImmI32Operand *)inst->GetValue(), cur_block);
	} else if (value_op->GetOperandType() == BasicOperand::IMMF32) {
		value_reg = GetNewTempRegister(FLOAT64);
		InsertImmFloat32Instruction(value_reg, (ImmF32Operand *)inst->GetValue(), cur_block);
	} else if (value_op->GetOperandType() == BasicOperand::REG) {
		RegOperand *value_regop = (RegOperand *)inst->GetValue();
		if(inst->GetDataType() == BasicInstruction::LLVMType::I32){
			value_reg = GetNewRegister(value_regop->GetRegNo(), INT64);
		} else if (inst->GetDataType() == BasicInstruction::LLVMType::FLOAT32) {
			value_reg = GetNewRegister(value_regop->GetRegNo(), FLOAT64);
		}
	}

	// solve pointer (the address want to be store)
	int op_code = (inst->GetDataType() == BasicInstruction::LLVMType::FLOAT32) ? RISCV_FSW :
				  (inst->GetDataType() == BasicInstruction::LLVMType::I32) ? RISCV_SW : -1 ;

	if (inst->GetPointer()->GetOperandType() == BasicOperand::REG) {  // local var (store in stack)
		RegOperand* pointer_op = (RegOperand *)inst->GetPointer();
		// case 1. pointer_op is a ptr_reg pointed to address 
		if (llvmReg_offset_map.find(pointer_op->GetRegNo()) == llvmReg_offset_map.end()) {
			auto ptr_reg = GetNewRegister(pointer_op->GetRegNo(), INT64);

			// sw value_reg, 0(ptr_reg)
			auto store_inst = rvconstructor->ConstructSImm(op_code, value_reg, ptr_reg, 0);
			cur_block->push_back(store_inst);
		} else { // case 2. pointer_op is a alloca_reg that pointed to stack
			auto stack_offset = llvmReg_offset_map[pointer_op->GetRegNo()];

			// sw value_reg, stack_offset(sp)
			auto store_inst = rvconstructor->ConstructSImm(op_code, value_reg, GetPhysicalReg(RISCV_sp), stack_offset);
			cur_block->push_back(store_inst);
			((RiscV64Function *)cur_func)->AddAllocaInst(store_inst);
		}
	} else if (inst->GetPointer()->GetOperandType() == BasicOperand::GLOBAL) {  // global var  (address = hi + lo)
		GlobalOperand* global_op = (GlobalOperand *)inst->GetPointer();

		auto ptr_reg = GetNewTempRegister(INT64);
		// lui ptr_reg, %hi(a)
		auto lui_inst = rvconstructor->ConstructULabel(RISCV_LUI, ptr_reg, RiscVLabel(global_op->GetName(), true));
		cur_block->push_back(lui_inst);

		// sw (fsw) value_reg, %lo(a)(ptr_reg)
		auto store_inst = rvconstructor->ConstructSLabel(op_code, value_reg, ptr_reg, RiscVLabel(global_op->GetName(), false));
		cur_block->push_back(store_inst);
	}
}

template <> void RiscV64Unit::ConvertAndAppend<ArithmeticInstruction *>(ArithmeticInstruction *inst) {
	bool op1_isreg = (inst->GetOperand1()->GetOperandType() == BasicOperand::REG);
	bool op2_isreg = (inst->GetOperand2()->GetOperandType() == BasicOperand::REG);
	auto *rd_op = (RegOperand *)inst->GetResult();

	auto *reg_op1 = (RegOperand *)inst->GetOperand1();
	auto *imm_op1 = (ImmI32Operand *)inst->GetOperand1();

	auto *reg_op2 = (RegOperand *)inst->GetOperand2();
	auto *imm_op2 = (ImmI32Operand *)inst->GetOperand2();

	switch (inst->GetOpcode()) {

		case BasicInstruction::LLVMIROpcode::ADD:
		 	// Imm + Imm 的情况, riscv64 指令集无法解决, 在窥孔优化里消除
			// 所有的 Imm 都需要存储在 Imm_reg 中，因为 addiw 等立即数运算有位宽限制
			if(op1_isreg && op2_isreg){  // reg + reg
                auto rd = GetNewRegister(rd_op->GetRegNo(), INT64);
                auto rs = GetNewRegister(reg_op1->GetRegNo(), INT64);
                auto rt = GetNewRegister(reg_op2->GetRegNo(), INT64);

				auto addw_instr = rvconstructor->ConstructR(RISCV_ADDW, rd, rs, rt);
                cur_block->push_back(addw_instr);
			} else if (op1_isreg && !op2_isreg) { // reg + imm 
                auto rd = GetNewRegister(rd_op->GetRegNo(), INT64);
                auto rs = GetNewRegister(reg_op1->GetRegNo(), INT64);
				Register imm_reg = GetNewTempRegister(INT64);
				InsertImmI32Instruction(imm_reg, imm_op2, cur_block);

				auto addw_instr = rvconstructor->ConstructR(RISCV_ADDW, rd, rs, imm_reg);
				cur_block->push_back(addw_instr);
			} else if (op1_isreg || op2_isreg) {  // imm + reg 
				auto rd = GetNewRegister(rd_op->GetRegNo(), INT64);
				Register imm_reg = GetNewTempRegister(INT64);
				InsertImmI32Instruction(imm_reg, imm_op1, cur_block);
				auto rt = GetNewRegister(reg_op2->GetRegNo(), INT64);
				
				auto addw_instr = rvconstructor->ConstructR(RISCV_ADDW, rd, imm_reg, rt);
				cur_block->push_back(addw_instr);
			} else std::cerr << "Error: addiw rd, imm, imm is not allowed" << std::endl;
			break;  

		case BasicInstruction::LLVMIROpcode::SUB:
			if(op1_isreg && op2_isreg){  // reg - reg
                auto rd = GetNewRegister(rd_op->GetRegNo(), INT64);
                auto rs = GetNewRegister(reg_op1->GetRegNo(), INT64);
                auto rt = GetNewRegister(reg_op2->GetRegNo(), INT64);

				auto subw_instr = rvconstructor->ConstructR(RISCV_SUBW, rd, rs, rt);
                cur_block->push_back(subw_instr);
			} else if (op1_isreg && !op2_isreg) { // reg - imm 
                auto rd = GetNewRegister(rd_op->GetRegNo(), INT64);
                auto rs = GetNewRegister(reg_op1->GetRegNo(), INT64);
				Register imm_reg = GetNewTempRegister(INT64);
				InsertImmI32Instruction(imm_reg, imm_op2, cur_block);

				auto subw_instr = rvconstructor->ConstructR(RISCV_SUBW, rd, rs, imm_reg);
				cur_block->push_back(subw_instr);
			} else if (op1_isreg || op2_isreg) {  // imm - reg 
				auto rd = GetNewRegister(rd_op->GetRegNo(), INT64);
				Register imm_reg = GetNewTempRegister(INT64);
				InsertImmI32Instruction(imm_reg, imm_op1, cur_block);
				auto rt = GetNewRegister(reg_op2->GetRegNo(), INT64);
				
				auto subw_instr = rvconstructor->ConstructR(RISCV_SUBW, rd, imm_reg, rt);
				cur_block->push_back(subw_instr);
			} else std::cerr << "Error: subiw rd, imm, imm is not allowed" << std::endl;
			break;  

		case BasicInstruction::LLVMIROpcode::DIV:  // RISC-V 指令集架构中，没有专门针对 [除以立即数] 的指令
			if(op1_isreg && op2_isreg){  // reg / reg
                auto rd = GetNewRegister(rd_op->GetRegNo(), INT64);
                auto rs = GetNewRegister(reg_op1->GetRegNo(), INT64);
                auto rt = GetNewRegister(reg_op2->GetRegNo(), INT64);

				auto divw_instr = rvconstructor->ConstructR(RISCV_DIVW, rd, rs, rt);
                cur_block->push_back(divw_instr);
			} else if (op1_isreg && !op2_isreg) { // reg / imm 
                auto rd = GetNewRegister(rd_op->GetRegNo(), INT64);
                auto rs = GetNewRegister(reg_op1->GetRegNo(), INT64);
				Register imm_reg = GetNewTempRegister(INT64);
				InsertImmI32Instruction(imm_reg, imm_op2, cur_block);

				auto divw_instr = rvconstructor->ConstructR(RISCV_DIVW, rd, rs, imm_reg);
				cur_block->push_back(divw_instr);
			} else if (op1_isreg || op2_isreg) {  // imm / reg 
				auto rd = GetNewRegister(rd_op->GetRegNo(), INT64);
				Register imm_reg = GetNewTempRegister(INT64);
				InsertImmI32Instruction(imm_reg, imm_op1, cur_block);
				auto rt = GetNewRegister(reg_op2->GetRegNo(), INT64);
				
				auto divw_instr = rvconstructor->ConstructR(RISCV_DIVW, rd, imm_reg, rt);
				cur_block->push_back(divw_instr);
			} else std::cerr << "Error: divw rd, imm, imm is not allowed" << std::endl;
			break;  

		case BasicInstruction::LLVMIROpcode::MUL:  // RISC-V 指令集架构中，没有专门针对 [乘以立即数] 的指令
			if(op1_isreg && op2_isreg){  // reg * reg
                auto rd = GetNewRegister(rd_op->GetRegNo(), INT64);
                auto rs = GetNewRegister(reg_op1->GetRegNo(), INT64);
                auto rt = GetNewRegister(reg_op2->GetRegNo(), INT64);

				auto mulw_instr = rvconstructor->ConstructR(RISCV_MULW, rd, rs, rt);
                cur_block->push_back(mulw_instr);
			} else if (op1_isreg && !op2_isreg) { // reg * imm 
                auto rd = GetNewRegister(rd_op->GetRegNo(), INT64);
                auto rs = GetNewRegister(reg_op1->GetRegNo(), INT64);
				Register imm_reg = GetNewTempRegister(INT64);
				InsertImmI32Instruction(imm_reg, imm_op2, cur_block);

				auto mulw_instr = rvconstructor->ConstructR(RISCV_MULW, rd, rs, imm_reg);
				cur_block->push_back(mulw_instr);
			} else if (op1_isreg || op2_isreg) {  // imm * reg 
				auto rd = GetNewRegister(rd_op->GetRegNo(), INT64);
				Register imm_reg = GetNewTempRegister(INT64);
				InsertImmI32Instruction(imm_reg, imm_op1, cur_block);
				auto rt = GetNewRegister(reg_op2->GetRegNo(), INT64);
				
				auto mulw_instr = rvconstructor->ConstructR(RISCV_MULW, rd, imm_reg, rt);
				cur_block->push_back(mulw_instr);
			} else std::cerr << "Error: mulw rd, imm, imm is not allowed" << std::endl;
			break;  

		case BasicInstruction::LLVMIROpcode::MOD:
			if(op1_isreg && op2_isreg){  // reg % reg
                auto rd = GetNewRegister(rd_op->GetRegNo(), INT64);
                auto rs = GetNewRegister(reg_op1->GetRegNo(), INT64);
                auto rt = GetNewRegister(reg_op2->GetRegNo(), INT64);

				auto remw_instr = rvconstructor->ConstructR(RISCV_REMW, rd, rs, rt);
                cur_block->push_back(remw_instr);
			} else if (op1_isreg && !op2_isreg) { // reg % imm 
                auto rd = GetNewRegister(rd_op->GetRegNo(), INT64);
                auto rs = GetNewRegister(reg_op1->GetRegNo(), INT64);
				Register imm_reg = GetNewTempRegister(INT64);
				InsertImmI32Instruction(imm_reg, imm_op2, cur_block);

				auto remw_instr = rvconstructor->ConstructR(RISCV_REMW, rd, rs, imm_reg);
				cur_block->push_back(remw_instr);
			} else if (op1_isreg || op2_isreg) {  // imm % reg 
				auto rd = GetNewRegister(rd_op->GetRegNo(), INT64);
				Register imm_reg = GetNewTempRegister(INT64);
				InsertImmI32Instruction(imm_reg, imm_op1, cur_block);
				auto rt = GetNewRegister(reg_op2->GetRegNo(), INT64);
				
				auto remw_instr = rvconstructor->ConstructR(RISCV_REMW, rd, imm_reg, rt);
				cur_block->push_back(remw_instr);
			} else std::cerr << "Error: remw rd, imm, imm is not allowed" << std::endl;
			break;  

		case BasicInstruction::LLVMIROpcode::FADD:
			if(op1_isreg && op2_isreg){  // reg + reg (double)
                auto rd = GetNewRegister(rd_op->GetRegNo(), FLOAT64);
                auto rs = GetNewRegister(reg_op1->GetRegNo(), FLOAT64);
                auto rt = GetNewRegister(reg_op2->GetRegNo(), FLOAT64);

				auto fadd_d_instr = rvconstructor->ConstructR(RISCV_FADD_S, rd, rs, rt);
                cur_block->push_back(fadd_d_instr);
			} else if (op1_isreg && !op2_isreg) { // reg + imm (double)
                auto rd = GetNewRegister(rd_op->GetRegNo(), FLOAT64);
                auto rs = GetNewRegister(reg_op1->GetRegNo(), FLOAT64);
				Register imm_reg = GetNewTempRegister(FLOAT64);
				InsertImmFloat32Instruction(imm_reg, imm_op2, cur_block);

				auto fadd_d_instr = rvconstructor->ConstructR(RISCV_FADD_S, rd, rs, imm_reg);
				cur_block->push_back(fadd_d_instr);
			} else if (op1_isreg || op2_isreg) {  // imm + reg (double)
				auto rd = GetNewRegister(rd_op->GetRegNo(), FLOAT64);
				Register imm_reg = GetNewTempRegister(FLOAT64);
				InsertImmFloat32Instruction(imm_reg, imm_op1, cur_block);
				auto rt = GetNewRegister(reg_op2->GetRegNo(), FLOAT64);
				
				auto fadd_d_instr = rvconstructor->ConstructR(RISCV_FADD_S, rd, imm_reg, rt);
				cur_block->push_back(fadd_d_instr);
			} else std::cerr << "Error: fadd.d rd, imm, imm is not allowed" << std::endl;
			break;  

		case BasicInstruction::LLVMIROpcode::FSUB:
			if(op1_isreg && op2_isreg){  // reg + reg (double)
                auto rd = GetNewRegister(rd_op->GetRegNo(), FLOAT64);
                auto rs = GetNewRegister(reg_op1->GetRegNo(), FLOAT64);
                auto rt = GetNewRegister(reg_op2->GetRegNo(), FLOAT64);

				auto fsub_d_instr = rvconstructor->ConstructR(RISCV_FSUB_S, rd, rs, rt);
                cur_block->push_back(fsub_d_instr);
			} else if (op1_isreg && !op2_isreg) { // reg + imm (double)
                auto rd = GetNewRegister(rd_op->GetRegNo(), FLOAT64);
                auto rs = GetNewRegister(reg_op1->GetRegNo(), FLOAT64);
				Register imm_reg = GetNewTempRegister(FLOAT64);
				InsertImmFloat32Instruction(imm_reg, imm_op2, cur_block);

				auto fsub_d_instr = rvconstructor->ConstructR(RISCV_FSUB_S, rd, rs, imm_reg);
				cur_block->push_back(fsub_d_instr);
			} else if (op1_isreg || op2_isreg) {  // imm - reg (double)
				auto rd = GetNewRegister(rd_op->GetRegNo(), FLOAT64);
				Register imm_reg = GetNewTempRegister(FLOAT64);
				InsertImmFloat32Instruction(imm_reg, imm_op1, cur_block);
				auto rt = GetNewRegister(reg_op2->GetRegNo(), FLOAT64);
				
				auto fsub_d_instr = rvconstructor->ConstructR(RISCV_FSUB_S, rd, imm_reg, rt);
				cur_block->push_back(fsub_d_instr);
			} else std::cerr << "Error: fsub.d rd, imm, imm is not allowed" << std::endl;
			break;  

		case BasicInstruction::LLVMIROpcode::FMUL:
			if(op1_isreg && op2_isreg){  // reg * reg (double)
                auto rd = GetNewRegister(rd_op->GetRegNo(), FLOAT64);
                auto rs = GetNewRegister(reg_op1->GetRegNo(), FLOAT64);
                auto rt = GetNewRegister(reg_op2->GetRegNo(), FLOAT64);

				auto fmul_d_instr = rvconstructor->ConstructR(RISCV_FMUL_S, rd, rs, rt);
                cur_block->push_back(fmul_d_instr);
			} else if (op1_isreg && !op2_isreg) { // reg * imm (double)
                auto rd = GetNewRegister(rd_op->GetRegNo(), FLOAT64);
                auto rs = GetNewRegister(reg_op1->GetRegNo(), FLOAT64);
				Register imm_reg = GetNewTempRegister(FLOAT64);
				InsertImmFloat32Instruction(imm_reg, imm_op2, cur_block);

				auto fmul_d_instr = rvconstructor->ConstructR(RISCV_FMUL_S, rd, rs, imm_reg);
				cur_block->push_back(fmul_d_instr);
			} else if (op1_isreg || op2_isreg) {  // imm * reg (double)
				auto rd = GetNewRegister(rd_op->GetRegNo(), FLOAT64);
				Register imm_reg = GetNewTempRegister(FLOAT64);
				InsertImmFloat32Instruction(imm_reg, imm_op1, cur_block);
				auto rt = GetNewRegister(reg_op2->GetRegNo(), FLOAT64);
				
				auto fmul_d_instr = rvconstructor->ConstructR(RISCV_FMUL_S, rd, imm_reg, rt);
				cur_block->push_back(fmul_d_instr);
			} else std::cerr << "Error: fmul.d rd, imm, imm is not allowed" << std::endl;
			break;  

		case BasicInstruction::LLVMIROpcode::FDIV:
			if(op1_isreg && op2_isreg){  // reg / reg (double)
                auto rd = GetNewRegister(rd_op->GetRegNo(), FLOAT64);
                auto rs = GetNewRegister(reg_op1->GetRegNo(), FLOAT64);
                auto rt = GetNewRegister(reg_op2->GetRegNo(), FLOAT64);

				auto fdiv_d_instr = rvconstructor->ConstructR(RISCV_FDIV_S, rd, rs, rt);
                cur_block->push_back(fdiv_d_instr);
			} else if (op1_isreg && !op2_isreg) { // reg / imm (double)
                auto rd = GetNewRegister(rd_op->GetRegNo(), FLOAT64);
                auto rs = GetNewRegister(reg_op1->GetRegNo(), FLOAT64);
				Register imm_reg = GetNewTempRegister(FLOAT64);
				InsertImmFloat32Instruction(imm_reg, imm_op2, cur_block);

				auto fdiv_d_instr = rvconstructor->ConstructR(RISCV_FDIV_S, rd, rs, imm_reg);
				cur_block->push_back(fdiv_d_instr);
			} else if (op1_isreg || op2_isreg) {  // imm / reg (double)
				auto rd = GetNewRegister(rd_op->GetRegNo(), FLOAT64);
				Register imm_reg = GetNewTempRegister(FLOAT64);
				InsertImmFloat32Instruction(imm_reg, imm_op1, cur_block);
				auto rt = GetNewRegister(reg_op2->GetRegNo(), FLOAT64);
				
				auto fdiv_d_instr = rvconstructor->ConstructR(RISCV_FDIV_S, rd, imm_reg, rt);
				cur_block->push_back(fdiv_d_instr);
			} else std::cerr << "Error: fdiv.d rd, imm, imm is not allowed" << std::endl;
			break;  

		default:
			std::cerr << "Error: undefined opcode." << std::endl;
	}
}
// inst1: %t2 = icmp ne i32 %t1, 0	   	     ====> bne %1, x0, .L3		
// inst2: br i1 %t2, label %B3, label %B4		   j .L4 			#riscv
// Tip: inst1 处的汇编代码生成 .L3 标签，但是这个标签只有指令选择执行到 inst2 的时候才能获取到
// 故需要预存 inst1, 通过关联寄存器 %t2, 使得 inst2 可以回溯到 inst1
template <> void RiscV64Unit::ConvertAndAppend<IcmpInstruction *>(IcmpInstruction *inst) {
    // TODO("Implement this if you need");
    auto res_op = (RegOperand *)inst->GetResult();
    auto res_reg = GetNewRegister(res_op->GetRegNo(), INT64);
    resReg_cmpInst_map[res_reg] = inst;
}

template <> void RiscV64Unit::ConvertAndAppend<FcmpInstruction *>(FcmpInstruction *inst) {
    // TODO("Implement this if you need");
	auto res_op = (RegOperand *)inst->GetResult();
    auto res_reg = GetNewRegister(res_op->GetRegNo(), INT64);
    resReg_cmpInst_map[res_reg] = inst;
}

// globalVar is no need to alloca and not in basicblock,
// globalVar is printed at riscv64_printasm.cc (GlobalVarDefineInstruction)
// localVar is need to alloca a space in stack, a llvm_reg is corresponding to a stack_offset
template <> void RiscV64Unit::ConvertAndAppend<AllocaInstruction *>(AllocaInstruction *inst) {
    // TODO("Implement this if you need");
	auto llvm_reg_op = (RegOperand *)inst->GetResult();

	// clac allocSz
	std::vector<int> dims = inst->GetDims();
	int allcoSz = 4;    // 4 bytes
    for (int dim : dims) allcoSz *= dim;
	
	// update llvmReg_offset_map
	// std::cerr << "offset of reg %r" << llvm_reg_op->GetRegNo() << " is " << cur_offset << std::endl;
	// std::cerr << "offset of reg %r" << llvm_reg_op->GetRegNo() << " is " << cur_offset + bias << std::endl;
    llvmReg_offset_map[llvm_reg_op->GetRegNo()] = cur_offset;
    // llvmReg_offset_map[llvm_reg_op->GetRegNo()] = cur_offset + bias;
    cur_offset += allcoSz;
}

// inst1: %t2 = icmp ne i32 %t1, 0	   	     ====> bne %1, x0, .L3		
// inst2: br i1 %t2, label %B3, label %B4		   j .L4 			#riscv
// Tip: inst1 处的汇编代码生成 .L3 标签，但是这个标签只有指令选择执行到 inst2 的时候才能获取到
// 故需要预存 inst1, 通过关联寄存器 %t2, 使得 inst2 可以回溯到 inst1
template <> void RiscV64Unit::ConvertAndAppend<BrCondInstruction *>(BrCondInstruction *inst) {
    // TODO("Implement this if you need");
    terminal_instr_map[cur_block->getLabelId()] = cur_block->GetInsList().back();
	// special case 
	// br i1 imm, label %B3, label %B4    ====> j .L3   (imm == 1)
	//											j .L4   (imm == 0)  
	// 此处是没有 Icmp 或者 Fcmp的过程的, resReg_cmpInst_map[res_reg] = null_ptr
	if (inst->GetCond()->GetOperandType() == BasicOperand::IMMI32) {
		auto cond_op = (ImmI32Operand *)inst->GetCond();  
		auto imm = cond_op->GetIntImmVal();

		auto true_label = (LabelOperand *)inst->GetTrueLabel();
		auto false_label = (LabelOperand *)inst->GetFalseLabel();

		// j .L3
		if (imm) {
			auto jal_inst = rvconstructor->ConstructJLabel(RISCV_JAL, GetPhysicalReg(RISCV_x0), RiscVLabel(true_label->GetLabelNo(), 0));
			cur_block->push_back(jal_inst);
		} else {
			auto jal_inst = rvconstructor->ConstructJLabel(RISCV_JAL, GetPhysicalReg(RISCV_x0), RiscVLabel(false_label->GetLabelNo(), 0));
			cur_block->push_back(jal_inst);
		}
		return;   
	} else if (inst->GetCond()->GetOperandType() == BasicOperand::IMMF32) {
		auto cond_op = (ImmF32Operand *)inst->GetCond();  
		auto imm = cond_op->GetFloatVal();

		auto true_label = (LabelOperand *)inst->GetTrueLabel();
		auto false_label = (LabelOperand *)inst->GetFalseLabel();

		// j .L3
		if (imm) {
			auto jal_inst = rvconstructor->ConstructJLabel(RISCV_JAL, GetPhysicalReg(RISCV_x0), RiscVLabel(true_label->GetLabelNo(), 0));
			cur_block->push_back(jal_inst);
		} else {
			auto jal_inst = rvconstructor->ConstructJLabel(RISCV_JAL, GetPhysicalReg(RISCV_x0), RiscVLabel(false_label->GetLabelNo(), 0));
			cur_block->push_back(jal_inst);
		}
		return;   
	}

    auto cond_op = (RegOperand *)inst->GetCond();  
    Register res_reg, cond_reg;
	res_reg = cond_reg = GetNewRegister(cond_op->GetRegNo(), INT64);  // res_reg == cond_reg
    auto it = resReg_cmpInst_map.find(res_reg);
	if (it == resReg_cmpInst_map.end()) {
		std::cout << "cmp_inst not found!" << std::endl;
		return;
	} 
	auto cmp_inst = resReg_cmpInst_map[res_reg]; 

    Register op1_reg, op2_reg;
    if (cmp_inst->GetOpcode() == BasicInstruction::LLVMIROpcode::ICMP) {
        auto icmp_inst = (IcmpInstruction *)cmp_inst;
		// solve op1
        if (icmp_inst->GetOp1()->GetOperandType() == BasicOperand::REG) {
			auto op1 = (RegOperand *)icmp_inst->GetOp1();
            op1_reg = GetNewRegister(op1->GetRegNo(), INT64);
        } else if (icmp_inst->GetOp1()->GetOperandType() == BasicOperand::IMMI32) {
			auto op1 = (ImmI32Operand *)icmp_inst->GetOp1();
			op1_reg = GetNewTempRegister(INT64);
			InsertImmI32Instruction(op1_reg, op1, cur_block);
		}
		// solve op2
        if (icmp_inst->GetOp2()->GetOperandType() == BasicOperand::REG) {
			auto op2 = (RegOperand *)icmp_inst->GetOp2();
            op2_reg = GetNewRegister(op2->GetRegNo(), INT64);
        } else if (icmp_inst->GetOp2()->GetOperandType() == BasicOperand::IMMI32) {
			auto op2 = (ImmI32Operand *)icmp_inst->GetOp2();
			op2_reg = GetNewTempRegister(INT64);
			InsertImmI32Instruction(op2_reg, op2, cur_block);
		}

		RISCV_INST opcode = IcmpSelect_map[icmp_inst->GetCond()];
		auto true_label = (LabelOperand *)inst->GetTrueLabel();
		auto false_label = (LabelOperand *)inst->GetFalseLabel();

		// bne %1, x0, .L3
		auto br_inst = rvconstructor->ConstructBLabel(opcode, op1_reg, op2_reg, RiscVLabel(true_label->GetLabelNo(), 0));
		cur_block->push_back(br_inst);
		// j .L4
		auto jal_inst = rvconstructor->ConstructJLabel(RISCV_JAL, GetPhysicalReg(RISCV_x0), RiscVLabel(false_label->GetLabelNo(), 0));
		cur_block->push_back(jal_inst);
	}

	// 对于浮点比较，需要先获取比较结果(set_inst), 再借助形如inst1、inst2的指令进行跳转
	else if (cmp_inst->GetOpcode() == BasicInstruction::LLVMIROpcode::FCMP) {
        auto fcmp_inst = (FcmpInstruction *)cmp_inst;
		// solve op1
        if (fcmp_inst->GetOp1()->GetOperandType() == BasicOperand::REG) {
			auto op1 = (RegOperand *)fcmp_inst->GetOp1();
            op1_reg = GetNewRegister(op1->GetRegNo(), FLOAT64);
        } else if (fcmp_inst->GetOp1()->GetOperandType() == BasicOperand::IMMF32) {
			auto op1 = (ImmF32Operand *)fcmp_inst->GetOp1();
			op1_reg = GetNewTempRegister(FLOAT64);
			InsertImmFloat32Instruction(op1_reg, op1, cur_block);
		}
		// solve op2
        if (fcmp_inst->GetOp2()->GetOperandType() == BasicOperand::REG) {
			auto op2 = (RegOperand *)fcmp_inst->GetOp2();
            op2_reg = GetNewRegister(op2->GetRegNo(), FLOAT64);
        } else if (fcmp_inst->GetOp2()->GetOperandType() == BasicOperand::IMMF32) {
			auto op2 = (ImmF32Operand *)fcmp_inst->GetOp2();
			op2_reg = GetNewTempRegister(FLOAT64);
			InsertImmFloat32Instruction(op2_reg, op2, cur_block);
		}
		// solve res_reg
		if (FcmpSelect_map.find(fcmp_inst->GetCond()) != FcmpSelect_map.end()){
			RISCV_INST opcode = FcmpSelect_map[fcmp_inst->GetCond()];
			RISCV_INST brcode = Not_map[fcmp_inst->GetCond()];
			auto true_label = (LabelOperand *)inst->GetTrueLabel();
			auto false_label = (LabelOperand *)inst->GetFalseLabel();

			// feq.s res_reg, op1, op2 
			auto set_inst = rvconstructor->ConstructR(opcode, res_reg, op1_reg, op2_reg);
			cur_block->push_back(set_inst);
			// beq(bne) res_reg, x0, ture_label
			auto br_ins = rvconstructor->ConstructBLabel(brcode, res_reg, GetPhysicalReg(RISCV_x0), RiscVLabel(true_label->GetLabelNo(), 0));
			cur_block->push_back(br_ins);
			// j false_lable
			auto jal_inst = rvconstructor->ConstructJLabel(RISCV_JAL, GetPhysicalReg(RISCV_x0), RiscVLabel(false_label->GetLabelNo(), 0));
			cur_block->push_back(jal_inst);

		} else std::cerr << "Error: this fcmp_inst is not supported" << std::endl;
	}
    block_terminal_type[inst->GetBlockID()] = 1;
}

template <> void RiscV64Unit::ConvertAndAppend<BrUncondInstruction *>(BrUncondInstruction *inst) {
    terminal_instr_map[cur_block->getLabelId()] = cur_block->GetInsList().back();
    auto dest_label = RiscVLabel(((LabelOperand *)inst->GetDestLabel())->GetLabelNo(), 0);
    auto jal_instr = rvconstructor->ConstructJLabel(RISCV_JAL, GetPhysicalReg(RISCV_x0), dest_label);

    cur_block->push_back(jal_instr);
    block_terminal_type[inst->GetBlockID()] = 0;
}

template <> void RiscV64Unit::ConvertAndAppend<CallInstruction *>(CallInstruction *inst) {
    // 如果函数的参数个数不超过 8 个, 则依次将参数存储在寄存器 a0/fa0 到 a7/fa7 中
    // 否则, 将超出部分放入栈(临时寄存器)
    // 函数返回值放在寄存器 a0/fa0 上
    auto paramlist = inst->GetParameterList();
    int now_int_num = 0;
    int now_float_num = 0;

	// 调用 llvm.memset.p0.i32，将数组的前 10 个字节设置为 42
    // call void @llvm.memset.p0.i32(ptr %ptr, i8 42, i32 10, i1 0)TODO
    if (inst->GetFunctionName() == std::string("llvm.memset.p0.i32")) {
        // slove parameter 0
		auto ptrreg_op = (RegOperand *)inst->GetParameterList()[0].second;
		int ptrreg_no = ptrreg_op->GetRegNo();

		if (llvm2rv_regmap.find(ptrreg_no) != llvm2rv_regmap.end()) { // 不一定memset数组的初始位置 
			// std::cerr << ptrreg_no << std::endl;
			Register ptr_virReg = llvm2rv_regmap[ptrreg_no];
			auto assign_a0_inst = rvconstructor->ConstructR(RISCV_ADD, GetPhysicalReg(RISCV_a0), GetPhysicalReg(RISCV_x0), ptr_virReg);
			cur_block->push_back(assign_a0_inst);
		} else {  // 数组的初始位置 sp + offset
			auto offset = llvmReg_offset_map[ptrreg_no];
            auto offset_reg = GetNewTempRegister(INT64);
            InsertImmI32Instruction(offset_reg, new ImmI32Operand(offset), cur_block);
            // InsertImmI32Instruction();
			auto assign_a0_inst = rvconstructor->ConstructR(RISCV_ADD, GetPhysicalReg(RISCV_a0), GetPhysicalReg(RISCV_sp), offset_reg);
			cur_block->push_back(assign_a0_inst);
			((RiscV64Function *)cur_func)->AddAllocaInst(assign_a0_inst);
		}
        
        // slove parameter 1
		auto imm_op1 = (ImmI32Operand *)(inst->GetParameterList()[1].second);
		InsertImmI32Instruction(GetPhysicalReg(RISCV_a1), imm_op1, cur_block);
	
        // slove paramter 2
		if (inst->GetParameterList()[2].second->GetOperandType() == BasicOperand::IMMI32) {
			auto imm_op2 = (ImmI32Operand *)(inst->GetParameterList()[2].second);
			InsertImmI32Instruction(GetPhysicalReg(RISCV_a2), imm_op2, cur_block);
		} else {
			auto SzReg = (RegOperand *)inst->GetParameterList()[2].second;
			int SzReg_no = SzReg->GetRegNo();
			if (llvm2rv_regmap.find(SzReg_no) == llvm2rv_regmap.end()) {
				Register Sz_virReg = llvm2rv_regmap[SzReg_no];
				auto assign_a2_inst = rvconstructor->ConstructR(RISCV_ADD, GetPhysicalReg(RISCV_a2), GetPhysicalReg(RISCV_x0), Sz_virReg);
				cur_block->push_back(assign_a2_inst);
			} else {
				auto offset = llvmReg_offset_map[ptrreg_no];

                auto offset_reg = GetNewTempRegister(INT64);
                InsertImmI32Instruction(offset_reg, new ImmI32Operand(offset), cur_block);
                auto assign_a2_inst = rvconstructor->ConstructR(RISCV_ADD, GetPhysicalReg(RISCV_a2), GetPhysicalReg(RISCV_sp), offset_reg);
				// auto assign_a2_inst = rvconstructor->ConstructIImm(RISCV_ADDI, GetPhysicalReg(RISCV_a2), GetPhysicalReg(RISCV_sp), offset);
				cur_block->push_back(assign_a2_inst);
				((RiscV64Function *)cur_func)->AddAllocaInst(assign_a2_inst);
			}
		}
        
        // call
        cur_block->push_back(rvconstructor->ConstructCall(RISCV_CALL, "memset", 3, 0));
        return;
    }


    for(auto [type, para] : paramlist){
        Register pararegister;
        if(para->GetOperandType() == BasicOperand::IMMI32){
            if(now_int_num < 8){
                pararegister = GetI32A(now_int_num++);
				InsertImmI32Instruction(pararegister, para, cur_block);
            }else{
                // pararegister = GetNewTempRegister(INT64);
				// do nothing but now_int_num++
				now_int_num++;
            }
        }else if(para->GetOperandType() == BasicOperand::IMMF32){
            if(now_float_num < 8){
                pararegister = GetF32A(now_float_num++);
				InsertImmFloat32Instruction(pararegister, para, cur_block);
            }else{
                // pararegister = GetNewTempRegister(FLOAT64);
				// do nothing but now_float_num++
				now_float_num++;
            }
        }else{
            auto reg = (RegOperand*)para;
            auto regno = reg->GetRegNo();
            Register originpara_register;
            if(type == BasicInstruction::I32 || type == BasicInstruction::PTR){
                originpara_register = GetNewRegister(regno, INT64);
                if(now_int_num < 8){
                    pararegister = GetI32A(now_int_num++);
					auto addw_instr = rvconstructor->ConstructR(RISCV_ADD, pararegister, originpara_register, GetPhysicalReg(RISCV_x0));
                	cur_block->push_back(addw_instr);
                }else{
                    // pararegister = GetNewTempRegister(INT64);
					// do nothing but now_int_num++
					now_int_num++;
                }
            }else{
                originpara_register = GetNewRegister(regno, FLOAT64);
                if(now_float_num < 8){
					pararegister = GetF32A(now_float_num++);
					// std::cerr << "pararegister: " << pararegister.get_reg_no() << " originpara_register: " << originpara_register.get_reg_no() << std::endl;
					auto fmvwx_instr = rvconstructor->ConstructR2(RISCV_FMV_S, pararegister, originpara_register);
					cur_block->push_back(fmvwx_instr);
                }else{
                    // pararegister = GetNewTempRegister(FLOAT64);
					// do nothing but now_float_num++
					now_float_num++;
                }
            }
        }
    }

	// solve nr_para more than 8
	/* ---------------------------gobolt 测试程序-----------------------------------
	int foo(int arg1, int arg2,float arg3,int arg4,int arg5,float arg6,
			int arg7,int arg8,int arg9,int arg10, int arg11, int arg12)
	{
		return arg1 + arg2 + arg3 + arg4 + arg5 + arg6 
		     + arg7 + arg8 + arg9 + arg10 + arg11 + arg12;
	}

	int main(){
		foo(1, 2, 3.3, 4, 5, 6.6, 7, 8, 9, 10, 11, 12);
		return 0;
	} 
	---------------------------------------------------------------------------------*/

	/* ---------------------------caller 相关汇编截取 ----------------------------------
        li      a5,12
        sd      a5,8(sp)
        li      a5,11
        sd      a5,0(sp)
        li      a7,10
        li      a6,9
        li      a5,8
        li      a4,7
        fmv.s   fa1,fa4
        li      a3,5
        li      a2,4
        fmv.s   fa0,fa5
        li      a1,2
        li      a0,1
        call    foo(int, int, float, int, int, float, int, int, int, int, int, int)
		调用约定注意点:
	    1. 实际能依靠寄存器传输的最大参数个数为 16 个 (a0-a7, fa0-fa7, 浮点整数各自传输)
		2. 0(sp)存储第一个溢出的参数, 8(sp)存储第二个...
	---------------------------------------------------------------------------------*/


	/* ---------------------------callee 相关汇编截取 ----------------------------------
		; 功能为累加, 逻辑简单直观
		; return arg1 + arg2 + arg3 + arg4 + ... + arg9 + arg10 + arg11 + arg12;
		
	    lw      a5,-20(s0)   ; arg1  a0 (reg)
        mv      a4,a5
        lw      a5,-24(s0)   ; arg2  a1 (reg)
        addw    a5,a4,a5
        sext.w  a5,a5
        fcvt.s.w        fa4,a5
        flw     fa5,-28(s0)  ; arg3  fa0 (reg)
        fadd.s  fa4,fa4,fa5
        lw      a5,-32(s0)   ; arg4  a2 (reg)
        fcvt.s.w        fa5,a5
        fadd.s  fa4,fa4,fa5
        lw      a5,-36(s0)   ; arg5  a3 (reg)
        fcvt.s.w        fa5,a5
        fadd.s  fa4,fa4,fa5
        flw     fa5,-40(s0)  ; arg6  fa1 (reg)
        fadd.s  fa4,fa4,fa5
        lw      a5,-44(s0)   ; arg7  a4 (reg)
        fcvt.s.w        fa5,a5
        fadd.s  fa4,fa4,fa5
        lw      a5,-48(s0)   ; arg8  a5 (reg)
        fcvt.s.w        fa5,a5
        fadd.s  fa4,fa4,fa5
        lw      a5,-52(s0)   ; arg9  a6 (reg)
        fcvt.s.w        fa5,a5
        fadd.s  fa4,fa4,fa5
        lw      a5,-56(s0)   ; arg10 a7 (reg)
        fcvt.s.w        fa5,a5
        fadd.s  fa4,fa4,fa5
        lw      a5,0(s0)     ; arg11 (stack)
        fcvt.s.w        fa5,a5
        fadd.s  fa4,fa4,fa5
        lw      a5,8(s0)     ; arg12 (stack)
        fcvt.s.w        fa5,a5
        fadd.s  fa5,fa4,fa5
        fcvt.w.s a5,fa5,rtz
        sext.w  a5,a5
		调用约定注意点:
	    1. 参数传入之后作为临时变量, 预先存储在栈中 (lowerframe)
		2. 溢出的参数需要通过栈底s0, 也就是原先的栈顶来查找, 如arg11, arg12 
		   (lowerstack 也许需要存储栈底fp/s0)
	---------------------------------------------------------------------------------*/
	
	int nr_stkpara = std::max(0, now_int_num - 8) + std::max(0, now_float_num - 8);
	// std::cerr << now_int_num << " " << now_float_num << std::endl;

	if (nr_stkpara != 0) {
		now_int_num = now_float_num = 0;
        int arg_off = 0;
        for (auto [type, arg_op] : inst->GetParameterList()) {
			Assert(InsertArgInStack(type, arg_op, arg_off, cur_block, now_int_num, now_float_num) == 0);
        }
    }
	// 开辟参数空间
	cur_func->UpdateParaSize(nr_stkpara * 8);
	// std::cerr << "UpdateParaSize to " << nr_stkpara * 8 << std::endl;

    auto call_instr = rvconstructor->ConstructCall(RISCV_CALL, 
												   inst->GetFunctionName(), 
												   std::min(8, now_int_num), 
												   std::min(8, now_float_num));
    cur_block->push_back(call_instr);

	// solve return value
    if(inst->GetResult() != nullptr){
        auto resultop = inst->GetResult();
        auto resultreg = (RegOperand*)resultop;
        auto resultreno = resultreg->GetRegNo();
        Register resultregister;
        if(inst->GetRetType() == BasicInstruction::I32 || inst->GetRetType() == BasicInstruction::PTR){
            resultregister = GetNewRegister(resultreno, INT64);
            auto addw_instr = rvconstructor->ConstructR(RISCV_ADDW, resultregister, GetPhysicalReg(RISCV_a0), GetPhysicalReg(RISCV_x0));
            cur_block->push_back(addw_instr);
        }else{
            resultregister = GetNewRegister(resultreno, FLOAT64);
            // std::cerr<<resultregister.get_reg_no()<<'\n';
            auto fmvwx_instr = rvconstructor->ConstructR2(RISCV_FMV_S, resultregister, GetPhysicalReg(RISCV_fa0));
            cur_block->push_back(fmvwx_instr);
        }
    }

}

template <> void RiscV64Unit::ConvertAndAppend<RetInstruction *>(RetInstruction *inst) {
    terminal_instr_map[cur_block->getLabelId()] = cur_block->GetInsList().back();
	// a0 寄存器设置为返回值
    if (inst->GetRetVal() != NULL) {
        if (inst->GetRetVal()->GetOperandType() == BasicOperand::IMMI32) {  		   // return imm i32
            InsertImmI32Instruction(GetPhysicalReg(RISCV_a0), inst->GetRetVal(), cur_block); 
        } else if (inst->GetRetVal()->GetOperandType() == BasicOperand::IMMF32) {   // return imm f32
            InsertImmFloat32Instruction(GetPhysicalReg(RISCV_fa0), inst->GetRetVal(), cur_block);
        } else if (inst->GetRetVal()->GetOperandType() == BasicOperand::REG) {
            if (inst->GetType() == BasicInstruction::LLVMType::I32) { 			   // return reg i32
                auto retreg_val = (RegOperand *)inst->GetRetVal();
                auto reg = GetNewRegister(retreg_val->GetRegNo(), INT64);

                auto retcopy_instr = rvconstructor->ConstructR(RISCV_ADD, GetPhysicalReg(RISCV_a0), reg, GetPhysicalReg(RISCV_x0));
                cur_block->push_back(retcopy_instr);
            } else if (inst->GetType() == BasicInstruction::LLVMType::FLOAT32) {	   // return reg f32
                auto retreg_val = (RegOperand *)inst->GetRetVal();
                auto reg = GetNewRegister(retreg_val->GetRegNo(), FLOAT64);
                auto retcopy_instr = rvconstructor->ConstructR2(RISCV_FMV_S, GetPhysicalReg(RISCV_fa0), reg);
                cur_block->push_back(retcopy_instr);
            }
        }
    }

    auto ret_instr = rvconstructor->ConstructIImm(RISCV_JALR, GetPhysicalReg(RISCV_x0), GetPhysicalReg(RISCV_ra), 0);
    if (inst->GetType() == BasicInstruction::I32) {
        ret_instr->setRetType(1);
    } else if (inst->GetType() == BasicInstruction::FLOAT32) {
        ret_instr->setRetType(2);
    } else {
        ret_instr->setRetType(0);
    }
    cur_block->push_back(ret_instr);
    block_terminal_type[inst->GetBlockID()] = 0;
}
/* (1) type transformer imm to reg
 *  %t1 = fptosi float 3.14 to i32  ===> LI %temp, 3.14  # li imm should smaller than 2^12 (InsertImmFloat32Instruction)
 * 										 FCVT.W.S %1, %temp  
 * 
 * (2) type transformer reg to reg
 *  %t1 = sitofp i32 %t2 to float   ===> FCVT.S.W %1, %2  
*/
template <> void RiscV64Unit::ConvertAndAppend<FptosiInstruction *>(FptosiInstruction *inst) {
    // TODO("Implement this if you need");
    auto dst_op = (RegOperand *)inst->GetResult();
    if (inst->GetSrc()->GetOperandType() == BasicOperand::REG) {
        auto src_op = (RegOperand *)inst->GetSrc();
		Register src_reg = GetNewRegister(src_op->GetRegNo(), FLOAT64);
		Register dst_reg = GetNewRegister(dst_op->GetRegNo(), INT64);

		auto fcvt_w_s = rvconstructor->ConstructR2(RISCV_FCVT_W_S, dst_reg, src_reg);
        cur_block->push_back(fcvt_w_s);
    } else if (inst->GetSrc()->GetOperandType() == BasicOperand::IMMF32) {
        auto src_op = (ImmF32Operand *)inst->GetSrc();
		Register src_reg = GetNewTempRegister(FLOAT64);
		InsertImmFloat32Instruction(src_reg, src_op, cur_block);
		Register dst_reg = GetNewRegister(dst_op->GetRegNo(), INT64);

		auto fcvt_w_s = rvconstructor->ConstructR2(RISCV_FCVT_W_S, dst_reg, src_reg);
        cur_block->push_back(fcvt_w_s);
	}
}

// SitofpInstruction is same as FptosiInstruction.
template <> void RiscV64Unit::ConvertAndAppend<SitofpInstruction *>(SitofpInstruction *inst) {
    // TODO("Implement this if you need");
	auto dst_op = (RegOperand *)inst->GetResult();
    if (inst->GetSrc()->GetOperandType() == BasicOperand::REG) {
        auto src_op = (RegOperand *)inst->GetSrc();
		Register src_reg = GetNewRegister(src_op->GetRegNo(), INT64);
		Register dst_reg = GetNewRegister(dst_op->GetRegNo(), FLOAT64);

		auto fcvt_s_w = rvconstructor->ConstructR2(RISCV_FCVT_S_W, dst_reg, src_reg);
        cur_block->push_back(fcvt_s_w);
    } else if (inst->GetSrc()->GetOperandType() == BasicOperand::IMMI32) {
        auto src_op = (ImmI32Operand *)inst->GetSrc();
		Register src_reg = GetNewTempRegister(INT64);
		InsertImmI32Instruction(src_reg, src_op, cur_block);
		Register dst_reg = GetNewRegister(dst_op->GetRegNo(), FLOAT64);
		auto fcvt_s_w = rvconstructor->ConstructR2(RISCV_FCVT_S_W, dst_reg, src_reg);
        cur_block->push_back(fcvt_s_w);
	}
}


/* !a 特殊情况 （icmp / fcmp 不一定只和 cond br 绑定，也有可能和符号扩展绑定）
 * inst1: %r3 = icmp eq i32 %r2,0 	  ===>    sltu  %r3, zero, %r2  # 如果 %r2 != 0，%r3 = 1；否则 %r3 = 0   负数也有效
 *                                            xori  %r3, %r3, 1     # 对 %r3 取反：如果 %r2 == 0，%r3 = 1；否则 %r3 = 0 	  
 * inst2: zext i1 %r3 to i32		
 * 在第二条指令进行选择时，第二条指令负责生成第一条指令的riscv形式 (第二条指令无需翻译)
 * */
template <> void RiscV64Unit::ConvertAndAppend<ZextInstruction *>(ZextInstruction *inst) {

	// special case : %r13 = zext i1 0 to i32
	if(inst->GetSrc()->GetOperandType() == BasicOperand::IMMI32) {
		auto srcop = (ImmI32Operand *)inst->GetSrc();
		auto res_reg = GetNewTempRegister(INT64);
        llvm2rv_regmap[((RegOperand*)inst->GetResult())->GetRegNo()] = res_reg;
		InsertImmI32Instruction(res_reg, srcop, cur_block);
		return;
	} else if(inst->GetSrc()->GetOperandType() == BasicOperand::IMMF32){
		auto srcop = (ImmF32Operand *)inst->GetSrc();
		auto res_reg = GetNewTempRegister(FLOAT64);
        llvm2rv_regmap[((RegOperand*)inst->GetResult())->GetRegNo()] = res_reg;
		InsertImmFloat32Instruction(res_reg, srcop, cur_block);
		return;
	}
	/*
	 * %cmp1 = icmp eq i32 %x, %y    ->    subw t0, x1, x2      
	 * 								       sltiu t1, t0, 1      						
	 * %cmp2 = icmp ne i32 %x, %y    ->    subw t0, x1, x2      
	 * 								       sltu t1, zero, t0      
	 * %cmp3 = icmp ugt i32 %x, %y   ->   
	 * %cmp4 = icmp uge i32 %x, %y   ->    
	 * %cmp5 = icmp ult i32 %x, %y   ->   
	 * %cmp6 = icmp ule i32 %x, %y   ->    
	 * %cmp7 = icmp sgt i32 %x, %y   ->    slt t0, x2, x1 
	 * %cmp8 = icmp sge i32 %x, %y   ->    slt t0, x1, x2
	 *  								   xori t1, t0, 1
	 * %cmp9 = icmp slt i32 %x, %y   ->    slt t0, x1, x2
	 * %cmp10 = icmp sle i32 %x, %y  ->    slt t0, x2, x1
	 * 									   xori t1, t0, 1
	*/
	auto srcop = inst->GetSrc();
	auto srcreg = (RegOperand*)srcop;
	auto srcregno = srcreg->GetRegNo();
	auto srcregister = GetNewRegister(srcregno, INT64);

	auto resultop = inst->GetResult();
	auto resultreg = (RegOperand*)resultop;
	auto resultregno = resultreg->GetRegNo();
	auto resultregister = GetNewRegister(resultregno, INT64);
	

	auto cmp_inst = resReg_cmpInst_map[srcregister];
	Register op1_reg, op2_reg;
	auto tempregister = GetNewTempRegister(INT64);


    if(inst->GetFromType() == BasicInstruction::I1 && inst->GetToType() == BasicInstruction::I32){
		if (cmp_inst->GetOpcode() == BasicInstruction::LLVMIROpcode::ICMP) {
			auto icmp_inst = (IcmpInstruction *)cmp_inst;
			auto cond = icmp_inst->GetCond();
            // std::cerr<<icmp_inst->GetOp1()->GetFullName()<<" "<<icmp_inst->GetOp2()->GetFullName()<<'\n';
			// solve op1
			if (icmp_inst->GetOp1()->GetOperandType() == BasicOperand::REG) {
				auto op1 = (RegOperand *)icmp_inst->GetOp1();
				op1_reg = GetNewRegister(op1->GetRegNo(), INT64);
                // inst->PrintIR(std::cerr);
			} else if (icmp_inst->GetOp1()->GetOperandType() == BasicOperand::IMMI32) {
				auto op1 = (ImmI32Operand *)icmp_inst->GetOp1();
				op1_reg = GetNewTempRegister(INT64);
                
				InsertImmI32Instruction(op1_reg, op1, cur_block);
			}
			// solve op2
			if (icmp_inst->GetOp2()->GetOperandType() == BasicOperand::REG) {
				auto op2 = (RegOperand *)icmp_inst->GetOp2();
				op2_reg = GetNewRegister(op2->GetRegNo(), INT64);
			} else if (icmp_inst->GetOp2()->GetOperandType() == BasicOperand::IMMI32) {
				auto op2 = (ImmI32Operand *)icmp_inst->GetOp2();
				op2_reg = GetNewTempRegister(INT64);
				InsertImmI32Instruction(op2_reg, op2, cur_block);
			}
			// gen code by cond
			switch (cond)
			{
				case BasicInstruction::IcmpCond::eq: 
				{
					auto subw_inst = rvconstructor->ConstructR(RISCV_SUBW, srcregister, op1_reg, op2_reg);
                    // std::cerr<<"eq = "<<srcregister.get_reg_no()<<'\n';
					auto sltu_inst = rvconstructor->ConstructIImm(RISCV_SLTIU, resultregister, srcregister, 1);
					cur_block->push_back(subw_inst);
					cur_block->push_back(sltu_inst);
					break;
				}
				case BasicInstruction::IcmpCond::ne:
				{
					auto subw_inst = rvconstructor->ConstructR(RISCV_SUBW, srcregister, op1_reg, op2_reg);
					auto sltu_inst = rvconstructor->ConstructR(RISCV_SLTU, resultregister, GetPhysicalReg(RISCV_x0), srcregister);
					cur_block->push_back(subw_inst);
					cur_block->push_back(sltu_inst);
					break;
				}
				case BasicInstruction::IcmpCond::sgt:
				{
					auto sltu_inst = rvconstructor->ConstructR(RISCV_SLT, resultregister, op2_reg, op1_reg);
					cur_block->push_back(sltu_inst);
					break;
				}
				case BasicInstruction::IcmpCond::sge:
				{
					auto slt_inst = rvconstructor->ConstructR(RISCV_SLT, srcregister, op1_reg, op2_reg);
					auto xori_inst = rvconstructor->ConstructIImm(RISCV_XORI, resultregister, srcregister, 1);
					cur_block->push_back(slt_inst);
					cur_block->push_back(xori_inst);
					break;
				}
				case BasicInstruction::IcmpCond::slt:
				{
					auto slt_inst = rvconstructor->ConstructR(RISCV_SLT, resultregister, op1_reg, op2_reg);
					cur_block->push_back(slt_inst);
					break;
				}
				case BasicInstruction::IcmpCond::sle:
				{
					auto slt_inst = rvconstructor->ConstructR(RISCV_SLT, srcregister, op2_reg, op1_reg);
					auto xori_inst = rvconstructor->ConstructIImm(RISCV_XORI, resultregister, srcregister, 1);
					cur_block->push_back(slt_inst);
					cur_block->push_back(xori_inst);
					break;
				}
				default:
				{
					std::cerr << "this icmpCond is not support at Zext." << std::endl;
					break;
				}
			}
		}

    }else{
        assert("Wrong Type Converse in ZextInstruction");
    }
}

template <> void RiscV64Unit::ConvertAndAppend<GetElementptrInstruction *>(GetElementptrInstruction *inst) {
    // TODO("Implement this if you need");
	// for (auto arg_reg : cur_func->GetParameters()){
	// 	std::cerr << arg_reg.get_reg_no() << std::endl;
	// }
    auto base_op = inst->GetPtrVal();
    // 1. 计算getelementptr的地址偏移
    auto dims = inst->GetDims();
    auto indexes = inst->GetIndexes();
    auto siz = indexes.size();
    int base = 4;
    int dimmax_base = 1;
    for(size_t i = 0; i < dims.size(); ++i){
		// std::cerr << dims[i] << std::endl;
        dimmax_base *= dims[i];
    }
    // std::cerr<<dims.size()<<'\n';
    Register last_register = GetPhysicalReg(RISCV_x0);
    for(size_t i = 0; i < siz; ++i){
        auto indexop = indexes[i];
        // auto dimnum = dims[i];
        // std::cerr<<i<<" : "<<indexop->GetFullName()<<" "<<dimnum<<" "<<dimmax_base<<'\n';
        bool isZeroFlag = indexop->GetOperandType() == BasicOperand::IMMI32 && ((ImmI32Operand*)indexop)->GetIntImmVal() == 0;
        if(!isZeroFlag){
            // 1.1 获取当前偏移基量, 存入 temp_offset_register
            auto temp_offset_register = GetNewTempRegister(INT64);
            InsertImmI32Instruction(temp_offset_register, new ImmI32Operand(dimmax_base), cur_block);
            Register AddRegister;
            // 1.2 获得下标 AddRegister
            if(indexop->GetOperandType() == BasicOperand::REG){
                auto indexregno = ((RegOperand*)indexop)->GetRegNo();
                AddRegister = GetNewRegister(indexregno, INT32);
				// std::cerr << "下标是一个寄存器 %" << AddRegister.get_reg_no() << std::endl;
            }else{
                // 立即数
				// std::cerr << "下标是一个立即数" << std::endl;
                auto imm = ((ImmI32Operand*)indexop)->GetIntImmVal();
                AddRegister = GetNewTempRegister(INT64);
                InsertImmI32Instruction(AddRegister, new ImmI32Operand(imm), cur_block);
            }
            // 1.3 算出实际偏移量 = 下标 * 偏移基量 tempresult_register
            auto mul_register = GetNewTempRegister(INT64);
            auto mul_instr = rvconstructor->ConstructR(RISCV_MUL, mul_register, AddRegister, temp_offset_register);
            cur_block->push_back(mul_instr);
            // 1.4 将实际偏移量加到总偏移量中
            if(last_register == GetPhysicalReg(RISCV_x0)){
                last_register = mul_register;
            }else{
                auto new_sum_register = GetNewTempRegister(INT64);
                // std::cerr<<"here : "<<new_sum_register.get_reg_no()<<"\n";
                auto add_instr = rvconstructor->ConstructR(RISCV_ADD, new_sum_register, mul_register, last_register);
                cur_block->push_back(add_instr);
                last_register = new_sum_register;
            }
        }
        if(i < dims.size()){
            dimmax_base /= dims[i];
        }
    }
    // 1.5 最终偏移量 = 总和偏移量 << 2
    Register final_offset_register;
    final_offset_register = GetNewTempRegister(INT64);
    auto slli_instr = rvconstructor->ConstructIImm(RISCV_SLLI, final_offset_register, last_register, 2);
    cur_block->push_back(slli_instr);
    
    // 2. 计算 base 地址(sp + offset)
    // 如果是函数参数, 则不需要计算
    if(base_op->GetOperandType() == BasicOperand::REG){
        // 局部数组
        auto base_regno = ((RegOperand*)base_op)->GetRegNo();
        auto offset_reg = GetNewTempRegister(INT64);
        if(llvm2rv_regmap.find(base_regno) != llvm2rv_regmap.end()){
			auto resultop = inst->GetResult();
            auto resultregno = ((RegOperand*)resultop)->GetRegNo();
            auto resultregister = GetNewRegister(resultregno, INT64);
            auto add_instr = rvconstructor->ConstructR(RISCV_ADD, resultregister, GetNewRegister(base_regno, INT64), final_offset_register);
            cur_block->push_back(add_instr);
        }else{
			Register addi_register = GetNewTempRegister(INT64);
            InsertImmI32Instruction(offset_reg, new ImmI32Operand(llvmReg_offset_map[base_regno]), cur_block);
            auto addi_instr = rvconstructor->ConstructR(RISCV_ADD, addi_register, GetPhysicalReg(RISCV_sp), offset_reg);
            cur_block->push_back(addi_instr);
			((RiscV64Function *)cur_func)->AddAllocaInst(addi_instr);
            auto resultop = inst->GetResult();
            auto resultregno = ((RegOperand*)resultop)->GetRegNo();
            auto resultregister = GetNewRegister(resultregno, INT64);
            auto add_instr = rvconstructor->ConstructR(RISCV_ADD, resultregister, addi_register, final_offset_register);
            cur_block->push_back(add_instr);
        }
        
    }else{
        // 全局数组
        GlobalOperand* global_op = (GlobalOperand *)inst->GetPtrVal();
        auto lui_register = GetNewTempRegister(INT64);
        auto lui_instr = rvconstructor->ConstructULabel(RISCV_LUI, lui_register, RiscVLabel(global_op->GetName(), true));
        cur_block->push_back(lui_instr);
        Register addi_register = GetNewTempRegister(INT64);
        auto addi_instr = rvconstructor->ConstructILabel(RISCV_ADDI, addi_register, lui_register, RiscVLabel(global_op->GetName(), false));
        cur_block->push_back(addi_instr);
        auto resultop = inst->GetResult();
        auto resultregno = ((RegOperand*)resultop)->GetRegNo();
        auto add_instr = rvconstructor->ConstructR(RISCV_ADD, GetNewRegister(resultregno, INT64), final_offset_register, addi_register);
        cur_block->push_back(add_instr);
    }
    
}
template <> void RiscV64Unit::ConvertAndAppend<PhiInstruction *>(PhiInstruction *inst) {
    // TODO("Implement this if you need");
    // https://www.cnblogs.com/lixingyang/p/17721341.html
    // 1. 使用并查集构建 phi 网络, 对每个 phi 网络构建一个临时寄存器
    // 2. 遍历 phi 网络里的每一个phi, 在前驱基本块 philist.label 存取数值 philist.val 进临时寄存器, 在 phi 指令处取出
    auto resultop = inst->GetResult();
    auto resultreg = (RegOperand*)resultop;
    auto resultregno = resultreg->GetRegNo();
    auto rootphiseq = phi_temp_phiseqmap[resultregno];
    // std::cerr<<resultregno<<" "<<rootphiseq<<'\n';
    if(phi_temp_registermap.find(rootphiseq) == phi_temp_registermap.end()){
        if(inst->GetResultType() == BasicInstruction::I32 || inst->GetResultType() == BasicInstruction::PTR){
            phi_temp_registermap[rootphiseq] = GetNewTempRegister(INT64);
        }else{
            phi_temp_registermap[rootphiseq] = GetNewTempRegister(FLOAT64);
        }
        mem2reg_destruc_set.insert(phi_temp_registermap[rootphiseq]);
        // llvm2rv_regmap[resultreg->GetRegNo()] = phi_temp_registermap[rootphiseq];
    }

    // inst->PrintIR(std::cerr);
    auto tempregister = phi_temp_registermap[rootphiseq];
    for(auto [labelop, valop] : inst->GetPhiList()){
        auto labelno = ((LabelOperand*)labelop)->GetLabelNo();
        auto cur_mcfg = cur_func->getMachineCFG();
        auto bb = (cur_mcfg->block_map)[labelno];
        auto bblist = bb->Mblock->GetInsList();
        // MachineBaseInstruction* terminalI = nullptr;
        // MachineBaseInstruction* terminalI2 = nullptr;
        // if(bblist.size() > 0){
        //     terminalI = bblist.back();
        //     bb->Mblock->pop_back();
        // }
        // if(bblist.size() > 1 && block_terminal_type[labelno]){
        //     terminalI2 = *(----bblist.end());
        //     bb->Mblock->pop_back();
        // }
        MachineBaseInstruction *add_instr;
        // "do right here"
        if(inst->GetResultType() == BasicInstruction::I32 || inst->GetResultType() == BasicInstruction::PTR){
            // inst->PrintIR(std::cerr);
            // std::cerr<<inst->GetResultType()<<" "<<valop->GetFullName()<<" "<<valop->GetOperandType()<<'\n';
            if(valop->GetOperandType() == BasicOperand::REG){
                auto valreg = (RegOperand*)valop;
                auto valregno = valreg->GetRegNo();
                auto reg_register = GetNewRegister(valregno, INT64);
                add_instr = rvconstructor->ConstructR(RISCV_ADD, tempregister, reg_register, GetPhysicalReg(RISCV_x0));
            }else{
                auto valimm = (ImmI32Operand*)valop;
                auto imm = valimm->GetIntImmVal();
                auto temp_imm_register = GetNewTempRegister(INT64);
                InsertImmI32Instruction(temp_imm_register, valimm, bb->Mblock, 1);
                add_instr = rvconstructor->ConstructR(RISCV_ADD, tempregister, temp_imm_register, GetPhysicalReg(RISCV_x0));
            }
        }else/* if(inst->GetResultType() == BasicInstruction::FLOAT32)*/{
            if(valop->GetOperandType() == BasicOperand::REG){
                auto valreg = (RegOperand*)valop;
                auto valregno = valreg->GetRegNo();
                auto reg_register = GetNewRegister(valregno, FLOAT64);
                add_instr = rvconstructor->ConstructR2(RISCV_FMV_S, tempregister, reg_register);
            }else{
                auto valimm = (ImmF32Operand*)valop;
                auto imm = valimm->GetFloatVal();
                auto temp_imm_register = GetNewTempRegister(FLOAT64);
                InsertImmFloat32Instruction(temp_imm_register, valimm, bb->Mblock, 1);
                add_instr = rvconstructor->ConstructR2(RISCV_FMV_S, tempregister, temp_imm_register);
            }
        }
        // 将生成的指令放在所有基本块都生成后再生成
        phi_instr_map[bb->Mblock->getLabelId()].push_back(add_instr);
    }

    if(inst->GetResultType() == BasicInstruction::I32 || inst->GetResultType() == BasicInstruction::PTR){
        auto newreg = GetNewRegister(resultregno, INT64);
        llvm2rv_regmap[resultregno] = newreg;
        auto add_instr = rvconstructor->ConstructR(RISCV_ADD, newreg, tempregister, GetPhysicalReg(RISCV_x0));
        cur_block->push_back(add_instr);
    }else{
        auto newreg = GetNewRegister(resultregno, FLOAT64);
        llvm2rv_regmap[resultregno] = newreg;
        auto add_instr = rvconstructor->ConstructR2(RISCV_FMV_S, newreg, tempregister);
        cur_block->push_back(add_instr);
    }
    // std::cerr<<llvm2rv_regmap[resultreg->GetRegNo()].get_reg_no()<<'\n';
}



void RiscV64Unit::BuildPhiWeb(CFG *C){
    std::map<int, int> UnionFindMap;
    std::function<int(int)> UnionFind = [&](int RegToFindNo) -> int {
        if (UnionFindMap[RegToFindNo] == RegToFindNo)
            return RegToFindNo;
        return UnionFindMap[RegToFindNo] = UnionFind(UnionFindMap[RegToFindNo]);
    };
    auto Connect = [&](Operand resultOp, Operand replaceOp) -> void {
        auto Reg1 = (RegOperand *)resultOp;
        auto Reg1no = Reg1->GetRegNo();
        auto Reg0 = (RegOperand *)replaceOp;
        auto Reg0no = Reg0->GetRegNo();
        UnionFindMap[UnionFind(Reg1no)] = UnionFind(Reg0no);
    };
    for (int i = 0; i <= C->max_reg; ++i) {
        UnionFindMap[i] = i;
    }
    phiseq = -1;
    for(auto [id, bb] : *C->block_map){
        for(auto I : bb->Instruction_list){
            if(I->GetOpcode() != BasicInstruction::PHI){
                break;
            }
            auto phiI = (PhiInstruction*)I;
            auto resultop = phiI->GetResult();
            auto resultreg = (RegOperand*)resultop;
            auto resultregno =resultreg->GetRegNo();
            for(auto [labelop, valop] : phiI->GetPhiList()){
                if(valop->GetOperandType() != BasicOperand::REG){
                    continue;
                }
                auto valreg = (RegOperand*)valop;
                auto valregno = valreg->GetRegNo();
                Connect(resultop, valop);
            }
        }
    }

    for(auto [id, bb] : *C->block_map){
        for(auto I : bb->Instruction_list){
            if(I->GetOpcode() != BasicInstruction::PHI){
                break;
            }
            // I->PrintIR(std::cerr);
            auto phiI = (PhiInstruction*)I;
            auto resultop = phiI->GetResult();
            auto resultreg = (RegOperand*)resultop;
            auto resultregno =resultreg->GetRegNo();
            // auto rootregno = UnionFind(resultregno);
            auto rootregno = resultregno;
            if(phi_temp_phiseqmap.find(rootregno) == phi_temp_phiseqmap.end()){
                phiseq++;
                phi_temp_phiseqmap[rootregno] = phiseq;
                // std::cerr<<resultregno<<" "<<rootregno<<" "<<phiseq<<" "<<phi_temp_phiseqmap[rootregno]<<'\n';
            }
            phi_temp_phiseqmap[resultregno] = phi_temp_phiseqmap[rootregno];
			// std::cerr<<resultregno<<" "<<rootregno<<" "<<phiseq<<" "<<phi_temp_phiseqmap[rootregno]<<'\n';
        }
    }
}
