#ifndef MACHINE_H
#define MACHINE_H
#include<string>
#include<vector>
#include<list>
#include<stack>
#include<queue>
#include"../include/Instruction.h"
#include"register.h"
#include"../inst_process/machine_instruction.h"

//注：参考原框架的MachineBlock,RiscV64Block,MachineBlockFactory,RiscV64BlockFactory,MachineCFG,MachineFunction,RiscV64Function,MachineUnit,RiscV64Unit

class MachineUnit;
class MachineFunction;
class MachineCFG;

class MachineBlock {
private:
    int label_id;

protected:
    std::list<MachineBaseInstruction *> instructions;

private:
    MachineFunction *parent;

public:

    int loop_depth = 0;

    decltype(instructions) &GetInsList() { return instructions; }
    void clear() { instructions.clear(); }
    auto erase(decltype(instructions.begin()) it) { return instructions.erase(it); }
    auto insert(decltype(instructions.begin()) it, MachineBaseInstruction *ins) { return instructions.insert(it, ins); }
    auto getParent() { return parent; }
    void setParent(MachineFunction *parent) { this->parent = parent; }
    auto getLabelId() { return label_id; }
    auto ReverseBegin() { return instructions.rbegin(); }
    auto ReverseEnd() { return instructions.rend(); }
    auto begin() { return instructions.begin(); }
    auto end() { return instructions.end(); }
    auto size() { return instructions.size(); }
    void push_back(MachineBaseInstruction *ins) { instructions.push_back(ins); }
    void push_front(MachineBaseInstruction *ins) { instructions.push_front(ins); }
    void pop_back() { instructions.pop_back(); }
    void pop_front() { instructions.pop_front(); }
    int getBlockInNumber() {
        for (auto ins : instructions) {
            return ins->getNumber() - 1;
        }
        ERROR("Empty block");
        // return (*(instructions.begin()))->getNumber();
    }
    int getBlockOutNumber() {
        for (auto it = instructions.rbegin(); it != instructions.rend(); ++it) {
            auto ins = *it;
            return ins->getNumber();
        }
        ERROR("Empty block");
        // return (*(instructions.rbegin()))->getNumber();
    }
    MachineBlock(int id) : label_id(id) {}
};
class MachineFunction {
private:
    // 函数名
    std::string func_name;
    // 所属的MachineUnit
    MachineUnit *parent;
    // 函数形参列表
    std::vector<Register> parameters;
    // 现存的最大块编号
    int max_exist_label;
    //可以修改成RiscvInstruction类型
    std::vector<MachineBaseInstruction *> allocalist;

protected:
    // 栈大小
    int stack_sz;
    // 传实参所用到的栈空间大小
    int para_sz;

public:
    // 更新现存的最大块编号
    void UpdateMaxLabel(int labelid) { max_exist_label = max_exist_label > labelid ? max_exist_label : labelid; }
    // 获取形参列表
    const decltype(parameters) &GetParameters() { return parameters; }
    // 增加形参列表
    void AddParameter(Register reg) { parameters.push_back(reg); }
    // 设置栈大小
    void SetStackSize(int sz) { stack_sz = sz; }
    // 更新传实参所用到的栈空间大小
    void UpdateParaSize(int parasz) {
        if (parasz > para_sz) {
            para_sz = parasz;
        }
    }
    // 获取传实参所用到的栈空间大小
    int GetParaSize() { return para_sz; }
    // 增加栈大小
    virtual void AddStackSize(int sz) { stack_sz += sz; }
    // 获取栈空间大小（自动对齐）
    int GetStackSize() { return ((stack_sz + 15) / 16) * 16; }
    // 获取栈底偏移
    int GetStackOffset() { return stack_sz; }
    // 获取MachineUnit
    MachineUnit *getParentMachineUnit() { return parent; }
    // 获取函数名
    std::string getFunctionName() { return func_name; }

    // 设置MachineUnit
    void SetParent(MachineUnit *parent) { this->parent = parent; }


