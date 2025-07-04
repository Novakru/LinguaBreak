#ifndef LICM_H
#define LICM_H
#include "../../include/ir.h"
#include "../pass.h"
#include "loop.h"

class LoopInvariantCodeMotionPass : public IRPass {
public:
	std::unordered_map<Operand, Instruction> inloop_defs;
	std::unordered_map<Operand, std::vector<Instruction>> inloop_uses;
	std::unordered_set<Instruction> eraseInsts;

    void RunLICMOnLoop(Loop* loop, DominatorTree* dt);
	bool dominatesAllExits(LLVMBlock bb, Loop* loop, DominatorTree* dt);
	bool canHoistWithAlias(Instruction inst, Loop* loop);
	bool isInvariant(Instruction inst, Loop* loop);
	void getInloopDefUses(Loop* loop);
	void eraseBlockOnLoop(Loop* loop);
    LoopInvariantCodeMotionPass(LLVMIR *IR) : IRPass(IR) {}
    void Execute();
};

#endif