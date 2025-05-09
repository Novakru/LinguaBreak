#include "simplify_cfg.h"

void SimplifyCFGPass::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        EliminateUnreachedBlocksInsts(cfg);
		cfg->BuildCFG();
    }
}

// 删除从函数入口开始到达不了的基本块和指令
void SimplifyCFGPass::EliminateUnreachedBlocksInsts(CFG *C) {
    for (auto it = C->block_map->begin(); it != C->block_map->end(); ) {
        if (!(it->second->dfs_id)) {
            it = C->block_map->erase(it); // 删除元素并更新迭代器
        } else {
            ++it; // 仅在未删除元素时移向下一个
        }
    }
}

// 合并基本块：如果一个基本快只有一个前驱，并且这个前驱只有一个后驱，则将当前基本快合并到唯一前驱中去
