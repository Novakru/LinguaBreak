#include "../../include/Instruction.h"
#include "../../include/ir.h"
#include "dominator_tree.h"
#include <stack>
#include "algorithm"

typedef std::vector<LLVMBlock>::iterator block_iterator;

class Loop {
private:
 	LLVMBlock preheader;   
    LLVMBlock header;         
    std::vector<LLVMBlock> latches;  
    std::vector<LLVMBlock> exits;  
	std::vector<LLVMBlock> exitings;  // 循环内部的块，这些块至少有一条边指向循环外的块

	std::unordered_set<LLVMBlock> block_set;
    std::vector<LLVMBlock> blocks; 
  
    Loop* parent_loop;          
    std::vector<Loop*> sub_loops;
  
    unsigned loop_depth;      
public:
    Loop(LLVMBlock header) : header(header), parent_loop(nullptr), loop_depth(0), preheader(nullptr) {
        block_set.insert(header);
        blocks.push_back(header);
    }

    LLVMBlock getHeader() const { return header; }
    Loop* getParentLoop() const { return parent_loop; }
	std::vector<Loop*> getSubLoops() const { return sub_loops; }
	std::vector<LLVMBlock> getLatches() const { return latches; }
	std::vector<LLVMBlock> getBlocks() const { return blocks; }

	void setParentLoop(Loop* L) { parent_loop = L; }

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
	void addExiting(LLVMBlock bb) { exitings.push_back(bb); }
	void addExit(LLVMBlock bb) { exits.push_back(bb); }
	void removeBlock(LLVMBlock bb);

    unsigned getLoopDepth() const { return loop_depth; }
    void setLoopDepth(unsigned depth) { loop_depth = depth; }
  
    bool verify() const;
	void dispLoop(int depth, bool is_last) const;
};

class LoopInfo {
private:
    std::vector<Loop*> top_level_loops;
    std::unordered_map<LLVMBlock, Loop*> bb_loop_map;

public:
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

	void displayLoopInfo() const;
};
