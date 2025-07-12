#include "loop.h"
#include <iomanip>

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


void Loop::dispLoop(int depth, bool is_last) const {
    const std::string COLOR_HEADER = "\033[1;36m";
    const std::string COLOR_LABEL  = "\033[1;33m"; 
    const std::string COLOR_RESET  = "\033[0m";    

    std::string indent;
    for (int i = 0; i < depth; ++i) {
        indent += (i == depth - 1) ? "│   " : "    ";
    }
    std::string connector = is_last ? "└── " : "├── ";

	std::string preheader_str;
	if (preheader == nullptr) {
		preheader_str = "preheader [none]\n";
	} else {
		preheader_str = "preheader [" + std::to_string(preheader->block_id) + "]\n";
	}

    std::cout << indent << connector 
              << COLOR_HEADER << "Loop (Depth " << loop_depth << ")" << COLOR_RESET 
              << " header [" << header->block_id << "], " + preheader_str;

    auto print_field = [&](const std::string& label, const auto& values) {
        std::cout << indent << "    " << COLOR_LABEL 
                  << std::setw(12) << label << ": " << COLOR_RESET;
        if (values.empty()) {
            std::cout << "none";
        } else {
            for (const auto& val : values) {
                std::cout << val->block_id << " ";
            }
        }
        std::cout << "\n";
    };

	print_field("Blocks", blocks);
    print_field("Latches", latches);
	print_field("Exitings", exitings);
	print_field("Exits", exits);

    size_t i = 0;
    size_t total = sub_loops.size();
    for (Loop* subloop : sub_loops) {
        bool last_child = (i == total - 1);
        subloop->dispLoop(depth + 1, last_child);
        i++;
    }
}

bool Loop::verifySimplifyForm(CFG* cfg) const {
	bool flag = true;
	Assert(flag &= (preheader != nullptr));
	if(!flag) return false;

	int phid = preheader->block_id;
	Assert(flag &= (phid != 0));
	Assert(flag &= (cfg->GetSuccessor(phid).size() == 1));

	Assert(flag &= (latches.size() == 1));

	Assert(flag &= (exits.size() >= 1));
	for(auto exit : exits) {
		for(auto pred : cfg->GetPredecessor(exit)) {
			Assert(flag &= (contains(pred)));
		}
	}
	
	return flag;
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
				markExitingAndExits(loop, cfg);
				break;
			}
		}
	}

	// Step 2: build loop forest, maitain subLoop
	buildLoopNesting(dom_tree);
}


Loop* LoopInfo::getOrCreateLoop(LLVMBlock header) {
	// subloop has high priority, should create a new loop
	// if (bb_loop_map.count(header)) {
	// 	return bb_loop_map[header];
	// }
	
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
		
		auto successors = cfg->GetSuccessor(bb);
		if (successors.find(header) != successors.end()) {
			loop->addLatch(bb);
		}
		auto predecessors = cfg->GetPredecessor(bb);
		for (auto pred : predecessors) {
			if (!visited.count(pred)) {
				visited.insert(pred);
				worklist.push(pred);
			}
		}
	}
}

