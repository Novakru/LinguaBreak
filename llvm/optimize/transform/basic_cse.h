#ifndef BASIC_CSE_H
#define BASIC_CSE_H

#include "../../include/cfg.h"
#include <vector>
#include <string>
#include <map>
#include <set>
#include <deque>
#include "../analysis/dominator_tree.h"

// 存储指令的CSE关键信息
struct InstCSEInfo {
    int opcode;                        // 指令操作码
    std::vector<std::string> operand_list; // 操作数字符串列表
    
    // 比较CSE是否相同
    bool operator<(const InstCSEInfo &other) const;
};

//extern std::map<std::string, CFG *> CFGMap;            // 全局函数名到CFG的映射

// 函数声明
// void CSEInit(CFG* cfg);
bool CSENotConsider(Instruction inst);
// InstCSEInfo GetCSEInfo(Instruction inst);
// void SimpleBlockCSE(CFG* cfg);
// void SimpleDomTreeWalkCSE(CFG* C);
//void SimpleCSE(CFG* cfg);

// 基本块CSE优化实现
class BasicBlockCSEOptimizer {
public:
    BasicBlockCSEOptimizer(CFG* cfg) : C(cfg), changed(false),flag(false) {}
    bool optimize();

private:
    CFG* C;
public:
    bool changed;
    bool flag;
    std::map<int, int> reg_replace_map;
    std::set<Instruction> erase_set;
    std::map<InstCSEInfo, int> inst_map;

    void reset();
    void processAllBlocks();
    bool processBlock(LLVMBlock bb);
    void processCallInstruction(CallInstruction* callI);
    void processStoreInstruction(StoreInstruction* storeI);
    void processLoadInstruction(LoadInstruction* loadI);
    void processRegularInstruction(BasicInstruction* I);
    void removeDeadInstructions();
    void applyRegisterReplacements();
};

// 支配树优化
class DomTreeCSEOptimizer {
private:
    CFG* C;
    bool changed;
    std::set<Instruction> eraseSet;
    std::map<InstCSEInfo, int> instCSEMap;
    //std::map<InstCSEInfo, std::vector<Instruction>> loadCSEMap;
    std::map<int, int> regReplaceMap;
    DomAnalysis *domtrees;

public:
    DomTreeCSEOptimizer(CFG* cfg) : C(cfg), changed(true) {}
    void optimize();

private:
    void dfs(int bbid);
    void processLoadInstruction(LoadInstruction* loadI, std::map<InstCSEInfo, int>& tmpLoadNumMap);
    void processStoreInstruction(StoreInstruction* storeI, std::map<InstCSEInfo, int>& tmpLoadNumMap);
    void processCallInstruction(CallInstruction* callI);
    void processRegularInstruction(BasicInstruction* I, std::set<InstCSEInfo>& tmpCSESet);
    //void cleanupTemporaryEntries(const std::set<InstCSEInfo>& tmpCSESet,const std::map<InstCSEInfo, int>& tmpLoadNumMap);
    void cleanupTemporaryEntries(const std::set<InstCSEInfo>& tmpCSESet);
    void removeDeadInstructions();
    void applyRegisterReplacements();
};

class SimpleCSEPass : public IRPass { 
private:
    DomAnalysis *domtrees;

public:
    SimpleCSEPass(LLVMIR *IR, DomAnalysis *dom) : IRPass(IR) { domtrees = dom; }
    void Execute();
    void BlockExecute();
    void CSEInit(CFG* cfg);
    InstCSEInfo GetCSEInfo(Instruction inst);
    void SimpleBlockCSE(CFG* cfg);
    void SimpleDomTreeWalkCSE(CFG* C);
};

#endif