    // 函数包含的所有块
    std::vector<MachineBlock *> blocks{};

public:
    // 获取新的虚拟寄存器
    Register GetNewRegister(int regtype, int regwidth, bool save_across_call=false);
    // 获取新的虚拟寄存器，等同于GetNewRegister(type.data_type, type.data_length)
    Register GetNewReg(MachineDataType type);

protected:
    // 获取新的块
    MachineBlock *InitNewBlock();
    bool has_inpara_instack;//新增
    MachineCFG *mcfg;

public:
    MachineFunction(std::string name)
        : func_name(name), stack_sz(0), para_sz(0), max_exist_label(0),has_inpara_instack(false){}
    //新增：
    bool HasInParaInStack() { return has_inpara_instack; }
    void SetHasInParaInStack(bool has) { has_inpara_instack = has; }
    MachineCFG *getMachineCFG() { return mcfg; }
    // 设置CFG
    void SetMachineCFG(MachineCFG *mcfg) { this->mcfg = mcfg; }
};
class MachineCFG {

public:
    class MachineCFGNode {
    public:
        MachineBlock *Mblock;
    };

private:
    std::map<int, MachineCFGNode *> block_map{};
    
    std::vector<std::vector<MachineCFGNode *>> G{}, invG{};
    int max_label;

   

     // 用于 DFS 的状态变量
     std::map<int, int> dfs_visited;
     std::stack<int> dfs_stk;
 
     // 用于 BFS 的状态变量
     std::map<int, int> bfs_visited;
     std::queue<int> bfs_que;
 
     // 用于 SeqScan 的状态变量
     decltype(block_map.begin()) seqscan_current;
 
     // 用于 Reverse 的状态变量
     std::vector<MachineCFGNode*> reverse_cache;
     decltype(reverse_cache.rbegin()) reverse_current_pos;
     

public:
    class Iterator {
    protected:
        MachineCFG *mcfg;

    public:
        Iterator(MachineCFG *mcfg) : mcfg(mcfg) {}
        MachineCFG *getMachineCFG() { return mcfg; }
        virtual void open() = 0;
        virtual MachineCFGNode *next() = 0;
        virtual bool hasNext() = 0;
        virtual void rewind() = 0;
        virtual void close() = 0;
    };
     // 定义 MockIterator 继承自 Iterator
     class MockIterator : public Iterator {
    public:
        MockIterator(MachineCFG* mcfg) : Iterator(mcfg) {}
        void open() override { mcfg->bfs_open(); }
        MachineCFG::MachineCFGNode *next() override { return mcfg->bfs_next(); }
        bool hasNext() override { return mcfg->bfs_hasNext(); }
        void rewind() override { mcfg->bfs_open(); }
        void close() override { mcfg->bfs_close(); }
    };
    
    
    MachineCFGNode *ret_block;//新增
    MachineCFG() : max_label(0){};
    void AssignEmptyNode(int id, MachineBlock *Mblk);

    // Just modify CFG edge, no change on branch instructions
    void MakeEdge(int edg_begin, int edg_end);
    void RemoveEdge(int edg_begin, int edg_end);

    MachineCFGNode *GetNodeByBlockId(int id) { return block_map[id]; }
    std::vector<MachineCFGNode *> GetSuccessorsByBlockId(int id) { return G[id]; }
    std::vector<MachineCFGNode *> GetPredecessorsByBlockId(int id) { return invG[id]; }
    // DFS 相关成员函数
    void dfs_open() {
        dfs_visited.clear();
        while (!dfs_stk.empty())
            dfs_stk.pop();
        dfs_stk.push(0);
    }

    MachineCFGNode* dfs_next() {
        auto return_idx = dfs_stk.top();
        dfs_visited[return_idx] = 1;
        dfs_stk.pop();
        for (auto succ : G[return_idx]) {
            if (dfs_visited[succ->Mblock->getLabelId()] == 0) {
                dfs_visited[succ->Mblock->getLabelId()] = 1;
                dfs_stk.push(succ->Mblock->getLabelId());
            }
        }
        return block_map[return_idx];
    }

    bool dfs_hasNext() { return !dfs_stk.empty(); }

