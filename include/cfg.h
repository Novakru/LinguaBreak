#ifndef CFG_H
#define CFG_H

#include "ast.h"
#include "basic_block.h"
// #include "../optimize/analysis/function_basicinfo.h"
// #include "../optimize/analysis/loop_analysis.h"
// #include "../optimize/analysis/dominator_tree.h"   // 循环调用问题
#include <bitset>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <vector>


class DominatorTree;

class CFG {
public:

    int max_reg = 0;
    int max_label = 0;
    FuncDefInstruction function_def;
    std::unordered_map<int, std::pair<int,Instruction>> def_instr_map; // SSA 的value只定义一次。result_regno -> block_id,def_Instruction

    /*this is the pointer to the value of LLVMIR.function_block_map
      you can see it in the LLVMIR::CFGInit()*/
    std::map<int, LLVMBlock> *block_map;

    // 使用邻接表存图
    std::unordered_map<int,std::set<LLVMBlock>> G{};       // control flow graph
    std::unordered_map<int,std::set<LLVMBlock>> invG{};    // inverse control flow graph
    std::unordered_map<int,std::set<int>> SSA_Graph; // value_regno->use_instruction's defregno  同一图内未必连通

	  // 通过LLVMIR获取支配树森林调用不方便，CFG单独存储支配树，方便调用且可以和G、invG同步更新
	  void* DomTree;
    void* PostDomTree;

    void SearchB(LLVMBlock B); // 辅助函数
    void BuildCFG();

    // 获取某个基本块节点的前驱/后继
    std::set<LLVMBlock> GetPredecessor(LLVMBlock B);
    std::set<LLVMBlock> GetPredecessor(int bbid);
    std::set<LLVMBlock> GetSuccessor(LLVMBlock B);
    std::set<LLVMBlock> GetSuccessor(int bbid);
    void GetSSAGraphAllSucc(std::set<int>& succs,int regno);

	void display(bool reverse = false);
};

#endif