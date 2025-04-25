#include "simplify_cfg.h"
#include <iostream>
#include <functional>

void SimplifyCFGPass::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        cfg->BuildCFG();
        EliminateUnreachedBlocksInsts(cfg);
        EliminateUselessPhi(cfg);
        EliminateDoubleUnBrUnCond(cfg);
        for(auto &[id,bb] : *cfg->block_map){
            for(auto &I : bb->Instruction_list){
                I->SetBlockID(id);
            }
        }
    }
}

// 删除从函数入口开始到达不了的基本块和指令
// 不需要考虑控制流为常量的情况，你只需要从函数入口基本块(0号基本块)开始dfs，将没有被访问到的基本块和指令删去即可
static std::map<int,bool> vis;

static void EliminateUnusedBlock(CFG *C);
static void EliminateUnusedBlockDFS(CFG *C, int ubbid);
static void EliminateUnusedInsts(CFG *C);
void SimplifyCFGPass::EliminateUnreachedBlocksInsts(CFG *C) { 
    EliminateUnusedBlock(C);
    EliminateUnusedInsts(C);
    // TODO("EliminateUnreachedBlocksInsts"); 
}

static void EliminateUnusedInsts(CFG *C){
    // std::set<int> defset;
    std::set<int> useset;
    std::set<Instruction> EraseSet;

    for(auto [id,bb] : *C->block_map){
        for(auto I : bb->Instruction_list){
            // if(I->GetResult() != nullptr){
            //     auto resultop = I->GetResult();
            //     auto resultreg = (RegOperand*)resultop;
            //     auto resultregno = resultreg->GetRegNo();
            //     defset.insert(resultregno);
            // }
            for(auto op : I->GetNonResultOperands()){
                if(op->GetOperandType() == BasicOperand::REG){
                    auto reg = (RegOperand*)op;
                    auto regno = reg->GetRegNo();
                    if(useset.find(regno) == useset.end()){
                        // I->PrintIR(std::cerr);
                        // std::cerr<<regno<<'\n';
                        useset.insert(regno);
                    }
                }
            }
        }
    }
    for(auto [id, bb] : *C->block_map){
        for(auto I : bb->Instruction_list){
            if(I->GetResult() != nullptr){
                if(I->GetOpcode() == BasicInstruction::CALL){
                    break;
                }
                auto resultop = I->GetResult();
                auto resultreg = (RegOperand*)resultop;
                auto resultregno = resultreg->GetRegNo();
                if(useset.find(resultregno) == useset.end()){
                    EraseSet.insert(I);
                }
            }
        }
    }
    for (auto [id, bb] : *C->block_map) {
        auto tmp_Instruction_list = bb->Instruction_list;
        bb->Instruction_list.clear();
        for (auto I : tmp_Instruction_list) {
            if (EraseSet.find(I) != EraseSet.end()) {
                continue;
            }
            bb->InsertInstruction(1, I);
        }
    }

}


static void EliminateUnusedBlock(CFG *C){
    vis.clear();
    EliminateUnusedBlockDFS(C, 0);
    std::map<int,LLVMBlock> newblockmap;
    for(auto it = C->block_map->begin();it != C->block_map->end(); ++it){
        auto id = it->first;
        auto bb = it->second;
		 // Redundancy [block_map->find(id) != block_map->end()] ?
        if(!(vis.find(id) == vis.end() && C->block_map->find(id) != C->block_map->end())){
            newblockmap.insert(std::make_pair(id,bb));
        }
    }
    C->block_map->clear();
    for(auto it = newblockmap.begin();it != newblockmap.end(); ++it){
        auto id = it->first;
        auto bb = it->second;
        C->block_map->insert(std::make_pair(id,bb));
    }    
    C->BuildCFG();
}

static void EliminateUnusedBlockDFS(CFG *C, int ubbid){
    if(vis.find(ubbid) != vis.end()){
        return;
    }
    vis[ubbid] = 1;
    for(auto vbb : C->G[ubbid]){
        auto vbbid = vbb->block_id;
        EliminateUnusedBlockDFS(C, vbbid);
    }
}

