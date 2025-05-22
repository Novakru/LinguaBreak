#include "../../include/Instruction.h"
#include "../../include/ir.h"
#include <assert.h>

int dfs_num = 0;

void LLVMIR::CFGInit() {
    for (auto &[defI, bb_map] : function_block_map) {
        CFG *cfg = new CFG();
        cfg->block_map = &bb_map;
        cfg->function_def = defI;
		cfg->max_label = function_max_label[defI];

        cfg->BuildCFG();
        llvm_cfg[defI] = cfg;
    }
}


void CFG::SearchB(LLVMBlock B){
    if(B->dfs_id!=0)return;
    B->dfs_id = ++dfs_num;//全局
    //遍历此基本块的所有指令
    for(auto it = B->Instruction_list.begin(); it != B->Instruction_list.end(); ++it){
        auto &intr = *it;
        //【1】无条件跳转指令
        if(intr->GetOpcode()==BasicInstruction::LLVMIROpcode::BR_UNCOND){
            //（1）取出跳转指令的目标标签
            Operand operand = ((BrUncondInstruction*)intr)->GetDestLabel();
            int next_label = ((LabelOperand*)operand)->GetLabelNo();
            LLVMBlock next_block = (*block_map)[next_label];
            //（2）维护G/invG
             G[B->block_id].insert(next_block);
            invG[next_label].insert(B);
            next_block->comment += ("L" + std::to_string(B->block_id) + ", ");
            //（3）递归调用，搜索它的目标块
            SearchB(next_block);
            //（4）删除当前块中跳转指令之后的所有指令
            if(std::next(it) != B->Instruction_list.end())
                B->Instruction_list.erase(std::next(it), B->Instruction_list.end());
            return;
        }
        //【2】条件跳转指令（与【1】类似，但是有两个目标块，都要维护）
        else if(intr->GetOpcode()==BasicInstruction::LLVMIROpcode::BR_COND){
            Operand operand1 = ((BrCondInstruction*)intr)->GetTrueLabel();
            Operand operand2 = ((BrCondInstruction*)intr)->GetFalseLabel();
            int true_label = ((LabelOperand*)operand1)->GetLabelNo();
            int false_label = ((LabelOperand*)operand2)->GetLabelNo();
            LLVMBlock true_block = (*block_map)[true_label];
            LLVMBlock false_block = (*block_map)[false_label];

            G[B->block_id].insert(true_block);
            invG[true_label].insert(B);
            //true_block->comment += ("L" + std::to_string(B->block_id) + ", ");

            G[B->block_id].insert(false_block);
            invG[false_label].insert(B);
            //false_block->comment += ("L" + std::to_string(B->block_id) + ", ");

            SearchB(true_block);
            SearchB(false_block);

            if(std::next(it) != B->Instruction_list.end())
                B->Instruction_list.erase(std::next(it), B->Instruction_list.end());
            return;
        }
        //ret指令
        else if(intr->GetOpcode()==BasicInstruction::LLVMIROpcode::RET){
            //（1）删除当前块中跳转指令之后的所有指令
            if(std::next(it) != B->Instruction_list.end())
                B->Instruction_list.erase(std::next(it), B->Instruction_list.end());
            return;
        }
    }
}

void CFG::BuildCFG() {
    G.clear();
    invG.clear();
    //G.resize(max_label + 1);
    //invG.resize(max_label + 1);
    
    // 重置dfs编号
    dfs_num = 0;
    for (auto [id, block] : *block_map) {
        block->dfs_id = 0;
    }

    // 深度优先遍历块内的所有指令，遇到跳转指令记录前置块和后缀块
    // 如果跳转/返回指令之后还有指令，全部删除
    // SearchB()进行递归调用
    LLVMBlock start_block = (*block_map)[0];
    SearchB(start_block);
}

std::set<LLVMBlock> CFG::GetPredecessor(LLVMBlock B) { return invG[B->block_id]; }

std::set<LLVMBlock> CFG::GetPredecessor(int bbid) { return invG[bbid]; }

std::set<LLVMBlock> CFG::GetSuccessor(LLVMBlock B) { return G[B->block_id]; }

std::set<LLVMBlock> CFG::GetSuccessor(int bbid) { return G[bbid]; }

void CFG::GetSSAGraphAllSucc(std::set<int>& succs, int regno){ 
    if(succs.count(regno) > 0) return;
    auto it = SSA_Graph.find(regno);
    if(it != SSA_Graph.end()){
        for(auto &s:it->second){
            succs.insert(s);
            GetSSAGraphAllSucc(succs,s);
        }
    }
    return ;
}

void CFG::display(bool reverse) {
    std::cout << "\n=== Control Flow Graph Information ===" << std::endl;
    if (!reverse) {
        // 显示后继块信息
        for(size_t i = 0; i < this->G.size(); i++) {
            if(this->G[i].empty()) continue;
            std::cout << "Block " << i << " 的后继块: ";
            for(auto succ : this->G[i]) {
                std::cout << succ->block_id << " ";
            }
            std::cout << std::endl;
        }
    } else {
        // 显示前驱块信息
        for(size_t i = 0; i < this->invG.size(); i++) {
            if(this->invG[i].empty()) continue;
            std::cout << "Block " << i << " 的前驱块: ";
            for(auto pred : this->invG[i]) {
                std::cout << pred->block_id << " ";
            }
            std::cout << std::endl;
        }
    }
    std::cout << "=== End of Control Flow Graph ===\n" << std::endl;
}
