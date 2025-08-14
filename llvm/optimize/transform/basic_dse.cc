// // 包含控制流图（CFG）相关的头文件
// #include "../../include/cfg.h"
// // 包含别名分析相关的头文件
// #include "../analysis/AliasAnalysis.h"
// // 包含内存依赖分析相关的头文件
// #include "../analysis/memdep.h"
// // 包含函数对象相关的标准库，用于后续的lambda表达式
// #include <functional>

// // 注释说明：需先运行dom.invExecute，再运行此pass，之后再运行dom.Execute
// // 外部声明别名分析器（全局变量）
// extern AliasAnalyser *alias_analyser;
// // 外部声明内存依赖分析器（全局变量）
// extern MemoryDependenceAnalyser *memdep_analyser;

// /**
//  * 获取控制流图（CFG）中的返回块（包含RET指令的基本块）
//  * @param C 控制流图指针
//  * @return 包含RET指令的基本块，若未找到则返回nullptr
//  */
// LLVMBlock GetRetBlock(CFG* C)
// {
//     // 获取基本块映射表的大小
//     int size=C->block_map->size();
//     // 遍历所有基本块（id为基本块标识，bb为基本块指针）
//     for(auto [id,bb]:*C->block_map)
//     {
//         // 遍历当前基本块中的所有指令
//         for(auto ins:bb->Instruction_list)
//         {
//             // 检查指令是否为RET（返回）指令
//             if(ins->GetOpcode() == BasicInstruction::LLVMIROpcode::RET)
//             {
//                 // 找到返回块，返回该基本块
//                 return bb;
//             }
//         }
//     }
//     // 未找到返回块，返回空指针
//     return nullptr;
// }

// /**
//  * 基本块内的死存储消除（仅处理基本块内的冗余存储）
//  * @param C 控制流图指针
//  */
// void BasicBlockDSE(CFG *C) {
//     // 用于存储待删除的指令集合
//     std::set<Instruction> EraseSet;
//     // 遍历所有基本块
//     for (auto [id, bb] : *C->block_map) {
//         // 映射表：键为操作数（存储地址），值为该地址对应的存储指令列表
//         std::map<Operand, std::vector<Instruction>> storeptrs_map;

//         // 从基本块的指令列表末尾向前遍历（反向遍历便于检测冗余存储）
//         for (int i = bb->Instruction_list.size() - 1; i >= 0; --i) {
//             // 获取当前指令
//             auto I = bb->Instruction_list[i];
//             // 检查是否为存储（STORE）指令
//             if (I->GetOpcode() == BasicInstruction::STORE) {
//                 // 将当前指令转换为存储指令类型
//                 auto StoreI = (StoreInstruction *)I;
//                 // 获取存储指令的目标地址（指针）
//                 auto ptr = StoreI->GetPointer();
//                 // 检查该地址是否已有存储指令记录
//                 if (storeptrs_map.find(ptr) != storeptrs_map.end()) {
//                     // 遍历该地址已有的存储指令
//                     for (auto oldI : storeptrs_map[ptr]) {
//                         // 若内存依赖分析器判断当前存储使旧存储冗余（同一地址且无中间使用）
//                         if (memdep_analyser->isStoreBeUsedSame(I, oldI, C)) {
//                             // 将当前存储指令加入待删除集合
//                             EraseSet.insert(I);
//                             // 找到冗余后跳出循环（无需检查更早的存储）
//                             break;
//                         }
//                     }
//                 } else {
//                     // 该地址首次出现，记录当前存储指令
//                     storeptrs_map[ptr].push_back(StoreI);
//                 }
//             }
//         }
//     }

//     // 从所有基本块中移除待删除的指令
//     for (auto [id, bb] : *C->block_map) {
//         // 临时保存原指令列表
//         auto tmp_Instruction_list = bb->Instruction_list;
//         // 清空原指令列表
//         bb->Instruction_list.clear();
//         // 遍历临时指令列表
//         for (auto I : tmp_Instruction_list) {
//             // 若指令不在待删除集合中，则保留
//             if (EraseSet.find(I) != EraseSet.end()) {
//                 continue;
//             }
//             // 将保留的指令重新插入基本块
//             bb->InsertInstruction(1, I);
//         }
//     }
// }

