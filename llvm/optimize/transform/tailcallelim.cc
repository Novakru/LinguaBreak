#include "tailcallelim.h"

bool TailCallElimPass::isTailCallFunc(CFG *C) {
	bool res = true;
	for(auto it = C->block_map->begin(); it != C->block_map->end(); it++) {
		int blockId = it->first;
		LLVMBlock block = it->second;
		for(auto inst : block->Instruction_list){
			inst->PrintIR(std::cerr);
		}
	}

	return res;
}

void TailCallElimPass::TailCallElim(CFG *C) {
	
}

void TailCallElimPass::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
		if(isTailCallFunc(cfg)){
			TailCallElim(cfg);
			cfg->BuildCFG();
		}
    }
}