    void dfs_rewind() { dfs_open(); }

    void dfs_close() {
        dfs_visited.clear();
        while (!dfs_stk.empty())
            dfs_stk.pop();
    }

    // BFS 相关成员函数
    void bfs_open() {
        bfs_visited.clear();
        while (!bfs_que.empty())
            bfs_que.pop();
        bfs_que.push(0);
    }

    MachineCFGNode* bfs_next() {
        auto return_idx = bfs_que.front();
        bfs_visited[return_idx] = 1;
        bfs_que.pop();
        for (auto succ : G[return_idx]) {
            if (bfs_visited[succ->Mblock->getLabelId()] == 0) {
                bfs_visited[succ->Mblock->getLabelId()] = 1;
                bfs_que.push(succ->Mblock->getLabelId());
            }
        }
        return block_map[return_idx];
    }

    bool bfs_hasNext() { return !bfs_que.empty(); }

    void bfs_rewind() { bfs_open(); }

    void bfs_close() {
        bfs_visited.clear();
        while (!bfs_que.empty())
            bfs_que.pop();
    }

    // SeqScan 相关成员函数
    void seqscan_open() { seqscan_current = block_map.begin(); }

    MachineCFGNode* seqscan_next() { return (seqscan_current++)->second; }

    bool seqscan_hasNext() { return seqscan_current != block_map.end(); }

    void seqscan_rewind() {
        seqscan_close();
        seqscan_open();
    }

    void seqscan_close() { seqscan_current = block_map.end(); }

    // Reverse 相关成员函数
    void reverse_open(Iterator* child) {
        reverse_child = child;
        reverse_child->open();
        reverse_cache.clear();
        while (reverse_child->hasNext()) {
            reverse_cache.push_back(reverse_child->next());
        }
        reverse_current_pos = reverse_cache.rbegin();
    }

    MachineCFGNode* reverse_next() { return *(reverse_current_pos++); }

    bool reverse_hasNext() { return reverse_current_pos != reverse_cache.rend(); }

    void reverse_rewind() {
        reverse_close();
        reverse_open(reverse_child);
    }

    void reverse_close() {
        reverse_child->close();
        reverse_cache.clear();
        reverse_current_pos = reverse_cache.rend();
    }
 private:
    Iterator* reverse_child;
};

// class MachineCFG {

// public:
//     class MachineCFGNode {
//     public:
//         MachineBlock *Mblock;
//     };

// private:
//     std::map<int, MachineCFGNode *> block_map{};
    
//     std::vector<std::vector<MachineCFGNode *>> G{}, invG{};
//     int max_label;

// public:
    
//     MachineCFGNode *ret_block;//新增
//     MachineCFG() : max_label(0){};
//     void AssignEmptyNode(int id, MachineBlock *Mblk);

//     // Just modify CFG edge, no change on branch instructions
//     void MakeEdge(int edg_begin, int edg_end);
//     void RemoveEdge(int edg_begin, int edg_end);

//     MachineCFGNode *GetNodeByBlockId(int id) { return block_map[id]; }
//     std::vector<MachineCFGNode *> GetSuccessorsByBlockId(int id) { return G[id]; }
//     std::vector<MachineCFGNode *> GetPredecessorsByBlockId(int id) { return invG[id]; }
// private:
//     class Iterator {
//     protected:
//         MachineCFG *mcfg;

//     public:
//         Iterator(MachineCFG *mcfg) : mcfg(mcfg) {}
//         MachineCFG *getMachineCFG() { return mcfg; }
//         virtual void open() = 0;
//         virtual MachineCFGNode *next() = 0;
//         virtual bool hasNext() = 0;
//         virtual void rewind() = 0;
//         virtual void close() = 0;
//     };
//     class SeqScanIterator;
//     class ReverseIterator;
//     class DFSIterator;
//     class BFSIterator;

// public:
//     DFSIterator *getDFSIterator();
//     BFSIterator *getBFSIterator();
//     SeqScanIterator *getSeqScanIterator();
//     ReverseIterator *getReverseIterator(Iterator *);
// };
// class MachineCFG::SeqScanIterator : public Iterator {
// private:
//     decltype(block_map.begin()) current;

