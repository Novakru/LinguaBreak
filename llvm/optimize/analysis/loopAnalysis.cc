#include "loopAnalysis.h"
#include "loop.h"

void LoopAnalysisPass::Execute() {
	for (auto [defI, cfg] : llvmIR->llvm_cfg) {
		LoopInfo* loopInfo = new LoopInfo();
		loopInfo->analyze(cfg);
		loopInfo->displayLoopInfo();
        cfg->loopInfo = loopInfo;
    }
}	