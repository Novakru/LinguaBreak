/********************************************************
 * Algorithm 4: Semidominators
 * from https://www.cs.au.dk/~gerth/advising/thesis/henrik-knakkegaard-christensen.pdf
 ********************************************************/
/* 1:   Create a DFS tree T  
 * 2:   Set semi(v) = v for each v ∈ V  
 * 3:   For each v ∈ V - {r}, in reverse preorder by the DFS  
 * 4:       For each q ∈ pred_G(v)  
 * 5:           z ← eval(q)  // Find the candidate semidominator of q  
 * 6:           If semi(z) < semi(v), then set semi(v) ← semi(z)  
 * 7:       End for loop  
 * 8:   Link v and parent_T(v)  
 * 9:   End for loop */


/****************************************************************************************
 * Algorithm 5: Lengauer-Tarjan Dominator Tree Algorithm
 * from https://www.cs.au.dk/~gerth/advising/thesis/henrik-knakkegaard-christensen.pdf
 ****************************************************************************************/
/* 1:   Create a DFS tree T  
 * 2:   For each w ∈ V - {r}, in reverse preorder by the DFS  
 * 3:   Compute the semidominator for w  
 * 4:   Add w to the bucket of semi(w)  
 * 5:   While the bucket of parent(w) is not empty, do the following  
 * 6:       Pop one element v from the bucket  
 * 7:       u ← eval(v)  // Find the candidate semidominator of v  
 * 8:       If semi(u) < semi(v), then  
 * 9:           idom(v) ← u  // Set immediate dominator of v to u  
 * 10:      Else  
 * 11:          idom(v) ← semi(v)  // Set immediate dominator of v to semi(v)  
 * 12:      End if  
 * 13:  End while loop  
 * 14:  End for loop  
 * 15:  For each w ∈ V - {r}, in preorder by the DFS  
 * 16:  If idom(w) ≠ semi(w), then  
 * 17:      idom(w) ← idom(idom(w))  // Update immediate dominator of w to idom(idom(w))  
 * 18:  End if  
 * 19:  End for loop */


// Code reference from https://blog.atal.moe/post/dom-tree.html

/* The only thing worth learning from this article is DSU (Union-find). 
 * In lines 0-12 of Algorithm 5, he did not explain his logic, and the conclusion in the article does not conform to his logic
 * So make modifications and understanding to match the original paper pseudo code
 * Any modifications have been annotated
 */


#include "dominator_tree.h"
#include "../../include/ir.h"
#include <algorithm>  

int dfc, tot, u, v;
std::vector<int> fa, fth, pos, mn, idm, sdm, dfn, sdom;
const int INF = 2e9;

void init(int max_label){
	dfc = tot = u = v = 0;
    fa.clear();
    fth.clear();
    pos.clear();
    mn.clear();
    idm.clear();
    sdm.clear();
    dfn.clear();
    sdom.clear();

    fa.resize(max_label + 2);
    fth.resize(max_label + 2);
    pos.resize(max_label + 2);
    mn.resize(max_label + 2);
    idm.resize(max_label + 2);
    sdm.resize(max_label + 2);
    dfn.resize(max_label + 2);
    sdom.resize(max_label + 2);
}

void add(std::vector<std::vector<LLVMBlock>>& graph, LLVMBlock u, LLVMBlock v) {
    graph[u->block_id].push_back(v);
}



/* The semi-dominator sdom(u) of a node u is the smallest node v such that there exists a path
* from v to u, where every node on the path (except u and v) is greater than u.
* Formally, the semi-dominator sdom(u) of u is defined as:
* sdom(u) = min(v | ∃v = v0 → v1 → ... → vk = u, ∀1 ≤ i ≤ k - 1, vi > u)
*/

// In the Union-Find (Disjoint Set) algorithm, the condition for two nodes to belong
// to the same equivalence class is whether they are part of the same subtree in the DFS tree.
//      
// Notice: 	(1) At the beginning of the algorithm, each node corresponds to a class
// 		   	(2) nodes are added in reverse order of traversal
// 			(3) Folding only occurs when a node finds a node larger than its dfn