// public:
//     SeqScanIterator(MachineCFG *mcfg) : Iterator(mcfg) {}
//     void open();
//     MachineCFGNode *next();
//     bool hasNext();
//     void rewind();
//     void close();
// };

// class MachineCFG::ReverseIterator : public Iterator {
// private:
//     Iterator *child;
//     std::vector<MachineCFGNode *> cache;
//     decltype(cache.rbegin()) current_pos;

// public:
//     ReverseIterator(Iterator *child) : Iterator(child->getMachineCFG()), child(child) {}
//     void open();
//     MachineCFGNode *next();
//     bool hasNext();
//     void rewind();
//     void close();
// };
// class MachineCFG::DFSIterator : public Iterator {
// private:
//     std::map<int, int> visited;
//     std::stack<int> stk;

// public:
//     DFSIterator(MachineCFG *mcfg) : Iterator(mcfg) {}
//     void open();
//     MachineCFGNode *next();
//     bool hasNext();
//     void rewind();
//     void close();
// };
// class MachineCFG::BFSIterator : public Iterator {
// private:
//     std::map<int, int> visited;
//     std::queue<int> que;

// public:
//     BFSIterator(MachineCFG *mcfg) : Iterator(mcfg) {}
//     void open();
//     MachineCFGNode *next();
//     bool hasNext();
//     void rewind();
//     void close();
// };
// class MachineFunction {
// private:
//     // 函数元数据
//     std::string func_name;
//     MachineUnit* parent_unit;
//     std::vector<Register> parameters;

//     // 控制流图数据
//     std::map<int, MachineBlock*> block_map;      // 标签ID到基本块
//     std::map<int, std::vector<MachineBlock*>> successors;   // 后继块
//     std::map<int, std::vector<MachineBlock*>> predecessors; // 前驱块
//     int max_label_id = 0;

//     // 栈管理
//     int stack_size = 0;
//     int param_stack_size = 0;
//     bool has_inpara_instack = false;

//     // 迭代器基类
//     class CFGIterator {
//     protected:
//         MachineFunction* mfunc;
//         std::vector<MachineBlock*> visited;
//     public:
//         explicit CFGIterator(MachineFunction* f) : mfunc(f) {}
//         virtual ~CFGIterator() = default;
//         virtual MachineBlock* next() = 0;
//         virtual bool hasNext() const = 0;
//     };

// public:
//     // 构造/元数据
//     explicit MachineFunction(std::string name) 
//         : func_name(std::move(name)), parent_unit(nullptr) {}

//     std::string getName() const { return func_name; }
//     MachineUnit* getParentUnit() const { return parent_unit; }

//     // 基本块管理 --------------------------------------------------------
//     MachineBlock* createBlock(int label_id) {
//         auto* block = new MachineBlock(label_id);
//         block->setParent(this);
//         block_map[label_id] = block;
//         max_label_id = std::max(max_label_id, label_id);
//         return block;
//     }

//     MachineBlock* getBlock(int label_id) const {
//         auto it = block_map.find(label_id);
//         return it != block_map.end() ? it->second : nullptr;
//     }

//     // 控制流图操作 -----------------------------------------------------
//     void addEdge(int from_label, int to_label) {
//         MachineBlock* from = getBlock(from_label);
//         MachineBlock* to = getBlock(to_label);
//         if (!from || !to) throw std::runtime_error("Invalid block label");

//         successors[from_label].push_back(to);
//         predecessors[to_label].push_back(from);
//     }

//     void removeEdge(int from_label, int to_label) {
//         auto& succs = successors[from_label];
//         succs.erase(std::remove_if(succs.begin(), succs.end(),
//             [to_label](MachineBlock* b) { return b->getLabelId() == to_label; }), 
//             succs.end());

//         auto& preds = predecessors[to_label];
//         preds.erase(std::remove_if(preds.begin(), preds.end(),
//             [from_label](MachineBlock* b) { return b->getLabelId() == from_label; }), 
//             preds.end());
//     }

