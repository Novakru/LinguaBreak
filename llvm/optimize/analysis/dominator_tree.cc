#include "dominator_tree.h"
#include "../../include/ir.h"

int inv_dfs_num = 0;

void DomAnalysis::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        DomInfo[cfg] = new DominatorTree(cfg);
        DomInfo[cfg]->BuildDominatorTree(false);
        cfg->DomTree = DomInfo[cfg];
    }
}

void DomAnalysis::invExecute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        DomInfo[cfg] = new DominatorTree(cfg);
        DomInfo[cfg]->BuildDominatorTree(true);
        cfg->PostDomTree = DomInfo[cfg];
    }
}

int DominatorTree::find(std::map<int,int>&mn_map, std::map<int,int> &fa_map, int id){
    if(fa_map[id]==id)return id;
    int temp = fa_map[id];
    fa_map[id] = find(mn_map, fa_map, fa_map[id]); // 路径压缩

    int fa_sdom = sdom_map[mn_map[temp]];
    int id_sdom = sdom_map[mn_map[id]];
    if((*(C->block_map))[fa_sdom]->dfs_id < (*(C->block_map))[id_sdom]->dfs_id){
        mn_map[id] = mn_map[temp];
    }
    return fa_map[id];
}

int DominatorTree::invfind(std::map<int,int>&mn_map, std::map<int,int> &fa_map, int id){
    if(fa_map[id]==id)return id;
    int temp = fa_map[id];
    fa_map[id] = invfind(mn_map, fa_map, fa_map[id]); // 路径压缩

    int fa_sdom = sdom_map[mn_map[temp]];
    int id_sdom = sdom_map[mn_map[id]];
    if(dfs[(*(C->block_map))[fa_sdom]->block_id] < dfs[(*(C->block_map))[id_sdom]->block_id]){
        mn_map[id] = mn_map[temp];
    }
    return fa_map[id];
}

void DominatorTree::SearchInvB(int bbid){
    if(dfs[bbid]!=-1) return;
    dfs[bbid] = ++inv_dfs_num;
    //std::cout<<bbid<<" "<<inv_dfs_num<<std::endl;

    for(int i=0; i<C->invG[bbid].size(); i++){
        SearchInvB(C->invG[bbid][i]->block_id);
    }
}