int find(int x) {
    if (fa[x] == x) { return x; }
	int tmp = fa[x];
    fa[x] = find(fa[x]);
    if (dfn[sdm[mn[tmp]]] < dfn[sdm[mn[x]]]) { mn[x] = mn[tmp]; }
    return fa[x];
}

void DominatorTree::BuildDF_table(std::vector<std::vector<LLVMBlock>> invG){
	DF_table.resize(C->max_label + 1);
	// std::cout << "Inverse Control Flow Graph (invG):" << std::endl;
	for(int id = 0; id <= C->max_label; id++) {
		bool is_multiPred = (invG[id].size() > 1);
		if(!is_multiPred) continue;
		for(LLVMBlock p : invG[id]) {
			LLVMBlock runner = p;
			
			// 严格支配节点（strictly dominator）: if d != n && d dom n, 则d严格支配n, 记d sdom n。
			// 所以 d 不严格支配 d, 即 df(d) 中可能会存在 d
			int cnt = 0;
			while(runner != idom[id] && cnt <= 100){
				cnt++;
				DF_table[runner->block_id].insert(id);
				runner = idom[runner->block_id];
			}
		}
	}
}

void DominatorTree::GetDfn(LLVMBlock u, std::vector<std::vector<LLVMBlock>> G) {
	dfn[u->block_id] = ++dfc; pos[dfc] = u->block_id;
	for (LLVMBlock v : G[u->block_id]) {
		if (!dfn[v->block_id]) { GetDfn(v, G); fth[v->block_id] = u->block_id; }
	}	
}

void DomAnalysis::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
		PostDomInfo[cfg].C = cfg;          
        PostDomInfo[cfg].BuildPostDominatorTree();
		PostDomInfo[cfg].BuildDF_table(cfg->G);
        cfg->PostDomTree = &PostDomInfo[cfg];

		DomInfo[cfg].C = cfg;           // assign cfg to domtree
        DomInfo[cfg].BuildDominatorTree();
		DomInfo[cfg].BuildDF_table(cfg->invG);
        cfg->DomTree = &DomInfo[cfg];
        // std::cerr<<DomInfo[cfg].dom_tree.size()<<'\n';
        // DomInfo[cfg].Display();

		// cfg->Display();
		// DomInfo[cfg].Display();

		// print DF_table
		// cfg->Display();
		// PostDomInfo[cfg].Display();
		// for (int id = 0; id < DomInfo[cfg].DF_table.size(); id++) {
		// 	std::cout << "DF(" << id << ") = { ";
		// 	for (int node : DomInfo[cfg].DF_table[id]) {
		// 		std::cout << node << " ";
		// 	}
		// 	std::cout << "}" << std::endl;
		// }
    }
}

void PostDominatorTree::BuildPostDominatorTree(){
	BuildDominatorTree(true);
}

void CFG::BuildDominatorTree(){
    auto domTree = (DominatorTree*)DomTree;
    domTree->BuildDominatorTree();
    auto postdomTree = (PostDominatorTree*)PostDomTree;
    postdomTree->BuildPostDominatorTree();
}


