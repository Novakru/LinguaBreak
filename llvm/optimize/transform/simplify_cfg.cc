#include "simplify_cfg.h"
#include "mem2reg.h"
#include "assert.h"

void SimplifyCFGPass::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        EliminateUnreachedBlocksInsts(cfg);
    }
}
void SimplifyCFGPass::EOBB() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        EliminateOneBrUncondBlocks(cfg);
    }
}
void SimplifyCFGPass::EOPPhi() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        for(auto &[bbid, bb] : *(cfg->block_map)) {
            bb->dfs_id = 0;
        }
        std::unordered_set<int> regno_tobedeleted{};
        LLVMBlock zero_block = (*(cfg->block_map))[0];
        EliminateOnePredPhi(cfg,zero_block ,regno_tobedeleted);
    }
}
//1. 删除不可达基本块（不可达指令已在build cfg中删除）
void SimplifyCFGPass::EliminateUnreachedBlocksInsts(CFG *C) {
    //(1)删除不可达基本块
    for (auto it = C->block_map->begin(); it != C->block_map->end(); ) {
        if (!(it->second->dfs_id)) {
            it=C->block_map->erase(it); // 删除当前元素
        }else{
            C->block_ids.insert(it->first); // 记录当前块id
            ++it; // 仅在未删除元素时移向下一个
        }
    }
}
void SimplifyCFGPass::EliminateNotExistPhi(CFG *C) {
    //(2)清理不可达块后继中的phi指令:前提：需要保证当前phi指令中的冗余label皆是不可达块
    for (auto &[bbid, bb] : *(C->block_map)) {
        auto old_intr_list = bb->Instruction_list;
        bb->Instruction_list.clear();
        for (auto &intr : old_intr_list) {
            if (intr->GetOpcode() == BasicInstruction::LLVMIROpcode::PHI) {
                PhiInstruction *phi_intr = (PhiInstruction *)intr;
                auto phi_list = phi_intr->GetPhiList();
                std::vector<std::pair<Operand, Operand>> new_phi_list;
                for (const auto &pair : phi_list) {
                    int label_no = ((LabelOperand *)pair.first)->GetLabelNo();
                    if (C->block_ids.count(label_no) > 0) {
                        // 如果label存在于当前CFG的块中，则保留
                        new_phi_list.push_back(pair);
                    }
                }
                // 剩余phi_list不可能为0，否则其为不可达块，也不存在了
                assert(new_phi_list.size()>0);
                bool still_phi=false;
                //[1] 剩余phi_list>=2，且不同
                if(new_phi_list.size()>=2){
                    Operand ave_operand=new_phi_list[0].second;
                    for(int i=1;i<new_phi_list.size();i++){
                        if(phi_intr->NotEqual(new_phi_list[i].second,ave_operand)){
                            still_phi=true;
                            break;
                        }
                    }
                }
                if(still_phi){
                    phi_intr->SetPhiList(new_phi_list);
                    bb->Instruction_list.push_back(intr);
                //[2] 剩余phi_list>=2，且均相同； 或剩余phi_list=1
                }else{
                    Operand phi_source_op=new_phi_list[0].second;
                    int def_regno=phi_intr->GetDefRegno();
                    auto it=C->use_map.find(def_regno);
                    if(it!=C->use_map.end()){
                        //对于所有用到此phi_def_reg的指令，将操作数替换，并删除此phi指令
                        std::vector<Instruction> intrs=it->second;
                        for(auto &intr:intrs){
                            auto operands=intr->GetNonResultOperands();
                            std::vector<Operand> new_operands{};
                            for(auto &operand:operands){
                                if(operand->GetOperandType()==BasicOperand::REG){
                                    int use_regno=((RegOperand*)operand)->GetRegNo();
                                    if(use_regno==def_regno){
                                        //匹配上了，替换值
                                        new_operands.push_back(phi_source_op);
                                    }else{
                                        new_operands.push_back(operand);
                                    }
                                }else{
                                    new_operands.push_back(operand);
                                }
                            }
                            intr->SetNonResultOperands(new_operands);
                        }
                    }
                }
            }else{
                bb->Instruction_list.push_back(intr);
            }
        }
    }

}
void SimplifyCFGPass::RebuildCFG(){
    for (auto &[defI,cfg] : llvmIR->llvm_cfg) {
        // 重建CFG（遍历cfg，重建G与invG)
        cfg->BuildCFG();
        // 消除不可达块
        EliminateUnreachedBlocksInsts(cfg);
        // 处理随不可达块受影响的phi指令
        EliminateNotExistPhi(cfg);
        // // 重建支配树
        // cfg->DomTree = nullptr; // 这里可以调用支配树重建函数
        // cfg->PostDomTree = nullptr; // 这里可以调用后支配树重建函数

    }
}