/*
 * this function will eliminate useless phi
 * such as example1:%rx = phi [%ry, %L1], [%ry, %L3]
 * such as example2:%rx = phi [5, %L5]
 */
// int UnionFind(int RegToFind,std::unordered_map<int,int> UnionFindMap){
//     if(UnionFindMap[RegToFind]==RegToFind)return RegToFind;
// 	return UnionFindMap[RegToFind]=UnionFind(UnionFindMap[RegToFind],UnionFindMap);
// }
void SimplifyCFGPass::EliminateUselessPhi(CFG *C) {
    std::unordered_map<int, Operand> UnionFindMap;
    std::set<Instruction> EraseSet;
    bool changed = true;
    auto FuncdefI = C->function_def;
    std::function<Operand(Operand)> UnionFind = [&](Operand RegToFind) -> Operand {
        auto RegToFindNo = ((RegOperand *)RegToFind)->GetRegNo();
        if (UnionFindMap[RegToFindNo] == RegToFind)
            return RegToFind;
        return UnionFindMap[RegToFindNo] = UnionFind(UnionFindMap[RegToFindNo]);
    };
    // init UnionFind
    for (auto [id, bb] : *C->block_map) {
        for (auto I : bb->Instruction_list) {
            if (I->GetOpcode() != BasicInstruction::PHI) {
                continue;
            }
            auto PhiI = (PhiInstruction *)I;
            auto NonResultOperands = PhiI->GetNonResultOperands();
            auto ResultReg = PhiI->GetResult();
            auto ResultRegNo = ((RegOperand *)ResultReg)->GetRegNo();
            UnionFindMap[ResultRegNo] = ResultReg;
            for (u_int32_t i = 0; i < NonResultOperands.size(); ++i) {
                auto NonResultReg = NonResultOperands[i];
                if (NonResultReg->GetOperandType() != BasicOperand::REG) {
                    continue;
                }
                // std::cerr<<NonResultReg->GetFullName()<<" "<<NonResultReg->GetFullName()<<'\n';
                auto NonResultRegNo = ((RegOperand *)NonResultOperands[i])->GetRegNo();
                UnionFindMap[NonResultRegNo] = NonResultReg;
            }
        }
    }

    while (changed) {
        changed = false;
        for (auto [id, bb] : *C->block_map) {
            for (auto &I : bb->Instruction_list) {
                if (I->GetOpcode() != BasicInstruction::PHI) {
                    continue;
                }
                auto PhiI = (PhiInstruction *)I;
                auto ResultOperands = PhiI->GetNonResultOperands();
                auto ResultReg = PhiI->GetResult();
                auto ResultRegNo = ((RegOperand *)ResultReg)->GetRegNo();
                bool NeedtoReleace = 1;
                bool isallreg = (ResultOperands[0]->GetOperandType() == BasicOperand::REG);
                for (u_int32_t i = 1; i < ResultOperands.size(); ++i) {
                    if (ResultOperands[i]->GetFullName() != ResultOperands[i - 1]->GetFullName()) {
                        NeedtoReleace = 0;
                        // break;
                    }
                    if (ResultOperands[i]->GetOperandType() != BasicOperand::REG) {
                        isallreg = 0;
                    }
                }

                if (NeedtoReleace) {

                    if (ResultOperands.size() == 1 && !isallreg) {
                        // example2
                        if (ResultOperands[0]->GetOperandType() == BasicOperand::IMMI32) {
                            changed |= true;
                            I = new ArithmeticInstruction(
                            BasicInstruction::ADD, BasicInstruction::I32, new ImmI32Operand(0),
                            new ImmI32Operand(((ImmI32Operand *)ResultOperands[0])->GetIntImmVal()),
                            PhiI->GetResult());
                        } else if (ResultOperands[0]->GetOperandType() == BasicOperand::IMMF32) {
                            changed |= true;
                            I = new ArithmeticInstruction(
                            BasicInstruction::FADD, BasicInstruction::FLOAT32, new ImmF32Operand(0),
                            new ImmF32Operand(((ImmF32Operand *)ResultOperands[0])->GetFloatVal()),
                            PhiI->GetResult());
                        }
                    } else if (isallreg) {
                        // example1
                        changed |= true;
                        EraseSet.insert(I);
                        auto Findfa = UnionFind(ResultOperands[0]);
                        auto Findson = UnionFind(ResultReg);
                        auto FindsonNo = ((RegOperand *)Findson)->GetRegNo();
                        UnionFindMap[FindsonNo] = Findfa;
                    }
                }
            }
        }
        for (auto [id, bb] : *C->block_map) {
            auto tmp_Instruction_list = bb->Instruction_list;
            bb->Instruction_list.clear();
            for (auto I : tmp_Instruction_list) {
                if (EraseSet.find(I) != EraseSet.end()) {
                    continue;
                }
                bb->InsertInstruction(1, I);
            }
        }
        for (auto [id, bb] : *C->block_map) {
            for (auto I : bb->Instruction_list) {
                auto NonResultOperands = I->GetNonResultOperands();
                bool Change = 0;
                for (u_int32_t i = 0; i < NonResultOperands.size(); ++i) {
                    if (NonResultOperands[i]->GetOperandType() != BasicOperand::REG) {
                        continue;
                    }
                    auto NonResultOperandsno = ((RegOperand *)NonResultOperands[i])->GetRegNo();
                    if (UnionFindMap.find(NonResultOperandsno) == UnionFindMap.end() ||
                        UnionFindMap[NonResultOperandsno] == NonResultOperands[i]) {
                        continue;
                    }
                    Change = 1;
                    auto Findfa = UnionFind(UnionFindMap[NonResultOperandsno]);
                    auto FindfaNo = ((RegOperand *)Findfa)->GetRegNo();
                    NonResultOperands[i] = GetNewRegOperand(FindfaNo);
                }
                if (Change) {
                    I->SetNonResultOperands(NonResultOperands);
                }
            }
        }
    }
}

