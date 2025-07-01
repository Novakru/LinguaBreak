#include "loopAnalysis.h"
#include "loop.h"

#define debug

void LoopAnalysisPass::Execute() {
	for (auto [defI, cfg] : llvmIR->llvm_cfg) {
		LoopInfo* loopInfo = new LoopInfo();
		loopInfo->analyze(cfg);
		#ifdef debug
			std::cerr << "LoopAnalysis result:" << std::endl;
			loopInfo->displayLoopInfo();
		#endif
		loopInfo->simplify(cfg);
		#ifdef debug
			std::cerr << "LoopSimplify result:" << std::endl;
			loopInfo->displayLoopInfo();
		#endif
		Assert(loopInfo->verifySimplifyForm(cfg));
        cfg->loopInfo = loopInfo;
    }
}	