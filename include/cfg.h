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

    LLVMBlock ret_block;

    FuncDefInstruction function_def;
    // FunctionBasicInfo FunctionInfo;

    // NaturalLoopForest loopforest;

    /*this is the pointer to the value of LLVMIR.function_block_map
      you can see it in the LLVMIR::CFGInit()*/
    std::map<int, LLVMBlock> *block_map;

    // 使用邻接表存图
    std::vector<std::vector<LLVMBlock>> G{};       // control flow graph
    std::vector<std::vector<LLVMBlock>> invG{};    // inverse control flow graph

	// 通过LLVMIR获取支配树森林调用不方便，CFG单独存储支配树，方便调用且可以和G、invG同步更新
	void* DomTree;
    void* PostDomTree;

    void BuildCFG();
    void BuildDominatorTree();
    // void BuildFunctionInfo();
    // void BuildLoopInfo();
    LLVMBlock GetBlock(int bbid);
    LLVMBlock NewBlock();

    // 获取某个基本块节点的前驱/后继
    std::vector<LLVMBlock> GetPredecessor(LLVMBlock B);
    std::vector<LLVMBlock> GetPredecessor(int bbid);
    std::vector<LLVMBlock> GetSuccessor(LLVMBlock B);
    std::vector<LLVMBlock> GetSuccessor(int bbid);
    
    // 展示图G和逆图invG
    void Display();
};

#endif