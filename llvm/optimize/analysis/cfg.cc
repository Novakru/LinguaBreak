#include "../../include/Instruction.h"
#include "../../include/ir.h"
#include <assert.h>

std::map<std::string, CFG*> CFGMap;

void LLVMIR::CFGInit() {
    for (auto &[defI, bb_map] : function_block_map) {
        CFG *cfg = new CFG();
        cfg->block_map = &bb_map;
        cfg->function_def = defI;
        cfg->max_reg = function_max_reg[defI];
        cfg->max_label = function_max_label[defI];
        cfg->BuildCFG();
        CFGMap[defI->GetFunctionName()] = cfg;
        
        llvm_cfg[defI] = cfg;
    }
}

void LLVMIR::BuildCFG() {
    for (auto [defI, cfg] : llvm_cfg) {
        cfg->BuildCFG();
    }
}

void CFG::BuildCFG() { 
    G.clear();
    invG.clear();
    G.resize(max_label + 2);
    invG.resize(max_label + 2);
    for(auto [id,bb] : *block_map){
        if(bb->Instruction_list.empty()){
            continue;
        }
        for(auto I : bb->Instruction_list){
            if(I->GetOpcode() == BasicInstruction::BR_COND 
                    || I->GetOpcode() == BasicInstruction::BR_UNCOND
                    || I->GetOpcode() == BasicInstruction::RET){
                while(bb->Instruction_list.back() != I){
                    bb->Instruction_list.pop_back();
                }
                break;
            }
        }
    }
    
    for(auto [id,bb] : *block_map){
        if(bb->Instruction_list.empty()){
            continue;
        }
        auto brI = bb->Instruction_list.back();
        if(brI->GetOpcode() == BasicInstruction::BR_COND){
            auto brcondI = (BrCondInstruction*)brI;
            auto truelabel = brcondI->GetTrueLabel();
            auto truebbid = ((LabelOperand*)truelabel)->GetLabelNo();
            auto truebb = (*block_map)[truebbid];
            G[id].push_back(truebb);
            auto falselabel = brcondI->GetFalseLabel();
            auto falsebbid = ((LabelOperand*)falselabel)->GetLabelNo();
            auto falsebb = (*block_map)[falsebbid];
            G[id].push_back(falsebb);
            invG[truebbid].push_back(bb);
            invG[falsebbid].push_back(bb);
        }else if(brI->GetOpcode() == BasicInstruction::BR_UNCOND){
            auto bruncondI = (BrUncondInstruction*)brI;
            auto dstlabel = bruncondI->GetDestLabel();
            auto dstbbid = ((LabelOperand*)dstlabel)->GetLabelNo();
            auto dstbb = (*block_map)[dstbbid];
            G[id].push_back(dstbb);
            invG[dstbbid].push_back(bb);
        }
    }
	// 每次BuildCFG后，BuildDominatorTree确保支配树是最新的
	// this->BuildDominatorTree();
}


LLVMBlock CFG::GetBlock(int bbid) { return (*block_map)[bbid]; }

LLVMBlock CFG::NewBlock() {
    ++max_label;
    (*block_map)[max_label] = new BasicBlock(max_label);
    return GetBlock(max_label);
}


std::vector<LLVMBlock> CFG::GetPredecessor(LLVMBlock B) { return invG[B->block_id]; }

std::vector<LLVMBlock> CFG::GetPredecessor(int bbid) { return invG[bbid]; }

std::vector<LLVMBlock> CFG::GetSuccessor(LLVMBlock B) { return G[B->block_id]; }

std::vector<LLVMBlock> CFG::GetSuccessor(int bbid) { return G[bbid]; }

void CFG::Display() {
    std::cout << "Control Flow Graph (G):" << std::endl;
    for (int i = 0; i < G.size(); ++i) {
        std::cout << "Block " << i << " -> ";
        for (const auto& succ : G[i]) {
            std::cout << succ->block_id << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "Inverse Control Flow Graph (invG):" << std::endl;
    for (int i = 0; i < invG.size(); ++i) {
        std::cout << "Block " << i << " -> ";
        for (const auto& pred : invG[i]) {
            std::cout << pred->block_id << " ";
        }
        std::cout << std::endl;
    }
}