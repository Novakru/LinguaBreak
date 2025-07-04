#include "loopRotate.h"
#include "loop.h"

void LoopRotate::Execute() {
	for (auto [defI, cfg] : llvmIR->llvm_cfg) {
		LoopInfo* loopInfo = cfg->getLoopInfo();
		
		#ifdef debug
			std::cerr << "LoopRotate result:" << std::endl;
			loopInfo->displayLoopInfo();
		#endif
        cfg->loopInfo = loopInfo;
    }
}