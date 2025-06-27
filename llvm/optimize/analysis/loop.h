#include "../../include/Instruction.h"
#include "../../include/ir.h"
#include "dominator_tree.h"
#include <stack>
#include "algorithm"

typedef std::vector<LLVMBlock>::iterator block_iterator;

class Loop {
private:
    LLVMBlock header;         
    std::vector<LLVMBlock> latches;  
    std::vector<LLVMBlock> preheaders;                    
  
    std::unordered_set<LLVMBlock> block_set;
    std::vector<LLVMBlock> blocks; 
    std::vector<LLVMBlock> exits;  
  
    Loop* parent_loop;          
    std::vector<Loop*> sub_loops;
  
    unsigned loop_depth;      
public:
    Loop(LLVMBlock header) : header(header), parent_loop(nullptr), loop_depth(0) {
        block_set.insert(header);
        blocks.push_back(header);
    }

    LLVMBlock getHeader() const { return header; }
    std::vector<LLVMBlock> getLatches() const { return latches; }
    Loop* getParentLoop() const { return parent_loop; }
	void setParentLoop(Loop* L) {parent_loop = L;}
  
    block_iterator block_begin() { return blocks.begin(); }
    block_iterator block_end() { return blocks.end(); }
  
    typedef std::vector<Loop*>::iterator loop_iterator;
    loop_iterator begin() { return sub_loops.begin(); }
    loop_iterator end() { return sub_loops.end(); }
  
    bool contains(const LLVMBlock bb) const;
    bool contains(const Loop* L) const;
  
    void addBlock(LLVMBlock bb);
    void removeBlock(LLVMBlock bb);

    void addLatch(LLVMBlock latch) { latches.push_back(latch); }
	void addSubLoop(Loop* L) {sub_loops.push_back(L);}
  
    unsigned getLoopDepth() const { return loop_depth; }
    void setLoopDepth(unsigned depth) { loop_depth = depth; }
  
    bool verify() const;
	void dispLoop() const;
};

class LoopInfo {
private:
    std::vector<Loop*> top_level_loops;
    std::unordered_map<LLVMBlock, Loop*> bb_loop_map;

public:
    void analyze(CFG* cfg);
    Loop* getOrCreateLoop(LLVMBlock header);
    void discoverLoopBlocks(Loop* loop, LLVMBlock back_edge_src, CFG* cfg);
    void buildLoopNesting(DominatorTree* dom_tree);
    void computeLoopDepth(Loop* loop);
	void displayLoopInfo() const;
};
