#include "loopSimplify.h"
#include "loop.h"

void LoopSimplifyPass::Execute() {
	for (auto [defI, cfg] : llvmIR->llvm_cfg) {
		LoopInfo* loopInfo = cfg->getLoopInfo();
		loopInfo->simplify(cfg);
		#ifdef debug 
			std::cerr << "LoopSimplify result:" << std::endl;
			loopInfo->displayLoopInfo();
		#endif
		Assert(loopInfo->verifySimplifyForm(cfg));
        cfg->loopInfo = loopInfo;
    }
}