void DominatorTree::BuildDominatorTree(bool reverse) { // default reverse = false

	if (C == nullptr) {
		std::cerr << "Error: CFG NULL" << std::endl;
		return;
	}
	
	auto const G    = (reverse) ? C->invG : C->G ;
	auto const invG = (reverse) ? C->G 	: C->invG;
	LLVMBlock blockSrc = (reverse) ? C->ret_block : (*C->block_map)[0];
    idom.clear();
	dom_tree.clear();
	dom_tree.resize(C->max_label + 2, std::vector<LLVMBlock>{});
	
	init(C->max_label); 
    GetDfn(blockSrc, G);

    for (int i = 1; i <= C->max_label + 1; ++i) { fa[i] = sdm[i] = mn[i] = i; }
    for (int i = dfc; i >= 2; --i) {
        int u = pos[i], res = INF;
        for (LLVMBlock v : invG[u]) {
			// Directly use the definition to solve
            find(v->block_id);
			// res = std::min(res, dfn[sdm[mn[v->block_id]]]);

			// use Theorem 1 to solve (https://blog.atal.moe/post/dom-tree.html)
 			// sdom(u) = min({v | 存在边 v → u 且 v < u} ∪ {sdom(w) | w > u 且存在路径 w → ... → v → u})
            if (dfn[v->block_id] < dfn[u]) { res = std::min(res, dfn[v->block_id]); }
            else { res = std::min(res, dfn[sdm[mn[v->block_id]]]); }

        }
        sdm[u] = pos[res]; fa[u] = fth[u]; add(dom_tree, (*C->block_map)[sdm[u]], (*C->block_map)[u]); u = fth[u];
        for (LLVMBlock v : dom_tree[u]) {
            find(v->block_id);
            if (sdm[mn[v->block_id]] == sdm[u]) { idm[v->block_id] = sdm[u]; }
            else {idm[v->block_id] = sdm[mn[v->block_id]]; }

			// if (u == sdm[mn[v]]) { idm[v] = u; } else { idm[v] = mn[v]; }
        }
    }
    for (int i = 2; i <= dfc; ++i) {
        int u = pos[i];
        if (idm[u] != sdm[u]) { idm[u] = idm[idm[u]]; }
    }

	for (int i = 0; i <= C->max_label; ++i) {
	    idom.push_back((*C->block_map)[idm[i]]);
	}

	this->SortDominatorTree();

	// this->Display();
	// std::cerr << "Test - IsDominate(0, C->max_label - 1) : " << IsDominate(0, C->max_label - 1) << std::endl;
	// std::cerr << "Test - IsDominate(2, 9) : " << IsDominate(2, 9) << std::endl;
	// std::cerr << "Test - IsDominate(8, 9) : " << IsDominate(8, 9) << std::endl;
	// std::cerr << "Test - IsDominate(14, 6) : " << IsDominate(14, 6) << std::endl;
	// std::cerr << "Test - IsDominate(6, 14) : " << IsDominate(6, 14) << std::endl;

 }

// 对于一个节点集合 S，其支配边界 DF(S) 是指所有节点的集合，这些节点满足以下条件：
// 1. 它们是 S 中某个节点的支配边界。
// 2. 它们不被 S 中的任何节点严格支配。
std::set<int> DominatorTree::GetDF(std::set<int> S) { 
    std::set<int> result;

    // 合并 S 中每个节点的支配边界
    for (int id : S) {
        std::set_union(result.begin(), result.end(),
                       DF_table[id].begin(), DF_table[id].end(),
                       std::inserter(result, result.begin()));
    }

    // 过滤掉被 S 中节点严格支配的节点
    for (int id : S) {
        for (int rid : result) {
            if (IsDominate(id, rid)) {
                result.erase(rid);
            } 
        }
    }

    return result;
}

std::set<int> DominatorTree::GetDF(int id) {
	return DF_table[id];
}


bool compareByBlockid(const LLVMBlock& a, const LLVMBlock& b) {
    return a->block_id < b->block_id;
}


void DominatorTree::SortDominatorTree() {
	for(std::vector<LLVMBlock> &lst : dom_tree){
		std::sort(lst.begin(), lst.end(), compareByBlockid);
	}
}

// a dom b, b dom c => a dom c
bool DominatorTree::IsDominate(int id1, int id2) { 
	assert(id1 <= C->max_label && id2 <= C->max_label);
	if(id1 == 0) return true;

    int current = id2;
    while (current != 0) {
        if (current == id1) {
            return true; 
        }
        current = idom[current]->block_id;
    }
    return false;
}

void DominatorTree::Display() {
    std::cout << "DominatorTree (dom_tree):" << std::endl;
    for (int i = 0; i < dom_tree.size(); ++i) {
        std::cout << "Block " << i << " -> ";
        for (const auto& succ : dom_tree[i]) {
            std::cout << succ->block_id << " ";
        }
        std::cout << std::endl;
    }
}