// 2. 删除只有一条无条件跳转指令的基本块
void SimplifyCFGPass::EliminateOneBrUncondBlocks(CFG *C){
    for(auto it = C->block_map->begin();it!=C->block_map->end();){
        int id=it->first; LLVMBlock block=it->second;
        if(block->Instruction_list.size()==1){
            auto intr=*block->Instruction_list.begin();
            if(intr->GetOpcode()==BasicInstruction::LLVMIROpcode::BR_UNCOND){
                // [1]该block仅一个uncond br 指令，获取其跳转目标label
                BrUncondInstruction* br_intr = (BrUncondInstruction*)intr;
                int dest_label = ((LabelOperand*)br_intr->GetDestLabel())->GetLabelNo();
                //std::cout<<"Find an only-bruncond block: "<<id<<std::endl;
                // [2]寻找该block的所有前驱，将跳转目标为它的br intr全部更新跳转目标
                //std::cout<<" [1] Change the pres' destination to its destination."<<std::endl;
                if(id==0){// [2.1]该block为0号block，无前驱
                    //[2.1](1)有理由认为，其下一个块只此一个入口；将下一个块的内容移动至block[0]（0号块必须存在）

                    //[2.1](2)将被替换块其它前驱的跳转目标更新为0
                    std::set<LLVMBlock> preds = C->GetPredecessor(dest_label);
                    for(auto& pred_block:preds){
                        //std::cout<<"    In pre_block:"<<pred_block->block_id<<" from "<<id<<" to "<<dest_label<<std::endl;
                        std::deque<Instruction> old_Intrs = pred_block->Instruction_list;
                        pred_block->Instruction_list.clear();
                        for(auto& pre_intr: old_Intrs){
                            if(pre_intr->GetOpcode()==BasicInstruction::LLVMIROpcode::BR_COND){
                                BrCondInstruction* pre_br_intr = (BrCondInstruction*)pre_intr;
                                int true_label = ((LabelOperand*)pre_br_intr->GetTrueLabel())->GetLabelNo();
                                int false_label = ((LabelOperand*)pre_br_intr->GetFalseLabel())->GetLabelNo();
                                if(true_label==dest_label){
                                    pre_br_intr->ChangeTrueLabel(GetNewLabelOperand(0));
                                }
                                if(false_label==dest_label){
                                    pre_br_intr->ChangeFalseLabel(GetNewLabelOperand(0));
                                }
                                //若change后两跳转目标相同，则将brcond改为bruncond
                                if(true_label==false_label){
                                    Instruction new_br_uncond = new BrUncondInstruction(GetNewLabelOperand(true_label));
                                    pred_block->Instruction_list.push_back(new_br_uncond);
                                }else{
                                    pred_block->Instruction_list.push_back(pre_intr);
                                }
                            }else if(pre_intr->GetOpcode()==BasicInstruction::LLVMIROpcode::BR_UNCOND){
                                BrUncondInstruction* pre_br_intr = (BrUncondInstruction*)pre_intr;
                                if(((LabelOperand*)pre_br_intr->GetDestLabel())->GetLabelNo()==dest_label){
                                    pre_br_intr->ChangeDestLabel(GetNewLabelOperand(0));
                                    pred_block->Instruction_list.push_back(pre_intr);
                                }
                            }else{
                                pred_block->Instruction_list.push_back(pre_intr);
                            }
                        }
                        //[2.1](2)维护G和invG
                        C->invG[0].insert(pred_block); 
                    }

                    //[2.1](3)将后续phi指令的前驱label更新为0
                    std::set<LLVMBlock> succs = C->GetSuccessor(dest_label);
                    for(auto& succ_block:succs){
                        for(auto &intr:succ_block->Instruction_list){
                            if(intr->GetOpcode()==BasicInstruction::LLVMIROpcode::PHI){
                                PhiInstruction* phi_intr = (PhiInstruction*)intr;
                                auto phi_list = phi_intr->GetPhiList();
                                for(int i=0;i<phi_list.size();i++){
                                    if(((LabelOperand*)phi_list[i].first)->GetLabelNo()==dest_label){
                                        phi_intr->ChangePhiPair(i,std::make_pair(GetNewLabelOperand(0),phi_list[i].second));
                                        break;
                                    }
                                }
                            }
                        }
                        //[2.1](3)维护G和invG
                        C->G[0].insert(succ_block);
                    }
                    C->G[0].erase((*(C->block_map))[dest_label]);
                    C->G.erase(dest_label);
                    C->invG.erase(dest_label);

                    //[2.1](4)替换并删除此块
                    it->second = (*(C->block_map))[dest_label];
                    it->second->block_id = 0;
                    it=(*(C->block_map)).erase(++it);
                    continue;
                }else{// [2.2]该block为有前驱的普通block
                    // //【2.2】（1）将该块的前驱的所有跳转目标更新为该块的跳转目标
                    // std::set<LLVMBlock> preds = C->GetPredecessor(id);
                    // for(auto& pred_block:preds){
                    //     //std::cout<<"    In pre_block:"<<pred_block->block_id<<" from "<<id<<" to "<<dest_label<<std::endl;
                    //     std::deque<Instruction> old_Intrs = pred_block->Instruction_list;
                    //     pred_block->Instruction_list.clear();
                    //     for(auto& pre_intr: old_Intrs){
                    //         if(pre_intr->GetOpcode()==BasicInstruction::LLVMIROpcode::BR_COND){
                    //             BrCondInstruction* pre_br_intr = (BrCondInstruction*)pre_intr;
                    //             int true_label = ((LabelOperand*)pre_br_intr->GetTrueLabel())->GetLabelNo();
                    //             int false_label = ((LabelOperand*)pre_br_intr->GetFalseLabel())->GetLabelNo();
                    //             if(true_label==id){
                    //                 pre_br_intr->ChangeTrueLabel(GetNewLabelOperand(dest_label));
                    //             }
                    //             if(false_label==id){
                    //                 pre_br_intr->ChangeFalseLabel(GetNewLabelOperand(dest_label));
                    //             }
                    //             //若change后两跳转目标相同，则将brcond改为bruncond
                    //             if(true_label==false_label){
                    //                 Instruction new_br_uncond = new BrUncondInstruction(GetNewLabelOperand(true_label));
                    //                 pred_block->Instruction_list.push_back(new_br_uncond);
                    //             }else{
                    //                 pred_block->Instruction_list.push_back(pre_intr);
                    //             }
                    //         }else if(pre_intr->GetOpcode()==BasicInstruction::LLVMIROpcode::BR_UNCOND){
                    //             BrUncondInstruction* pre_br_intr = (BrUncondInstruction*)pre_intr;
                    //             if(((LabelOperand*)pre_br_intr->GetDestLabel())->GetLabelNo()==id){
                    //                 pre_br_intr->ChangeDestLabel(GetNewLabelOperand(dest_label));
                    //                 pred_block->Instruction_list.push_back(pre_intr);
                    //             }
                    //         }else{
                    //             pred_block->Instruction_list.push_back(pre_intr);
                    //         }
                    //     }
                    //     //[2.2](2)维护G和invG
                    //     C->G[pred_block->block_id].insert((*(C->block_map))[dest_label]);
                    //     C->G[pred_block->block_id].erase(block);
                    //     C->invG[dest_label].insert(pred_block); 
                    // }
                    //     C->invG[dest_label].erase(block);
                    //     C->G.erase(id);
                    //     C->invG.erase(id);
                        
                    // //std::cout<<" [2] Change the dests' phi_from into its pre_block"<<std::endl;
                    // // [2.2](3)将该块后继的phi指令的前驱label更新为该块前驱
                    // Operand change_regop;
                    // int pre_id =(*preds.begin())->block_id;

                    // LLVMBlock nextbb=(*(C->block_map))[dest_label];//后继块
                    // for(auto &next_intr:nextbb->Instruction_list){
                    //     if(next_intr->GetOpcode()==BasicInstruction::LLVMIROpcode::PHI){
                    //         //std::cout<<"    In next_block:"<<nextbb->block_id<<" from "<<id<<" to"<<pre_id<<std::endl;
                    //         PhiInstruction* phi_intr = (PhiInstruction*)next_intr;
                    //         auto phi_list = phi_intr->GetPhiList();
                    //         for(int i=0;i<phi_list.size();i++){
                    //             if(((LabelOperand*)phi_list[i].first)->GetLabelNo()==id){
                    //                 //std::cout<<"    In phi: "<<((RegOperand*)phi_list[i].second)->GetRegNo()<<" from "<<id<<" to "<<pre_id<<std::endl;
                    //                 change_regop = phi_list[i].second;
                    //                 phi_intr->ChangePhiPair(i,std::make_pair(GetNewLabelOperand(pre_id),change_regop));
                    //                 break;
                    //             }
                    //         }
                    //         if(preds.size()>1){
                    //             auto it=preds.begin(); it++;
                    //             for(;it!=preds.end();it++){
                    //                 int predblock_id = (*it)->block_id;
                    //                 phi_intr->AddPhi(std::make_pair(GetNewLabelOperand(predblock_id),change_regop));
                    //             }
                    //         }
                    //     }
                    // }

                    // // [4]删除此块
                    // it = C->block_map->erase(it);
                    // continue;
                    break;
                }
            }
        }
        ++it;
    }

}

