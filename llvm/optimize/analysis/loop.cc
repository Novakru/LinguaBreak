#include "loop.h"

void Loop::addBlock(LLVMBlock bb) {
	if (block_set.insert(bb).second) {
		blocks.push_back(bb);
	}
}

void Loop::removeBlock(LLVMBlock bb) {
    auto it = block_set.find(bb);
    if (it != block_set.end()) {
        block_set.erase(it);
		blocks.erase(std::find(blocks.begin(), blocks.end(), bb));
    }
}

bool Loop::contains(const LLVMBlock bb) const {
    return block_set.find(bb) != block_set.end(); 
}

bool Loop::contains(const Loop* L) const {
	for (auto bb : L->blocks) {
        if (block_set.find(bb) == block_set.end()) {
            return false;
        }
    }
    return true;
}

void Loop::dispLoop() const {
	std::cout << "Loop Information:\n";
	std::cout << "--------------------------\n";
	
	std::cout << "Header Block: ";
	if (header != nullptr) {
		std::cout << header->block_id << "\n";
	} else {
		std::cout << "Invalid\n";
	}

	std::cout << "Latch Blocks (" << latches.size() << "): ";
	for (const auto& latch : latches) {
		std::cout << latch->block_id << " ";
	}
	std::cout << "\n";

	std::cout << "Preheader Blocks (" << preheaders.size() << "): ";
	for (const auto& preheader : preheaders) {
		std::cout << preheader->block_id << " ";
	}
	std::cout << "\n";

	std::cout << "Loop Blocks (" << blocks.size() << "): ";
	for (const auto& block : blocks) {
		std::cout << block->block_id << " ";
	}
	std::cout << "\n";

	std::cout << "Exit Blocks (" << exits.size() << "): ";
	for (const auto& exit : exits) {
		std::cout << exit->block_id << " ";
	}
	std::cout << "\n";

	std::cout << "Parent Loop: ";
	if (parent_loop) {
		std::cout << "exists (header: " << parent_loop->header->block_id << ")\n";
	} else {
		std::cout << "none\n";
	}

	std::cout << "Sub Loops (" << sub_loops.size() << "): ";
	for (const auto& sub_loop : sub_loops) {
		std::cout << sub_loop->header->block_id << " ";
	}
	std::cout << "\n";

	std::cout << "Loop Depth: " << loop_depth << "\n";
	std::cout << "--------------------------\n";
}

void LoopInfo::analyze(CFG* cfg) {
	auto dom_tree = static_cast<DominatorTree*>(cfg->DomTree);
	top_level_loops.clear();
	bb_loop_map.clear();

	// Step 1: find loop header
	for (auto& block_pair : *(cfg->block_map)) {
		int block_id = block_pair.first;
		LLVMBlock bb = block_pair.second;
	
		auto predecessors = cfg->GetPredecessor(block_id);
		for (auto pred : predecessors) {
			if (dom_tree->dominates(bb, pred)) {  // bb is loop header
				Loop* loop = getOrCreateLoop(bb); // create or get the loop with bb as header
				discoverLoopBlocks(loop, pred, cfg); // maintain blocks
				break;
			}
		}
	}

	// Step 2: build loop forest, maitain subLoop
	buildLoopNesting(dom_tree);
}


Loop* LoopInfo::getOrCreateLoop(LLVMBlock header) {
	if (bb_loop_map.count(header)) {
		return bb_loop_map[header];
	}
	
	Loop* loop = new Loop(header);
	bb_loop_map[header] = loop;
	return loop;
}

void LoopInfo::discoverLoopBlocks(Loop* loop, LLVMBlock back_edge_src, CFG* cfg) {
	std::stack<LLVMBlock> worklist;
	std::unordered_set<LLVMBlock> visited;
	
	worklist.push(back_edge_src);
	visited.insert(back_edge_src);
	
	LLVMBlock header = loop->getHeader();
	
	while (!worklist.empty()) {
		LLVMBlock bb = worklist.top();
		worklist.pop();

		if (loop->contains(bb)) { continue; }

		loop->addBlock(bb);
		bb_loop_map[bb] = loop;
		
		// check latch block
		auto successors = cfg->GetSuccessor(bb);
		if (successors.find(header) != successors.end()) {
			loop->addLatch(bb);
		}
		
		// insert predecessors into blocks
		auto predecessors = cfg->GetPredecessor(bb);
		for (auto pred : predecessors) {
			if (!visited.count(pred)) {
				visited.insert(pred);
				worklist.push(pred);
			}
		}
	}
}

void LoopInfo::buildLoopNesting(DominatorTree* dom_tree) {

    for (auto& entry : bb_loop_map) {
		// skip block that is not header.
		Loop* loop = entry.second;
        if (loop->getHeader() != entry.first) continue;
        
        Loop* parent = nullptr;
		LLVMBlock header = loop->getHeader();
		// The nearest loop that satisfies contains() in the dominance chain.
        for (auto& dom : dom_tree->getDominators(header)) {
            if (dom != header && bb_loop_map.count(dom)) {
                Loop* candidate = bb_loop_map[dom];
                if (candidate->getHeader() != header && candidate->contains(header)) {
                    parent = candidate;
                    break; 
                }
            }
        }
        if (parent) {
            parent->addSubLoop(loop);
            loop->setParentLoop(parent);
        } else {
            top_level_loops.push_back(loop);
        }
    }

    for (Loop* loop : top_level_loops) {
        computeLoopDepth(loop);
    }
}

void LoopInfo::computeLoopDepth(Loop* loop) {
    unsigned depth = (loop->getParentLoop() ? loop->getParentLoop()->getLoopDepth() + 1 : 1);
    loop->setLoopDepth(depth);
    
    for (Loop* subloop : *loop) {
        computeLoopDepth(subloop);
    }
}

void LoopInfo::displayLoopInfo() const {
	std::cout << "==== Loop Hierarchy ====\n";
	for (auto loop : top_level_loops) {
		loop->dispLoop();
	}
	std::cout << "=======================\n";
}