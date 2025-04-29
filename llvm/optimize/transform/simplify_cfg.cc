#include "simplify_cfg.h"

void SimplifyCFGPass::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        EliminateUnreachedBlocksInsts(cfg);
    }
}

// 删除从函数入口开始到达不了的基本块和指令（删除不可达指令已经在之前完成，这里只需要遍历取出不可达基本块）
void SimplifyCFGPass::EliminateUnreachedBlocksInsts(CFG *C) {
    for (auto it = C->block_map->begin(); it != C->block_map->end(); ) {
        if (!(it->second->dfs_id)) {
            it = C->block_map->erase(it); // 删除元素并更新迭代器
        } else {
            ++it; // 仅在未删除元素时移向下一个
        }
    }
}