// /**
//  * 判断指令I1执行后是否可能执行到I2（I1是否能到达I2）
//  * @param I1 起始指令
//  * @param I2 目标指令
//  * @param C 控制流图指针
//  * @return 若能到达则返回true，否则返回false
//  */
// static bool CanReach(Instruction I1, Instruction I2, CFG *C) {
//     // 获取I1所在基本块的ID
//     auto bb1_id = I1->GetBlockID();
//     // 获取I2所在基本块的ID
//     auto bb2_id = I2->GetBlockID();
//     // 若两指令在同一基本块，则返回false（此处逻辑为排除同块情况）
//     if (bb1_id == bb2_id) {
//         return false;
//     }

//     // 用于标记基本块是否已访问
//     std::vector<int> vis;
//     // 用于BFS遍历的队列
//     std::queue<int> q;

//     // 初始化访问标记数组（大小为最大基本块标签+1）
//     vis.resize(C->max_label + 1);
//     // 将I1所在基本块ID加入队列，开始BFS
//     q.push(bb1_id);

//     // BFS遍历控制流图
//     while (!q.empty()) {
//         // 取出队首基本块ID
//         auto x = q.front();
//         q.pop();
//         // 若到达I2所在基本块，返回true
//         if (x == bb2_id) {
//             return true;
//         }
//         // 若已访问过该基本块，跳过
//         if (vis[x]) {
//             continue;
//         }
//         // 标记为已访问
//         vis[x] = true;

//         // 遍历当前基本块的所有后继基本块，加入队列
//         for (auto bb : C->GetSuccessor(x)) {
//             q.push(bb->block_id);
//         }
//     }
//     // 遍历结束仍未到达I2所在基本块，返回false
//     return false;
// }

// /**
//  * 基于后支配树遍历的死存储消除
//  * @param C 控制流图指针
//  */
// void PostDomTreeWalkDSE(CFG *C) {
//     // 用于存储待删除的指令集合
//     std::set<Instruction> EraseSet;
//     // 映射表：键为操作数（存储地址），值为该地址对应的存储指令列表
//     std::map<Operand, std::vector<Instruction>> storeptrs_map;

//     // 定义DFS函数（用于遍历后支配树），捕获外部变量
//     std::function<void(int)> dfs = [&](int bbid) {
//         // 临时映射表：记录当前基本块为每个地址添加的存储指令数量
//         std::map<Operand, int> tmp_map;
//         // 获取当前基本块
//         LLVMBlock now = (*C->block_map)[bbid];
//         // 从基本块的指令列表末尾向前遍历
//         for (int i = now->Instruction_list.size() - 1; i >= 0; --i) {
//             // 获取当前指令
//             auto I = now->Instruction_list[i];
//             // 检查是否为存储指令
//             if (I->GetOpcode() == BasicInstruction::STORE) {
//                 // 标记当前存储是否可被消除
//                 bool is_dse = false;
//                 // 转换为存储指令类型
//                 auto StoreI = (StoreInstruction *)I;
//                 // 获取存储地址
//                 auto ptr = StoreI->GetPointer();
//                 // 检查该地址是否已有存储指令记录
//                 if (storeptrs_map.find(ptr) != storeptrs_map.end()) {
//                     // 遍历该地址已有的存储指令
//                     for (auto oldI : storeptrs_map[ptr]) {
//                         // 若内存依赖分析器判断两存储冗余，且当前存储无法到达旧存储
//                         if (memdep_analyser->isStoreBeUsedSame(I, oldI, C) && !CanReach(I, oldI, C)) {
//                             // 将当前存储加入待删除集合
//                             EraseSet.insert(I);
//                             is_dse = true;
//                             // 找到冗余后跳出循环
//                             break;
//                         }
//                     }
//                 }
//                 // 若当前存储可被消除，则不记录该指令
//                 if (is_dse) {
//                     continue;
//                 }
//                 // 记录当前地址新增的存储指令数量
//                 tmp_map[ptr] += 1;
//                 // 将当前存储指令加入全局映射表
//                 storeptrs_map[ptr].push_back(StoreI);
//             }
//         }
        