void DominatorTree::BuildDominatorTree(bool reverse) {
    // 初始化 dom_tree
    dom_tree.clear();
    dom_tree.resize(C->block_map->size());
    for (auto [id, block] : *(C->block_map)) {
        dom_tree[id] = std::vector<LLVMBlock>();
    }
    
    if(!reverse){
        std::map<int, int> fa_map{};  // 这是用于记录路径压缩的祖宗节点的map
        std::map<int, int> mn_map{};  // 这是用于记录一个block顺着逆向图目前可以找到的最小sdom的block_id

        // 遍历原图，获取dfs序号-->block_id的对应map
        // 初始化辅助map
        for(auto iter = C->block_map->begin(); iter != C->block_map->end(); iter++){
            int block_id = iter->first;
            LLVMBlock block = iter->second;

            dfs_map[block->dfs_id] = block_id;
            sdom_map[block_id] = block_id;
            fa_map[block_id] = block_id;
            mn_map[block_id] = block_id;
        }
        
        // 按照dfs的逆序遍历和invG遍历以获取idom
        std::map<int, int>::reverse_iterator iter;
        for(iter=dfs_map.rbegin(); iter!=dfs_map.rend(); iter++){
            int dfn = iter->first;
            int block_id = iter->second;
            if(block_id==0) continue;

            std::vector<LLVMBlock> vec = C->invG[block_id];
            int res = INT32_MAX;

            for(int i=0; i<vec.size(); i++){
                if(dfn > vec[i]->dfs_id){
                    res = std::min(res, vec[i]->dfs_id);
                }
                else{
                    find(mn_map, fa_map, vec[i]->block_id);
                    int sdom = sdom_map[mn_map[vec[i]->block_id]];
                    res = std::min(res, (*(C->block_map))[sdom]->dfs_id);
                }
            }
            sdom_map[block_id] = dfs_map[res];
            fa_map[block_id] = block_id==0?0:dfs_map[dfn-1];
        }

        // debug
        for(auto &[block_id, block]: *(C->block_map)){
            block->comment += (" idom:" + std::to_string(sdom_map[block_id]));
        }

        // 建立DF_map
        for(int i=0; i<C->invG.size(); i++){
            if(C->invG[i].size()>=2){
                for(int j=0; j<C->invG[i].size(); j++){
                    int runner = C->invG[i][j]->block_id;
                    while(runner!=sdom_map[i]){
                        DF_map[runner].insert(i);
                        (*(C->block_map))[runner]->comment += " DF:" + std::to_string(i);
                        if(runner==0)break;
                        runner = sdom_map[runner];
                    }
                }
            }
        }
    }
    else{
        std::set<int> inv_start; // 反转图的起点（其实就是cfg的exit）

        // 寻找结束代码块
        for(int i=0; i<C->G.size(); i++){
            if(C->G[i].size()==0){
                inv_start.insert(i);
            }
            dfs[i]=-1; //初始化作为未访问标记
        }

        // 计算反图的dfs
        for(auto& start: inv_start){
            SearchInvB(start);
        }
        
        std::map<int, int> fa_map{};  // 这是用于记录路径压缩的祖宗节点的map
        std::map<int, int> mn_map{};  // 这是用于记录一个block顺着逆向图目前可以找到的最小sdom的block_id

        // 遍历原图，获取dfs序号-->block_id的对应map
        // 初始化辅助map
        for(auto iter = C->block_map->begin(); iter != C->block_map->end(); iter++){
            int block_id = iter->first;
            LLVMBlock block = iter->second;

            dfs_map[dfs[block_id]] = block_id;
            sdom_map[block_id] = block_id;
            fa_map[block_id] = block_id;
            mn_map[block_id] = block_id;
        }
        
        // 按照dfs的逆序遍历和invG遍历以获取idom（这里的idom在带权并查集其实就是sdom）
        std::map<int, int>::reverse_iterator iter;
        for(iter=dfs_map.rbegin(); iter!=dfs_map.rend(); iter++){
            int dfn = iter->first;
            int block_id = iter->second;
            std::vector<LLVMBlock> vec = C->G[block_id];
            int res = INT32_MAX;

            for(int i=0; i<vec.size(); i++){
                if(dfn > dfs[vec[i]->block_id]){
                    res = std::min(res, dfs[vec[i]->block_id]);
                }
                else{
                    invfind(mn_map, fa_map, vec[i]->block_id);
                    int sdom = sdom_map[mn_map[vec[i]->block_id]];
                    res = std::min(res, dfs[(*(C->block_map))[sdom]->block_id]);
                }
            }
            sdom_map[block_id] = dfs_map[res];
            fa_map[block_id] = block_id==0?0:dfs_map[dfn-1];
        }

        // debug
        for(auto &[block_id, block]: *(C->block_map)){
            block->comment += (" inv_idom:" + std::to_string(sdom_map[block_id]));
        }

        // 建立DF_map
        for(int i=0; i<C->G.size(); i++){
            if(C->G[i].size()>=2){
                for(int j=0; j<C->G[i].size(); j++){
                    int runner = C->G[i][j]->block_id;
                    while(runner!=sdom_map[i]){
                        DF_map[runner].insert(i);
                        (*(C->block_map))[runner]->comment += " inv_DF:" + std::to_string(i);
                        if(inv_start.find(runner)!=inv_start.end())
                            break;
                        runner = sdom_map[runner];
                    }
                }
            }
        }
    }

    // 根据半支配点构建支配树
    std::set<int> visited; // 用于记录已经处理过的块
    for(auto iter = C->block_map->begin(); iter != C->block_map->end(); iter++){
        int block_id = iter->first;
        if(block_id == 0 || visited.count(block_id) > 0) continue; // 跳过入口块和已处理的块
        
        // 将当前块加入其半支配点的支配子树中
        dom_tree[sdom_map[block_id]].push_back(iter->second);
        visited.insert(block_id);
    }
    
}


std::set<int> DominatorTree::GetDF(int id) {
    return DF_map[id];
}

void DominatorTree::display() {
    std::cout << "\n=== Dominator Tree Structure ===" << std::endl;
    for (size_t i = 0; i < dom_tree.size(); i++) {
        if (dom_tree[i].empty()) continue;
        std::cout << "Block " << i << " dominates: ";
        for (auto block : dom_tree[i]) {
            std::cout << block->block_id << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "=== End of Dominator Tree ===\n" << std::endl;
}