//     const std::vector<MachineBlock*>& getSuccessors(int label_id) const {
//         static const std::vector<MachineBlock*> empty;
//         auto it = successors.find(label_id);
//         return it != successors.end() ? it->second : empty;
//     }

//     const std::vector<MachineBlock*>& getPredecessors(int label_id) const {
//         static const std::vector<MachineBlock*> empty;
//         auto it = predecessors.find(label_id);
//         return it != predecessors.end() ? it->second : empty;
//     }

//     // 迭代器工厂方法 ---------------------------------------------------
//     class DFSIterator : public CFGIterator {
//         std::stack<MachineBlock*> stack;
//     public:
//         explicit DFSIterator(MachineFunction* f) : CFGIterator(f) {
//             if (!f->block_map.empty()) 
//                 stack.push(f->block_map.begin()->second);
//         }

//         MachineBlock* next() override {
//             if (stack.empty()) return nullptr;
//             MachineBlock* current = stack.top();
//             stack.pop();
//             visited.push_back(current);

//             for (auto* succ : mfunc->getSuccessors(current->getLabelId())) {
//                 if (std::find(visited.begin(), visited.end(), succ) == visited.end()) {
//                     stack.push(succ);
//                 }
//             }
//             return current;
//         }

//         bool hasNext() const override { return !stack.empty(); }
//     };

//     DFSIterator* createDFSIterator() { return new DFSIterator(this); }

//     // 寄存器管理 -------------------------------------------------------
//     Register createVirtualRegister(MachineDataType type, bool save_across_call = false) {
//         static int counter = 0;
//         return Register(true, counter++, type, save_across_call);
//     }

//     // 栈管理 ----------------------------------------------------------
//     void adjustStackSize(int delta) { 
//         stack_size += delta; 
//         stack_size = (stack_size + 15) & ~15; // 16字节对齐
//     }

//     int getStackSize() const { return stack_size; }
// };
// class MachineCFG {

// public:
//     class MachineCFGNode {
//     public:
//         MachineBlock *Mblock;
//     };

// private:
//     std::map<int, MachineCFGNode *> block_map{};
    
//     std::vector<std::vector<MachineCFGNode *>> G{}, invG{};
//     int max_label;

// public:
    
//     MachineCFGNode *ret_block;//新增
//     MachineCFG() : max_label(0){};
//     void AssignEmptyNode(int id, MachineBlock *Mblk);

//     // Just modify CFG edge, no change on branch instructions
//     void MakeEdge(int edg_begin, int edg_end);
//     void RemoveEdge(int edg_begin, int edg_end);

//     MachineCFGNode *GetNodeByBlockId(int id) { return block_map[id]; }
//     std::vector<MachineCFGNode *> GetSuccessorsByBlockId(int id) { return G[id]; }
//     std::vector<MachineCFGNode *> GetPredecessorsByBlockId(int id) { return invG[id]; }

// private:
//     class Iterator {
//     protected:
//         MachineCFG *mcfg;

//     public:
//         Iterator(MachineCFG *mcfg) : mcfg(mcfg) {}
//         MachineCFG *getMachineCFG() { return mcfg; }
//         virtual void open() = 0;
//         virtual MachineCFGNode *next() = 0;
//         virtual bool hasNext() = 0;
//         virtual void rewind() = 0;
//         virtual void close() = 0;
//     };
//     class SeqScanIterator;
//     class ReverseIterator;
//     class DFSIterator;
//     class BFSIterator;

// public:
//     DFSIterator *getDFSIterator();
//     BFSIterator *getBFSIterator();
//     SeqScanIterator *getSeqScanIterator();
//     ReverseIterator *getReverseIterator(Iterator *);
// };


//保存全局变量与函数信息


// class MachineBlock {
// private:
//     int label_id;

// protected:
//     std::list<MachineBaseInstruction *> instructions;

// private:
//     MachineFunction *parent;

// public:
//     //新增：
//     virtual std::list<MachineBaseInstruction *>::iterator getInsertBeforeBrIt() = 0;
//     int loop_depth = 0;

