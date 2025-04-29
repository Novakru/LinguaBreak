#ifndef DOMINATOR_TREE_H
#define DOMINATOR_TREE_H
#include "../../include/ir.h"
#include "../pass.h"
#include <set>
#include <vector>

class DominatorTree {
public:
    CFG *C;
    std::vector<std::vector<LLVMBlock>> dom_tree{};
    std::vector<LLVMBlock> idom{};
    std::map<int, int> sdom_map{};      // 半支配点的map(block_id-->半支配点block_id)
    std::map<int, int> dfs_map{};       // 深度优先搜索的map(搜索序号-->block_id)
    std::map<int, int> dfs{};           // 用来记录反图的dfs辅助变量
    std::map<int, std::set<int>> DF_map{};  // 记录每个块作为cfg节点的支配边界

    DominatorTree(CFG *cfg) : C(cfg) {}
    void BuildDominatorTree(bool reverse = false);    // build the dominator tree of CFG* C
    //std::set<int> GetDF(std::set<int> S);             // return DF(S)  S = {id1,id2,id3,...}
    std::set<int> GetDF(int id);                      // return DF(id)
    //bool IsDominate(int id1, int id2);                // if blockid1 dominate blockid2, return true, else return false
    void SearchInvB(int bbid);                        // 用于递归获取反图dfs
    
    int find(std::map<int,int>&mn_map, std::map<int,int>&fa_map, int id);
    int invfind(std::map<int,int>&mn_map, std::map<int,int>&fa_map, int id);
};

class DomAnalysis : public IRPass {
private:
    std::map<CFG *, DominatorTree *> DomInfo;

public:
    DomAnalysis(LLVMIR *IR) : IRPass(IR) {}
    void Execute();
    void invExecute();
    DominatorTree *GetDomTree(CFG *C) { return DomInfo[C]; }
};
#endif