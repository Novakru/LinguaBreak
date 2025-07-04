#include "loopAnalysis.h"
#include "loopAnalysis.h"
#include "loop.h"

void LoopAnalysisPass::Execute() {
	for (auto [defI, cfg] : llvmIR->llvm_cfg) {
		LoopInfo* loopInfo = new LoopInfo();
		loopInfo->analyze(cfg);
		#ifdef debug
			std::cerr << "LoopAnalysis result:" << std::endl;
			loopInfo->displayLoopInfo();
		#endif
        cfg->loopInfo = loopInfo;
    }
}	