//         // 递归遍历后支配树的子节点（后支配树的后继节点）
//         for (auto v : C->PostDomTree->dom_tree[bbid]) {
//             dfs(v->block_id);
//         }

//         // 回溯：移除当前基本块添加的存储指令（恢复全局映射表状态）
//         for (auto [op, num] : tmp_map) {
//             for (int i = 0; i < num; ++i) {
//                 storeptrs_map[op].pop_back();
//             }
//         }
//     };
//     // 获取返回块作为后支配树遍历的起点
//     auto ret_block=GetRetBlock(C);
//     // 从返回块开始DFS遍历后支配树
//     dfs(ret_block->block_id);

//     // 从所有基本块中移除待删除的指令（与BasicBlockDSE逻辑相同）
//     for (auto [id, bb] : *C->block_map) {
//         auto tmp_Instruction_list = bb->Instruction_list;
//         bb->Instruction_list.clear();
//         for (auto I : tmp_Instruction_list) {
//             if (EraseSet.find(I) != EraseSet.end()) {
//                 continue;
//             }
//             bb->InsertInstruction(1, I);
//         }
//     }
// }

// /**
//  * 消除未被使用的存储指令
//  * @param C 控制流图指针
//  */
// void EliminateNotUsedStore(CFG *C) {
//     // 用于存储待删除的指令集合
//     std::set<Instruction> EraseSet;

//     // 遍历所有基本块
//     for (auto [id, bb] : *C->block_map) {
//         // 遍历基本块中的所有指令
//         for (auto I : bb->Instruction_list) {
//             // 只处理存储指令
//             if (I->GetOpcode() != BasicInstruction::STORE) {
//                 continue;
//             }
//             // 转换为存储指令类型
//             auto StoreI = (StoreInstruction *)I;
//             // 获取存储地址
//             auto ptr = StoreI->GetPointer();
//             // 注释说明：在SysY2022中，所有全局变量都是静态的，因此可消除无用的全局存储
//             // 若地址是局部指针，或当前函数是main函数
//             if (alias_analyser->is_localptrs(C, ptr) || C->function_def->GetFunctionName() == "main") {
//                 // 若内存依赖分析器判断该存储未被使用
//                 if (memdep_analyser->isStoreNotUsed(I, C)) {
//                     // 将该存储指令加入待删除集合
//                     EraseSet.insert(I);
//                 }
//             }
//         }
//     }

//     // 从所有基本块中移除待删除的指令（与前面的消除逻辑相同）
//     for (auto [id, bb] : *C->block_map) {
//         auto tmp_Instruction_list = bb->Instruction_list;
//         bb->Instruction_list.clear();
//         for (auto I : tmp_Instruction_list) {
//             if (EraseSet.find(I) != EraseSet.end()) {
//                 continue;
//             }
//             bb->InsertInstruction(1, I);
//         }
//     }
// }

// /**
//  * 简单死存储消除的入口函数，依次调用三种DSE优化
//  * @param C 控制流图指针
//  */
// void SimpleDSE(CFG *C) {
//     // 为每个指令设置其所在基本块的ID（用于后续分析）
//     for (auto [id, bb] : *C->block_map) {
//         for (auto I : bb->Instruction_list) {
//             I->SetBlockID(id);
//         }
//     }

//     // 1. 执行基本块内的死存储消除
//     BasicBlockDSE(C);
//     // 2. 执行基于后支配树的死存储消除
//     PostDomTreeWalkDSE(C);
//     // 3. 执行未被使用的存储消除
//     EliminateNotUsedStore(C);
// }