void LoopInfo::markExitingAndExits(Loop* loop, CFG* cfg) {
	loop->clearExit();
	loop->clearExiting();
	for (LLVMBlock bb : loop->getBlocks()) {
		auto successors = cfg->GetSuccessor(bb);
		for (auto succ : successors) {
			if (!loop->contains(succ)) {
				loop->addExiting(bb);
				loop->addExit(succ);
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
            top_level_loops.insert(loop);
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
    std::cout << "LoopInfo: Loop Hierarchy" << std::endl;
    size_t i = 0;
    size_t total = top_level_loops.size();
    for (Loop* loop : top_level_loops) {
        bool is_last = (i == total - 1);
        loop->dispLoop(0, is_last);
        i++;
    }
}


/*  1. A preheader.
	2. A single backedge (which implies that there is a single latch).
	3. Dedicated exits. (may be not only one)
		That is, no exit block for the loop has a predecessor that is outside the loop. 
		This implies that all exit blocks are dominated by the loop header.
*/
void LoopInfo::simplify(CFG* cfg) {
    for (Loop* top_loop : top_level_loops) {
        simplifyLoop(top_loop, cfg);
    }

	cfg->BuildCFG();
    auto dom_tree = static_cast<DominatorTree*>(cfg->DomTree);
    dom_tree->BuildDominatorTree(false);
}

void LoopInfo::simplifyLoop(Loop* loop, CFG* cfg) {
	// dfs, make sure that childLoop simplify firstly.
    for (Loop* sub_loop : loop->getSubLoops()) {
        simplifyLoop(sub_loop, cfg);
    }

    insertPreheader(loop, cfg); 
    createDedicatedExits(loop, cfg); 
	mergeLatches(loop, cfg);
}

void LoopInfo::insertPreheader(Loop* loop, CFG* cfg) {
	LLVMBlock header = loop->getHeader();
	std::vector<LLVMBlock> external_preds;
	for (auto pred : cfg->GetPredecessor(header)) {
		if (!loop->contains(pred)) {
			external_preds.push_back(pred);
		}
	}

	assert(!external_preds.empty());

	if (external_preds.size() == 1) {
		LLVMBlock pred = external_preds.front();
		const auto successors = cfg->GetSuccessor(pred);
		if (pred->block_id != 0 && successors.size() == 1 && *(successors.begin()) == header) {
			loop->setPreheader(pred);
		}
	}

	if (loop->getPreheader()) return;

    LLVMBlock preheader = cfg->GetNewBlock();

    // 重定向 entrys 到 preheader 
	auto preds = cfg->GetPredecessor(header->block_id);
	std::set<LLVMBlock> entrys;
    for (auto pred : preds) {
        if (!loop->contains(pred)) { 
            entrys.insert(pred);
        }
    }
	cfg->replaceSuccessors(entrys, header, preheader);

    loop->setPreheader(preheader);

	Loop* curr_loop = loop->getParentLoop();
	while(curr_loop) {
		curr_loop->addBlock(preheader);
		curr_loop = curr_loop->getParentLoop();
	}
}

void LoopInfo::createDedicatedExits(Loop* loop, CFG* cfg) {
	// insertPreheader will affect exits
	markExitingAndExits(loop, cfg);

    auto exits = loop->getExits(); 
	std::unordered_set<LLVMBlock> new_exits;

    for (auto exit : exits) {
        std::set<LLVMBlock> preds = cfg->GetPredecessor(exit->block_id);
		std::set<LLVMBlock> exitings;  // exitings that point to curr exit
        bool hasOutsidePred = false;
        
        for (auto pred : preds) {
            if (!loop->contains(pred)) hasOutsidePred = true;
			else exitings.insert(pred);
        }

        if (hasOutsidePred) {
            LLVMBlock dedicated_exit = cfg->GetNewBlock();
			cfg->replaceSuccessors(exitings, exit, dedicated_exit);
			
			// 检查新创建的 dedicated_exit 是否应该被添加到父循环中
			// 如果子循环的出口边恰好是父循环的回边，新块需要成为父循环的 latch
			Loop* parent_loop = loop->getParentLoop();
			while (parent_loop) {
				// 检查 dedicated_exit 是否指向父循环的 header
				auto successors = cfg->GetSuccessor(dedicated_exit);
				if (successors.find(parent_loop->getHeader()) != successors.end()) {
					// 这是一个回边，需要将 dedicated_exit 添加到父循环并成为 latch
					parent_loop->addBlock(dedicated_exit);
					// 如果 exitings 中存在 latch , 需要删除
					for(auto exiting : exitings) {
						if(parent_loop->contains(exiting)) {
							parent_loop->removeLatch(exiting);
						}
					}
					parent_loop->addLatch(dedicated_exit);
					
					// 更新父循环的 exiting 和 exits
					markExitingAndExits(parent_loop, cfg);
				}
				parent_loop = parent_loop->getParentLoop();
			}
			
			new_exits.insert(dedicated_exit);
        } else {
			new_exits.insert(exit);
		}
    }

	loop->setExits(new_exits);
	Loop* curr_loop = loop->getParentLoop();
	while(curr_loop) {
		markExitingAndExits(curr_loop, cfg);
		curr_loop = curr_loop->getParentLoop();
	}	
}

void LoopInfo::mergeLatches(Loop* loop, CFG* cfg) {
    auto latches = loop->getLatches();
	std::set<LLVMBlock> latchSet(latches.begin(), latches.end());
    if (latches.size() == 1) return;

    LLVMBlock header = loop->getHeader();
    LLVMBlock new_latch = cfg->GetNewBlock();
	cfg->replaceSuccessors(latchSet, header, new_latch);
	
	loop->clearLatches();  // clear
    loop->addLatch(new_latch);

    auto curr_loop = loop;
    while (curr_loop) {
		curr_loop->addBlock(new_latch);
        curr_loop = curr_loop->getParentLoop();
    }
}

bool LoopInfo::verifySimplifyForm(CFG* cfg) {
	std::queue<Loop*> q;
	for (Loop* loop : top_level_loops) {
		q.push(loop);
	}
	
	while(!q.empty()) {
		Loop* curr_loop = q.front();
		if(!curr_loop->verifySimplifyForm(cfg))
			return false;
		for(auto sub_loop : curr_loop->getSubLoops()) {
			q.push(sub_loop);
		}
		q.pop();
	}

	return true;
}