#include "adce.h"

Instruction ADCEPass::GetTerminal(CFG *C, int label){
    Instruction intr = (*C->block_map)[label]->Instruction_list.back();
    return intr;
}

void ADCEPass::ADCE(CFG *C){
    // std::cout<<" blocks rest:"<<C->block_map->size()<<" 个 。";
    // for(auto &[id, block]: *(C->block_map)){
    //     std::cout<<id<<" ";
    // }std::cout<<std::endl;

    std::set<Instruction> worklist;
    std::map<Instruction, int> intr_bbid_map; // intr --> block_id
    std::map<int, Instruction> defmap; //def_regno --> intr

    // 过滤出有用的指令
    for(int i=0; i<C->block_map->size(); i++){
        if((*C->block_map)[i]==nullptr) // 在之前的cfg基础优化中会删除块，所以要检查是否为空
            continue;
        for(auto& intr: (*C->block_map)[i]->Instruction_list){
            intr_bbid_map[intr] = i;
            // 将所有有效指令放入worklist
            if(intr->GetOpcode()==BasicInstruction::LLVMIROpcode::STORE
                || intr->GetOpcode()==BasicInstruction::LLVMIROpcode::RET
                || intr->GetOpcode()==BasicInstruction::LLVMIROpcode::CALL){
                worklist.insert(intr);
            }
            // 收集所有def
            else{
                int regno = intr->GetDefRegno();
                if(regno!=-1)
                    defmap[regno]=intr;
            }
        }
    }

    while(!worklist.empty()){
        auto iter = worklist.begin();
        Instruction intr = *iter;

        worklist.erase(iter);
        live.insert(intr); // 添加活跃指令
        live_block.insert(intr_bbid_map[intr]); // 添加活跃块

        // phi指令的每一条前驱都应该被当作活跃的:前驱block和前驱block的最后一条指令br
        if(intr->GetOpcode()==BasicInstruction::LLVMIROpcode::PHI){
            auto phiI = (PhiInstruction*)intr;
            for(auto& phi_pair: phiI->GetPhiList()){
                int label = ((LabelOperand*)phi_pair.first)->GetLabelNo();
                // std::cout<<"get terminal: "<<label<<std::endl;
                // if(C->block_map->find(label)==C->block_map->end()){
                //     std::cout<<"label not found: "<<label<<std::endl;
                //     continue;
                // }
                Instruction terminal = GetTerminal(C, label);
                if(worklist.find(terminal)==worklist.end() && live.find(terminal)==live.end()){
                    worklist.insert(terminal);
                }
                live_block.insert(label);
            }
        }

        // 加入当前块的全部控制依赖前驱（利用CDG）
        for(auto& bbid: domtrees->GetDomTree(C)->DF_map[intr_bbid_map[intr]]){
            Instruction terminal = GetTerminal(C, bbid);
            if(worklist.find(terminal)==worklist.end() && live.find(terminal)==live.end()){
                worklist.insert(terminal);
            }
        }

        // 对每个use的变量，将其def加入worklist
        std::set<int> use_list = intr->GetUseRegno();
        for(auto& use_regno: use_list){
            if(defmap.find(use_regno)!=defmap.end() && live.find(defmap[use_regno])==live.end()){
                worklist.insert(defmap[use_regno]);
            }
        }
    }
}

void ADCEPass::CleanUnlive(CFG *C){
    // 从block_map中删除不活跃的块
    live_block.insert(0); // 保证第一个块永远存在
    std::set<int> unlive_block;
    for(auto &[id, block]: *(C->block_map)){
        if(live_block.find(id)==live_block.end()){
            unlive_block.insert(id);
        }
    }
    for(auto& ulb: unlive_block){
        C->block_map->erase(ulb);
    }

    // 删除不活跃的指令
    for(auto &[id, block]: *(C->block_map)){
        std::deque<Instruction> old_Intrs = block->Instruction_list;
        block->Instruction_list.clear();

        for(auto& intr: old_Intrs){
            // 检查跳转label对应块是否会被删除，如果是则需要寻找后继块补充，跳转指令不可以缺失
            if(intr->GetOpcode()==BasicInstruction::LLVMIROpcode::BR_COND){
                BrCondInstruction* br_intr = (BrCondInstruction*)intr;
                int true_label = ((LabelOperand*)br_intr->GetTrueLabel())->GetLabelNo();
                int false_label = ((LabelOperand*)br_intr->GetFalseLabel())->GetLabelNo();
                // 对应块被删除， 在live_block中找不到
                if(live_block.find(true_label)==live_block.end()){
                    while(live_block.find(true_label)==live_block.end()){
                        true_label = domtrees->GetDomTree(C)->sdom_map[true_label];
                    }
                    br_intr->ChangeTrueLabel(GetNewLabelOperand(true_label));
                }
                if(live_block.find(false_label)==live_block.end()){
                    while(live_block.find(false_label)==live_block.end()){
                        false_label = domtrees->GetDomTree(C)->sdom_map[false_label];
                    }
                    br_intr->ChangeFalseLabel(GetNewLabelOperand(false_label));
                }

                // 检查两个label是否相同，相同就把指令替换为uncond
                if(true_label==false_label){
                    Instruction new_br_uncond = new BrUncondInstruction(GetNewLabelOperand(true_label));
                    block->Instruction_list.push_back(new_br_uncond);
                }
                else{
                    block->Instruction_list.push_back(intr);
                }
            }
            else if(intr->GetOpcode()==BasicInstruction::LLVMIROpcode::BR_UNCOND){
                BrUncondInstruction* br_intr = (BrUncondInstruction*)intr;
                int dest_label = ((LabelOperand*)br_intr->GetDestLabel())->GetLabelNo();
                // 对应块被删除， 在live_block中找不到
                if(live_block.find(dest_label)==live_block.end()){
                    while(live_block.find(dest_label)==live_block.end()){
                        dest_label = domtrees->GetDomTree(C)->sdom_map[dest_label];
                    }
                    br_intr->ChangeDestLabel(GetNewLabelOperand(dest_label));
                }
                block->Instruction_list.push_back(intr);
            }
            else{
                // 发现是活跃的指令
                if(live.find(intr)!=live.end()){
                    block->Instruction_list.push_back(intr);
                }
            }
        }
    }
}

void ADCEPass::Execute(){
    for (auto [defI, cfg] : llvmIR->llvm_cfg){
        live.clear();
        live_block.clear();
        ADCE(cfg);
        CleanUnlive(cfg);
    }
}