//     decltype(instructions) &GetInsList() { return instructions; }
//     void clear() { instructions.clear(); }
//     auto erase(decltype(instructions.begin()) it) { return instructions.erase(it); }
//     auto insert(decltype(instructions.begin()) it, MachineBaseInstruction *ins) { return instructions.insert(it, ins); }
//     auto getParent() { return parent; }
//     void setParent(MachineFunction *parent) { this->parent = parent; }
//     auto getLabelId() { return label_id; }
//     auto ReverseBegin() { return instructions.rbegin(); }
//     auto ReverseEnd() { return instructions.rend(); }
//     auto begin() { return instructions.begin(); }
//     auto end() { return instructions.end(); }
//     auto size() { return instructions.size(); }
//     void push_back(MachineBaseInstruction *ins) { instructions.push_back(ins); }
//     void push_front(MachineBaseInstruction *ins) { instructions.push_front(ins); }
//     void pop_back() { instructions.pop_back(); }
//     void pop_front() { instructions.pop_front(); }
//     int getBlockInNumber() {
//         for (auto ins : instructions) {
//             return ins->getNumber() - 1;
//         }
//         ERROR("Empty block");
//         // return (*(instructions.begin()))->getNumber();
//     }
//     int getBlockOutNumber() {
//         for (auto it = instructions.rbegin(); it != instructions.rend(); ++it) {
//             auto ins = *it;
//             return ins->getNumber();
//         }
//         ERROR("Empty block");
//         // return (*(instructions.rbegin()))->getNumber();
//     }
//     MachineBlock(int id) : label_id(id) {}
// };

// class RiscV64Block : public MachineBlock {
// public:
//     RiscV64Block(int id) : MachineBlock(id) {}
//     std::list<MachineBaseInstruction *>::iterator getInsertBeforeBrIt();
// };
    

// class MachineBlockFactory {
// public:
//     virtual MachineBlock *CreateBlock(int id) = 0;
// };

// class RiscV64BlockFactory : public MachineBlockFactory {
// public:
//     MachineBlock *CreateBlock(int id) { return new RiscV64Block(id); }
// };


// class MachineCFG {
//     friend class MachineDominatorTree;

// public:
//     class MachineCFGNode {
//     public:
//         MachineBlock *Mblock;
//     };

// private:
//     std::map<int, MachineCFGNode *> block_map{};
    
//     std::vector<std::vector<MachineCFGNode *>> G{}, invG{};
//     int max_label;

// public:
    
//     MachineCFGNode *ret_block;//新增
//     MachineCFG() : max_label(0){};
//     void AssignEmptyNode(int id, MachineBlock *Mblk);

//     // Just modify CFG edge, no change on branch instructions
//     void MakeEdge(int edg_begin, int edg_end);
//     void RemoveEdge(int edg_begin, int edg_end);

//     MachineCFGNode *GetNodeByBlockId(int id) { return block_map[id]; }
//     std::vector<MachineCFGNode *> GetSuccessorsByBlockId(int id) { return G[id]; }
//     std::vector<MachineCFGNode *> GetPredecessorsByBlockId(int id) { return invG[id]; }

//     // //新增
//     // MachineDominatorTree DomTree, PostDomTree;
//     // void BuildDominatoorTree(bool buildPost = true) {
//     //     DomTree.C = this;
//     //     DomTree.BuildDominatorTree();

//     //     PostDomTree.C = this;
//     //     if (buildPost) {
//     //         PostDomTree.BuildPostDominatorTree();
//     //     }
//     // }
//     // MachineNaturalLoopForest LoopForest;
//     // void BuildLoopForest() {
//     //     LoopForest.C = this;
//     //     LoopForest.BuildLoopForest();
//     // }

// private:
//     class Iterator {
//     protected:
//         MachineCFG *mcfg;

//     public:
//         Iterator(MachineCFG *mcfg) : mcfg(mcfg) {}
//         MachineCFG *getMachineCFG() { return mcfg; }
//         virtual void open() = 0;
//         virtual MachineCFGNode *next() = 0;
//         virtual bool hasNext() = 0;
//         virtual void rewind() = 0;
//         virtual void close() = 0;
//     };
//     class SeqScanIterator;
//     class ReverseIterator;
//     class DFSIterator;
//     class BFSIterator;

