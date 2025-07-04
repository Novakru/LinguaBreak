#include "../../include/Instruction.h"
#include "../../include/ir.h"
#include "dominator_tree.h"
#include <stack>
#include "algorithm"

#define debug

typedef std::vector<LLVMBlock>::iterator block_iterator;

class Loop {
private:
 	LLVMBlock preheader;   			  // 循环之前的一个块，通常唯一指向 header
    LLVMBlock header;         
    std::vector<LLVMBlock> latches;   // 门闩，指向 header 的块，backedge 的起始基本块
    std::unordered_set<LLVMBlock> exits;  
	std::unordered_set<LLVMBlock> exitings;  // 循环内部的块，这些块至少有一条边指向循环外的块

	std::unordered_set<LLVMBlock> block_set; // 循环包含的基本块集合
    std::vector<LLVMBlock> blocks; 
  
    Loop* parent_loop;                     
    std::vector<Loop*> sub_loops;     // 循环深度（顶层循环为 1，嵌套每层 +1）
  
    unsigned loop_depth;      
public:
    Loop(LLVMBlock header) : header(header), parent_loop(nullptr), loop_depth(0), preheader(nullptr) {
        block_set.insert(header);
        blocks.push_back(header);
    }

	// get method
	LLVMBlock getPreheader() const {return preheader;}
    LLVMBlock getHeader() const { return header; }
    Loop* getParentLoop() const { return parent_loop; }
	std::vector<Loop*> getSubLoops() const { return sub_loops; }
	std::vector<LLVMBlock> getLatches() const { return latches; }
	std::vector<LLVMBlock> getBlocks() const { return blocks; }
	std::unordered_set<LLVMBlock> getExits() const { return exits; }
	std::unordered_set<LLVMBlock> getExitings() const { return exitings; }
	unsigned getLoopDepth() const { return loop_depth; }

	// set method
	void setPreheader(LLVMBlock b) { preheader = b; }
	void setHeader(LLVMBlock b) { header = b; }
	void setParentLoop(Loop* ploop) { parent_loop = ploop; }
	void setSubLoops(const std::vector<Loop*>& sub) { sub_loops = sub; }
	void setLatches(const std::vector<LLVMBlock>& l) { latches = l; }
	void setBlocks(const std::vector<LLVMBlock>& b) { blocks = b; }
	void setExits(const std::unordered_set<LLVMBlock>& e) { exits = e; }
	void setExitingss(const std::unordered_set<LLVMBlock>& e) { exitings = e; }
	void setLoopDepth(unsigned depth) { loop_depth = depth; }

    block_iterator block_begin() { return blocks.begin(); }
    block_iterator block_end() { return blocks.end(); }
  
    typedef std::vector<Loop*>::iterator loop_iterator;
    loop_iterator begin() { return sub_loops.begin(); }
    loop_iterator end() { return sub_loops.end(); }
  
    bool contains(const LLVMBlock bb) const;
    bool contains(const Loop* L) const;
  
	void addBlock(LLVMBlock bb);
    void addLatch(LLVMBlock latch) { latches.push_back(latch); }
	void addSubLoop(Loop* L) {sub_loops.push_back(L);}
	void addExiting(LLVMBlock bb) { exitings.insert(bb); }
	void addExit(LLVMBlock bb) { exits.insert(bb); }
	void removeBlock(LLVMBlock bb);
	void clearExit() { exitings.clear(); }
	void clearExiting() { exits.clear(); }

    bool verifySimplifyForm(CFG* cfg) const;
	void dispLoop(int depth, bool is_last) const;
};

class LoopInfo {
private:
    std::vector<Loop*> top_level_loops;
    std::unordered_map<LLVMBlock, Loop*> bb_loop_map;

public:
	/* normal method */
	std::vector<Loop*> getTopLevelLoops() { return top_level_loops; }

	/* loop Analysis about */
    void analyze(CFG* cfg);
    Loop* getOrCreateLoop(LLVMBlock header);
    void discoverLoopBlocks(Loop* loop, LLVMBlock back_edge_src, CFG* cfg);
    void buildLoopNesting(DominatorTree* dom_tree);
    void computeLoopDepth(Loop* loop);
	void markExitingAndExits(Loop* loop, CFG* cfg);

	/* loop Simplify about */
	void simplify(CFG* cfg);
	void simplifyLoop(Loop* loop, CFG* cfg);
	void insertPreheader(Loop* loop, CFG* cfg);
	void createDedicatedExits(Loop* loop, CFG* cfg);
	void mergeLatches(Loop* loop, CFG* cfg);
	bool verifySimplifyForm(CFG* cfg);

	void displayLoopInfo() const;
};