// 3. 删除只有一个前驱的phi指令
void SimplifyCFGPass::EliminateOnePredPhi(CFG* C,LLVMBlock nowblock,std::unordered_set<int> regno_tobedeleted){
    //std::cout<<" start a block!"<<std::endl;
    if(nowblock->dfs_id>0)return;
    //std::cout<<" we do: "<<std::endl;
    nowblock->dfs_id++;
        //[1]遍历此块的所有指令，处理冗余phi指令
        std::deque<Instruction> old_Intrs = nowblock->Instruction_list;
        nowblock->Instruction_list.clear();
        for(auto& intr: old_Intrs){
            if(intr->GetOpcode()==BasicInstruction::LLVMIROpcode::PHI){
                PhiInstruction* phi_intr = (PhiInstruction*)intr;
                auto phi_list = phi_intr->GetPhiList();
                //[1.1]若此phi指令的源值只有一个，则删除此phi指令
                if(phi_list.size()==1){
                    //删除此phi指令，并记录下其result regno，以便后继也清除
                    int regno = ((RegOperand*)phi_intr->GetResult())->GetRegNo();
                    regno_tobedeleted.insert(regno);
                    //std::cout<<"in block : "<<nowblock->block_id<<" delete phi of "<<regno<<std::endl;
                    continue;
                }else {
                
                    // // [1.2]若此phi指令的源值有多个，但都相同，也删除此phi指令
                    // bool tobedeleted=true;
                    // int sourceregno = ((RegOperand*)phi_list[0].second)->GetRegNo();
                    // for(auto & phi_pair: phi_list){
                    //     if(((RegOperand*)phi_pair.second)->GetRegNo()!=sourceregno){
                    //         //std::cout<<"in block : "<<nowblock->block_id<<" delete phi of "<<sourceregno<<std::endl;
                    //         tobedeleted=false;
                    //         break;
                    //     }
                    // }
                    // if(tobedeleted){
                    //     //删除此phi指令，并记录下其result regno，以便后继也清除
                    //     int regno = ((RegOperand*)phi_intr->GetResult())->GetRegNo();
                    //     regno_tobedeleted.insert(regno);
                    //     //std::cout<<"in block : "<<nowblock->block_id<<" delete phi of "<<regno<<std::endl;
                    //     continue;
                    // }
                    //[1.3]若此phi指令的源值被清除，assert：该phi指令冗余,也删除
                    bool tobedeleted=false;
                    for(int i=0;i<phi_list.size();i++){
                         if(regno_tobedeleted.count(((RegOperand*)phi_list[i].second)->GetRegNo())){
                             int regno=((RegOperand*)phi_intr->GetResult())->GetRegNo();
                             regno_tobedeleted.insert(regno);
                             //std::cout<<"in block : "<<nowblock->block_id<<" delete phi of "<<regno<<std::endl;
                             tobedeleted=true;
                             break;
                         }
                    }
                    if(!tobedeleted){
                        nowblock->Instruction_list.push_back(intr);
                    }
                    continue;
                }
            }
            nowblock->Instruction_list.push_back(intr);
        }
        //[2]递归遍历此块的所有后继
        auto succ_blocks = C->GetSuccessor(nowblock);
        for(auto &succ_block:succ_blocks){
            EliminateOnePredPhi(C,succ_block,regno_tobedeleted);
        }
        return ;
}