// public:
//     DFSIterator *getDFSIterator();
//     BFSIterator *getBFSIterator();
//     SeqScanIterator *getSeqScanIterator();
//     ReverseIterator *getReverseIterator(Iterator *);
// };

// class MachineFunction {
// private:
//     // 函数名
//     std::string func_name;
//     // 所属的MachineUnit
//     MachineUnit *parent;
//     // 函数形参列表
//     std::vector<Register> parameters;
//     // 现存的最大块编号
//     int max_exist_label;

//     // 用于基本块相关操作的辅助类
//     MachineBlockFactory *block_factory;

// protected:
//     // 栈大小
//     int stack_sz;
//     // 传实参所用到的栈空间大小
//     int para_sz;
//     MachineCFG *mcfg;

// public:
//     // 更新现存的最大块编号
//     void UpdateMaxLabel(int labelid) { max_exist_label = max_exist_label > labelid ? max_exist_label : labelid; }
//     // 获取形参列表
//     const decltype(parameters) &GetParameters() { return parameters; }
//     // 增加形参列表
//     void AddParameter(Register reg) { parameters.push_back(reg); }
//     // 设置栈大小
//     void SetStackSize(int sz) { stack_sz = sz; }
//     // 更新传实参所用到的栈空间大小
//     void UpdateParaSize(int parasz) {
//         if (parasz > para_sz) {
//             para_sz = parasz;
//         }
//     }
//     // 获取传实参所用到的栈空间大小
//     int GetParaSize() { return para_sz; }
//     // 增加栈大小
//     virtual void AddStackSize(int sz) { stack_sz += sz; }
//     // 获取栈空间大小（自动对齐）
//     int GetStackSize() { return ((stack_sz + 15) / 16) * 16; }
//     // 获取栈底偏移
//     int GetStackOffset() { return stack_sz; }
//     // 获取CFG
//     MachineCFG *getMachineCFG() { return mcfg; }
//     // 获取MachineUnit
//     MachineUnit *getParentMachineUnit() { return parent; }
//     // 获取函数名
//     std::string getFunctionName() { return func_name; }

//     // 设置MachineUnit
//     void SetParent(MachineUnit *parent) { this->parent = parent; }
//     // 设置CFG
//     void SetMachineCFG(MachineCFG *mcfg) { this->mcfg = mcfg; }

//     // 函数包含的所有块
//     std::vector<MachineBlock *> blocks{};

// public:
//     // 获取新的虚拟寄存器
//     Register GetNewRegister(int regtype, int regwidth, bool save_across_call=false);
//     // 获取新的虚拟寄存器，等同于GetNewRegister(type.data_type, type.data_length)
//     Register GetNewReg(MachineDataType type);

// protected:
//     // 获取新的块
//     MachineBlock *InitNewBlock();
//     bool has_inpara_instack;//新增

// public:
//     MachineFunction(std::string name, MachineBlockFactory *blockfactory)
//         : func_name(name), stack_sz(0), para_sz(0), block_factory(blockfactory), max_exist_label(0),has_inpara_instack(false){}
//     //新增：
//     bool HasInParaInStack() { return has_inpara_instack; }
//     void SetHasInParaInStack(bool has) { has_inpara_instack = has; }
// };

// class RiscV64Function : public MachineFunction {
// public:
//     RiscV64Function(std::string name) : MachineFunction(name, new RiscV64BlockFactory()) {}

// private:
//     // TODO: add your own members here
//     std::vector<RiscV64Instruction *> allocalist;
// public:
//     // TODO: add your own members here
//     void AddAllocaIns(RiscV64Instruction *ins) { allocalist.push_back(ins); }
//     //新增
//     void AddParameterSize(int sz) {
//         for (auto ins : allocalist) {
//             ins->setImm(ins->getImm() + sz);
//         }
//     }
// };

#endif

