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
    // std::vector<std::vector<LLVMBlock>> post_dom_tree{};
    std::vector<LLVMBlock> idom{};
	std::vector<std::set<int>> DF_table;              // table of Dominance Frontier

	void GetDfn(LLVMBlock u, std::vector<std::vector<LLVMBlock>> G);                         
    void BuildDominatorTree(bool reverse = false);    // build the dominator tree of CFG* C
	void SortDominatorTree();                         // make CFG from list[0]->7->3-> to list[0]->2->3->7 
    void BuildDF_table(std::vector<std::vector<LLVMBlock>> invG);    
    std::set<int> GetDF(std::set<int> S);             // return DF(S)  S = {id1,id2,id3,...}
    std::set<int> GetDF(int id);                      // return DF(id)
    bool IsDominate(int id1, int id2);                // if blockid1 dominate blockid2, return true, else return false
	void Display();                                   // display dom_tree

    // TODO(): add or modify functions and members if you need
};

class PostDominatorTree : public DominatorTree{
	public:
	void BuildPostDominatorTree();
};

class DomAnalysis : public IRPass {
private:
    std::map<CFG *, DominatorTree> DomInfo;
	std::map<CFG *, PostDominatorTree> PostDomInfo;

public:
    DomAnalysis(LLVMIR *IR) : IRPass(IR) {}
    void Execute();
    DominatorTree *GetDomTree(CFG *C) { return &DomInfo[C]; }
	DominatorTree *GetPostDomTree(CFG *C) { return &PostDomInfo[C]; }

    // TODO(): add more functions and members if you need
};
#endif