/**
    * this function will eliminate the double br_uncond
    *
    * example:
    * L1:
        LIST1
        br label %L2
      L2:
        LIST2
        br label %L3
      L3:
        LIST3
        br label %L4

    * the example after this function will be:
    * L1:
        LIST1
        LIST2
        LIST3
        br label %L4

    * this pass will be useful after sccp
    * you can use testcase 29_lone_line.sy to check
    * @param C the control flow graph of the function */
void SimplifyCFGPass::EliminateDoubleUnBrUnCond(CFG *C){
    std::vector<std::vector<LLVMBlock>> &G = C->G;
    std::vector<std::vector<LLVMBlock>> &invG = C->invG;
    std::map<int, int> vsd;
    std::map<int, int> PhiMap;
    std::map<int, int> OtherPhiMap;
    std::stack<LLVMBlock> bbstack;
    bool changed = true;
    // auto FuncdefI = C->function_def;
    // FuncdefI->PrintIR(std::cerr);
    while (changed) {
        changed = false;
        bbstack.push(C->block_map->begin()->second);
        for (auto [id, bb] : *C->block_map) {
            vsd[id] = 0;
        }
        while (!bbstack.empty()) {
            auto bbu = bbstack.top();
            auto uid = bbu->block_id;
            bbstack.pop();
            vsd[uid] = 1;
            
            for (auto bbv : G[uid]) {
                // bbv->printIR(std::cerr);
                int vid = bbv->block_id;
                if (vsd[vid] == 1) {
                    continue;
                }
                if (uid == 0) {
                    bbstack.push(bbv);
                    continue;
                }

                bool check1 = (G[uid].size() >= 1 && G[vid].size() >= 1 && G[uid][0] == bbv && G[vid][0] == bbu);
                bool check2 = (G[uid].size() >= 1 && G[vid].size() > 1 && G[uid][0] == bbv && G[vid][1] == bbu);
                bool check3 = (G[uid].size() > 1 && G[vid].size() >= 1 && G[uid][1] == bbv && G[vid][0] == bbu);
                bool check4 = (G[uid].size() > 1 && G[vid].size() > 1);
                // bbu->printIR(std::cerr);
                // bbv->printIR(std::cerr);
                // std::cerr<<G[uid].size()<<" "<<G[vid].size()<<" "<<check1<<'\n';
                // bool check5 = (invG[vid].size() == 1);
                // check5 = 1;
                bool check = ((!check4) && (check1 || check2 || check3));
                if (check) {
                    int x, y;    // x->v,y->u
                    if (check1) {
                        x = 0;
                        y = 0;
                    } else if (check2) {
                        x = 0;
                        y = 1;
                    } else {
                        x = 1;
                        y = 0;
                    }
                    if (G[vid].size() > 1) {
                        changed |= true;
                        G[uid][x] = bbu;
                        auto inv = G[vid][y ^ 1];
                        auto invid = inv->block_id;
                        for (int i = 0; i < invG[invid].size(); ++i) {
                            if (invG[invid][i] == bbv) {
                                invG[invid][i] = bbu;
                                break;
                            }
                        }
                        PhiMap[vid] = uid;
                        // bbv->Instruction_list.pop_back();
                        // auto endI=bbu->Instruction_list.back();
                        bbu->Instruction_list.pop_back();
                        // bbv->printIR(std::cerr);
                        while (!bbv->Instruction_list.empty()) {
                            bbu->InsertInstruction(1, bbv->Instruction_list.front());
                            bbv->Instruction_list.pop_front();
                        }
                        // bbu->Instruction_list.push_back(endI);
                        G[vid].clear();
                        invG[vid].clear();
                        C->block_map->erase(vid);
                    } else if (bbv->Instruction_list.size() == 1 && bbv->Instruction_list.back()->GetOpcode() != BasicInstruction::RET) {

                        if (G[uid].size() == 1) {
                            continue;
                        }
                        changed |= true;
                        auto endI = (BrCondInstruction *)bbu->Instruction_list.back();
                        bbu->Instruction_list.pop_back();
                        auto trueop = (LabelOperand *)endI->GetTrueLabel();
                        auto falseop = (LabelOperand *)endI->GetFalseLabel();
                        auto trueopno = trueop->GetLabelNo();
                        auto falseopno = falseop->GetLabelNo();
                        // bbv->printIR(std::cerr);
                        if (trueopno == vid) {
                            endI->SetTrueLabel(GetNewLabelOperand(uid));
                        } else {
                            endI->SetFalseLabel(GetNewLabelOperand(uid));
                        }

                        bbu->Instruction_list.push_back(endI);
                        PhiMap[vid] = uid;
                        G[vid].clear();
                        invG[vid].clear();
                        C->block_map->erase(vid);
                    } else {
                        bbstack.push(bbv);
                    }
                } else if (G[uid].size() == 1 && invG[vid].size() == 1) {
                    changed |= true;
                    PhiMap[vid] = uid;
                    // update edge from inv u
                    G[uid].clear();
                    for (int j = 0; j < G[vid].size(); ++j) {
                        auto inv = G[vid][j];
                        auto invid = inv->block_id;
                        G[uid].push_back(inv);
                        for (int i = 0; i < G[invid].size(); ++i) {
                            if (invG[invid][i] == bbv) {
                                invG[invid][i] = bbu;
                                break;
                            }
                        }
                    }
                    // merge u to v
                    bbu->Instruction_list.pop_back();
                    for (auto I : bbv->Instruction_list) {
                        bbu->InsertInstruction(1, I);
                    }
                    G[vid].clear();
                    invG[vid].clear();
                    bbv->Instruction_list.clear();
                    C->block_map->erase(vid);
                    bbstack.push(bbu);
                } else if (G[uid].size() == 2 && invG[vid].size() == 1 && G[vid].size() == 1 &&
                           bbv->Instruction_list.size() == 1 && bbv->Instruction_list.back()->GetOpcode() != BasicInstruction::RET) {
                    auto inv = G[vid][0];
                    auto invid = inv->block_id;
                    auto I = bbu->Instruction_list.back();
                    if (I->GetOpcode() == BasicInstruction::BR_COND &&
                        (((LabelOperand *)((BrCondInstruction *)I)->GetTrueLabel())->GetLabelNo() == invid ||
                         ((LabelOperand *)((BrCondInstruction *)I)->GetFalseLabel())->GetLabelNo() == invid)) {
                        bbstack.push(bbv);
                        continue;
                    }
                    changed |= true;
                    OtherPhiMap[vid] = uid;
                    PhiMap[vid] = invid;
                    if (G[uid][0] == bbv) {
                        G[uid][0] = inv;
                    } else {
                        G[uid][1] = inv;
                    }
                    if (invG[invid][0] == bbv) {
                        invG[invid][0] = bbu;
                    } else {
                        invG[invid][1] = bbu;
                    }
                    auto endI = (BrCondInstruction *)bbu->Instruction_list.back();
                    bbu->Instruction_list.pop_back();
                    auto trueop = (LabelOperand *)endI->GetTrueLabel();
                    auto falseop = (LabelOperand *)endI->GetFalseLabel();
                    auto trueopno = trueop->GetLabelNo();
                    auto falseopno = falseop->GetLabelNo();
                    if (trueopno == vid) {
                        endI->SetTrueLabel(GetNewLabelOperand(invid));
                    } else {
                        endI->SetFalseLabel(GetNewLabelOperand(invid));
                    }
                    bbu->Instruction_list.push_back(endI);
                    G[vid].clear();
                    invG[vid].clear();
                    bbv->Instruction_list.clear();
                    C->block_map->erase(vid);
                    bbstack.push(bbu);
                } else {
                    bbstack.push(bbv);
                }
            }
        }
        
        for (auto [id, bb] : *C->block_map) {
            for (auto I : bb->Instruction_list) {
                if (I->GetOpcode() == BasicInstruction::PHI) {
                    auto PhiI = (PhiInstruction *)I;
                    auto ResultOperands = PhiI->GetPhiList();
                    for (u_int32_t i = 0; i < ResultOperands.size(); ++i) {
                        auto Labelop = (LabelOperand *)ResultOperands[i].first;
                        auto oldop = Labelop->GetLabelNo();
                        auto Labelopno = Labelop->GetLabelNo();
                        if (OtherPhiMap.find(Labelopno) != OtherPhiMap.end()) {
                            Labelopno = OtherPhiMap[Labelopno];
                            while (PhiMap.find(Labelopno) != PhiMap.end()) {
                                Labelopno = PhiMap[Labelopno];
                            }
                            PhiI->SetNewLabelFrom(oldop, Labelopno);
                        } else if (PhiMap.find(Labelopno) != PhiMap.end()) {
                            while (PhiMap.find(Labelopno) != PhiMap.end()) {
                                Labelopno = PhiMap[Labelopno];
                            }
                            PhiI->SetNewLabelFrom(oldop, Labelopno);
                        }
                    }
                } else if (I->GetOpcode() == BasicInstruction::BR_COND) {
                    auto brcondI = (BrCondInstruction *)I;
                    auto trueop = (LabelOperand *)brcondI->GetTrueLabel();
                    auto falseop = (LabelOperand *)brcondI->GetFalseLabel();
                    auto trueopno = trueop->GetLabelNo();
                    auto falseopno = falseop->GetLabelNo();
                    if (PhiMap.find(trueopno) != PhiMap.end()) {
                        while (PhiMap.find(trueopno) != PhiMap.end()) {
                            trueopno = PhiMap[trueopno];
                        }
                        brcondI->SetTrueLabel(GetNewLabelOperand(trueopno));
                    }
                    if (PhiMap.find(falseopno) != PhiMap.end()) {
                        while (PhiMap.find(falseopno) != PhiMap.end()) {
                            falseopno = PhiMap[falseopno];
                        }
                        brcondI->SetFalseLabel(GetNewLabelOperand(falseopno));
                    }
                } else if (I->GetOpcode() == BasicInstruction::BR_UNCOND) {
                    auto bruncondI = (BrUncondInstruction *)I;
                    auto Labelop = (LabelOperand *)bruncondI->GetDestLabel();
                    auto Labelopno = Labelop->GetLabelNo();
                    if (PhiMap.find(Labelopno) == PhiMap.end()) {
                        continue;
                    }
                    while (PhiMap.find(Labelopno) != PhiMap.end()) {
                        Labelopno = PhiMap[Labelopno];
                    }
                    bruncondI->SetTarget(GetNewLabelOperand(Labelopno));
                }
            }
        }
        OtherPhiMap.clear();
        PhiMap.clear();
    }
    // puts("PUITASD");
    int cnt = -1;
    std::map<int, int> NewMap;
    for (auto [id, bb] : *C->block_map) {
        NewMap[id] = ++cnt;
    }

    for (auto [id, bb] : *C->block_map) {
        for (auto I : bb->Instruction_list) {
            if (I->GetOpcode() == BasicInstruction::PHI) {
                auto PhiI = (PhiInstruction *)I;
                auto ResultOperands = PhiI->GetPhiList();
                std::set<int> ReplaceSet;
                for (u_int32_t i = 0; i < ResultOperands.size(); ++i) {
                    auto Labelop = (LabelOperand *)ResultOperands[i].first;
                    auto Labelopno = Labelop->GetLabelNo();
                    if (NewMap.find(Labelopno) != NewMap.end()) {
                        ReplaceSet.insert(Labelopno);
                    }
                }
                for (auto it = ReplaceSet.begin(); it != ReplaceSet.end(); ++it) {
                    PhiI->SetNewLabelFrom(*it, NewMap[*it]);
                }
                ReplaceSet.clear();
            } else if (I->GetOpcode() == BasicInstruction::BR_COND) {
                auto brcondI = (BrCondInstruction *)I;
                auto trueop = (LabelOperand *)brcondI->GetTrueLabel();
                auto falseop = (LabelOperand *)brcondI->GetFalseLabel();
                auto trueopno = trueop->GetLabelNo();
                auto falseopno = falseop->GetLabelNo();
                if (NewMap.find(trueopno) != NewMap.end()) {
                    brcondI->SetTrueLabel(GetNewLabelOperand(NewMap[trueopno]));
                }
                if (NewMap.find(falseopno) != NewMap.end()) {
                    brcondI->SetFalseLabel(GetNewLabelOperand(NewMap[falseopno]));
                }
            } else if (I->GetOpcode() == BasicInstruction::BR_UNCOND) {
                auto bruncondI = (BrUncondInstruction *)I;
                auto Labelop = (LabelOperand *)bruncondI->GetDestLabel();
                auto Labelopno = Labelop->GetLabelNo();
                if (NewMap.find(Labelopno) == NewMap.end()) {
                    continue;
                }
                bruncondI->SetTarget(GetNewLabelOperand(NewMap[Labelopno]));
            }
        }
    }
    std::map<int, LLVMBlock> new_block_map = *C->block_map;
    C->block_map->clear();
    for (auto [id, bb] : new_block_map) {
        bb->block_id = NewMap[bb->block_id];
        C->block_map->insert(std::make_pair(NewMap[id], bb));
        // bb->printIR(std::cerr);
    }
    C->max_label = cnt;
    C->BuildCFG();
    
}