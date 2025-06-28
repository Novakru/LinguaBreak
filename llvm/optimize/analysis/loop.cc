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

    for (size_t i = 0; i < sub_loops.size(); ++i) {
        bool last_child = (i == sub_loops.size() - 1);
        sub_loops[i]->dispLoop(depth + 1, last_child);
    }
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
				// std::cerr << "find loop header: " << bb->block_id << std::endl;
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
    std::cout << "LoopInfo: Loop Hierarchy" << std::endl;
    for (size_t i = 0; i < top_level_loops.size(); ++i) {
        bool is_last = (i == top_level_loops.size() - 1);
        top_level_loops[i]->dispLoop(0, is_last);
    }
}


/*  1. A preheader.
	2. A single backedge (which implies that there is a single latch).
	3. Dedicated exits. 
		That is, no exit block for the loop has a predecessor that is outside the loop. 
		This implies that all exit blocks are dominated by the loop header.
*/
/*
void LoopInfo::simplify(CFG* cfg) {
    // dfs, ensure sub_loop simplify first
    for (Loop* top_loop : top_level_loops) {
        simplifyLoop(top_loop, cfg);
    }

    // rebuildDominatorTree
	auto dom_tree = static_cast<DominatorTree*>(cfg->DomTree);
	auto post_dom_tree = static_cast<DominatorTree*>(cfg->PostDomTree);
	dom_tree->BuildDominatorTree(false);
	post_dom_tree->BuildDominatorTree(true);
}

void LoopInfo::simplifyLoop(Loop* loop, CFG* cfg) {
    // 1. 先递归处理子循环
    for (Loop* sub_loop : loop->getSubLoops()) {
        simplifyLoop(sub_loop, cfg);
    }

    // 2. 处理当前循环
    mergeLatches(loop, cfg);
    insertPreheader(loop, cfg);
    createDedicatedExits(loop, cfg);
}

void LoopInfo::insertPreheader(Loop* loop, CFG* cfg) {
    LLVMBlock header = loop->getHeader();
    auto preds = cfg->GetPredecessor(header->block_id);

    // 1. 创建 preheader 块
    LLVMBlock preheader = new BasicBlock(cfg->generateBlockID(), "preheader");
    cfg->addBlock(preheader);

    // 2. 重定向所有非循环内的前驱到 preheader
    for (auto pred : preds) {
        if (!loop->contains(pred)) {
            cfg->replaceSuccessor(pred, header, preheader);
        }
    }

    // 3. 添加 preheader -> header 的无条件跳转
    cfg->addEdge(preheader, header);
    loop->setPreheader(preheader);
}

void LoopInfo::createDedicatedExits(Loop* loop, CFG* cfg) {
    auto exits = loop->getExitBlocks(); // 需实现 getExitBlocks()
    for (auto exit : exits) {
        // 如果出口块有多个前驱（来自循环内外的混合控制流）
        if (cfg->GetPredecessor(exit->block_id).size() > 1) {
            // 创建新的专用出口块
            BasicBlock* dedicated_exit = new BasicBlock(cfg->generateBlockID(), "loop.exit");
            cfg->addBlock(dedicated_exit);

            // 重定向循环内到 exit 的边到 dedicated_exit
            for (auto pred : cfg->GetPredecessor(exit->block_id)) {
                if (loop->contains(pred)) {
                    cfg->replaceSuccessor(pred, exit, dedicated_exit);
                }
            }

            // 添加 dedicated_exit -> exit 的无条件跳转
            cfg->addEdge(dedicated_exit, exit);
            loop->addExitBlock(dedicated_exit); // 记录专用出口
        }
    }
}

void LoopInfo::mergeLatches(Loop* loop, CFG* cfg) {
    auto& latches = loop->getLatchBlocks();
    if (latches.size() <= 1) return; 

    // 1. 创建统一的新 latch 块
    BasicBlock* new_latch = new BasicBlock(cfg->generateBlockID(), "loop.latch");
    cfg->addBlock(new_latch);

    // 2. 重定向所有原 latch 到新 latch
    for (auto old_latch : latches) {
        // 复制旧 latch 的终结指令到新 latch（如条件分支）
        auto terminator = old_latch->getTerminator();
        if (terminator->isConditional()) {
            new_latch->addInstruction(terminator->clone()); // 需实现 clone()
        } else {
            // 默认无条件跳转回循环头
            new_latch->addInstruction(new BranchInst(loop->getHeader()));
        }

        // 重定向旧 latch 的所有非循环内后继到新 latch
        for (auto succ : cfg->GetSuccessor(old_latch->block_id)) {
            if (!loop->contains(succ)) {
                cfg->replaceSuccessor(old_latch, succ, new_latch);
            }
        }

        // 删除旧 latch 到循环头的边
        cfg->removeEdge(old_latch, loop->getHeader());
    }

    // 3. 添加新 latch 到循环头的边
    cfg->addEdge(new_latch, loop->getHeader());
    loop->clearLatches();
    loop->addLatch(new_latch); // 更新为唯一 latch
}
*/
