#include "weaken_mem2reg.h"
#include <iostream>
#include <tuple>
#include <unordered_map>
#include <functional>

void WeakenMem2RegPass::WeakenMem2Reg(CFG *C){
    EliminateUseDefSameBlockLoadStore(C);
    EliminateNoUseAlloca(C);
}

void WeakenMem2RegPass::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        WeakenMem2Reg(cfg);
    }
}

void WeakenMem2RegPass::EliminateNoUseAlloca(CFG *C){
    std::set<int> allocaset;
    std::set<int> useallocaset;
    std::set<Instruction> EraseSet;
    std::map<int, int> UnionFindMap;
    std::function<int(int)> UnionFind = [&](int RegToFindNo) -> int {
        if (UnionFindMap[RegToFindNo] == RegToFindNo)
            return RegToFindNo;
        return UnionFindMap[RegToFindNo] = UnionFind(UnionFindMap[RegToFindNo]);
    };
    auto Connect = [&](Operand resultOp, Operand replaceOp) -> void {
        auto Reg1 = (RegOperand *)resultOp;
        auto Reg1no = Reg1->GetRegNo();
        auto Reg0 = (RegOperand *)replaceOp;
        auto Reg0no = Reg0->GetRegNo();
        UnionFindMap[UnionFind(Reg1no)] = UnionFind(Reg0no);
    };

    if (C->max_reg <= 0) {
        return;
    }
    for (int i = 0; i <= C->max_reg; ++i) {
        UnionFindMap[i] = i;
    }

    for(auto [id, bb] : *C->block_map){
        for(auto I : bb->Instruction_list){
            if(I->GetOpcode() == BasicInstruction::ALLOCA){
                auto allocaI = (AllocaInstruction*)I;
                allocaset.insert(((RegOperand*)I->GetResult())->GetRegNo());
            }else if(I->GetOpcode() == BasicInstruction::LOAD){
                auto loadI = (LoadInstruction*)I;
                auto ptrop = loadI->GetPointer();
                auto ptrreg = (RegOperand*)ptrop;
                auto ptrregno = ptrreg->GetRegNo();
                // std::cerr<<ptrregno<<'\n';
                // I->PrintIR(std::cerr);
                Connect(I->GetResult(), ptrop);
            }else if(I->GetOpcode() == BasicInstruction::GETELEMENTPTR){
                auto gepI = (GetElementptrInstruction*)I;
                auto ptrop = gepI->GetPtrVal();
                auto ptrreg = (RegOperand*)ptrop;
                auto ptrregno = ptrreg->GetRegNo();
                Connect(I->GetResult(), ptrop);
            }
        }
    }

    for (int i = 0; i <= C->max_reg; ++i) {
        UnionFindMap[i] = UnionFind(i);
        if(i != UnionFindMap[i] && allocaset.find(UnionFindMap[i]) != allocaset.end() && useallocaset.find(UnionFindMap[i]) == useallocaset.end()){
            useallocaset.insert(UnionFindMap[i]);
        }
    }
    for(auto [id, bb] : *C->block_map){
        for(auto I : bb->Instruction_list){
            if(I->GetOpcode() == BasicInstruction::ALLOCA){
                auto allocaI = (AllocaInstruction*)I;
                auto resultop = allocaI->GetResult();
                auto resultreg = (RegOperand*)resultop;
                auto resultregno = resultreg->GetRegNo();
                if(useallocaset.find(resultregno) == useallocaset.end()){
                    EraseSet.insert(I);
                }
            }else{
                // I->PrintIR(std::cerr);
                for(auto op : I->GetNonResultOperands()){
                    if(op->GetOperandType() == BasicOperand::REG){
                        auto reg = (RegOperand*)op;
                        auto regno = UnionFindMap[reg->GetRegNo()];
                        if(allocaset.find(regno) != allocaset.end() && useallocaset.find(regno) == useallocaset.end()){
                            EraseSet.insert(I);
                            break;
                        }
                    }
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

void WeakenMem2RegPass::EliminateUseDefSameBlockLoadStore(CFG *C){
    
    std::set<Instruction> EraseSet;
    std::unordered_map<int, int> allocamap;
    std::unordered_map<int, Instruction> usemap;
    std::map<int,int> replacemap;


    std::map<int, int> UnionFindMap;
    std::function<int(int)> UnionFind = [&](int RegToFindNo) -> int {
        if (UnionFindMap[RegToFindNo] == RegToFindNo)
            return RegToFindNo;
        return UnionFindMap[RegToFindNo] = UnionFind(UnionFindMap[RegToFindNo]);
    };
    auto Connect = [&](Operand resultOp, Operand replaceOp) -> void {
        auto Reg1 = (RegOperand *)resultOp;
        auto Reg1no = Reg1->GetRegNo();
        auto Reg0 = (RegOperand *)replaceOp;
        auto Reg0no = Reg0->GetRegNo();
        UnionFindMap[UnionFind(Reg1no)] = UnionFind(Reg0no);
    };

    if (C->max_reg <= 0) {
        return;
    }
    for (int i = 0; i <= C->max_reg; ++i) {
        UnionFindMap[i] = i;
    }

    for(auto [id,bb] : *C->block_map){
        for(auto I : bb->Instruction_list){
            if(I->GetOpcode() == BasicInstruction::ALLOCA){
                auto allocaI = (AllocaInstruction*)I;
                if(allocaI->GetDims().empty()){
                    allocamap[((RegOperand*)I->GetResult())->GetRegNo()] = -1;
                }
            }
            if(I->GetResult() != nullptr){
                auto resultop = I->GetResult();
                auto resultreg = (RegOperand*)resultop;
                auto resultregno = resultreg->GetRegNo();
                usemap[resultregno] = I;
            }
            if(I->GetOpcode() == BasicInstruction::LOAD){
                auto loadI = (LoadInstruction*)I;
                auto ptrop = loadI->GetPointer();
                if(ptrop->GetOperandType() != BasicOperand::REG){
                    continue;
                }
                auto ptrreg = (RegOperand*)ptrop;
                auto ptrregno = ptrreg->GetRegNo();
                if(allocamap.find(ptrregno) != allocamap.end()){
                    if(allocamap[ptrregno] != -1 && allocamap[ptrregno] != id){
                        allocamap.erase(ptrregno);
                        
                    }else{
                        // I->PrintIR(std::cerr);
                        allocamap[ptrregno] = id;
                        // std::cerr<<ptrregno<<'\n';
                    }
                }
            }
            if(I->GetOpcode() == BasicInstruction::STORE){
                auto storeI = (StoreInstruction*)I;
                auto ptrop = storeI->GetPointer();
                if(ptrop->GetOperandType() != BasicOperand::REG){
                    continue;
                }
                auto ptrreg = (RegOperand*)ptrop;
                auto ptrregno = ptrreg->GetRegNo();
                if(allocamap.find(ptrregno) != allocamap.end()){
                    if(allocamap[ptrregno] != -1 && allocamap[ptrregno] != id){
                        allocamap.erase(ptrregno);
                        
                    }else{
                        allocamap[ptrregno] = id;
                    }
                }   
            }
            if(I->GetOpcode() == BasicInstruction::GETELEMENTPTR){
                auto gepI = (GetElementptrInstruction*)I;
                auto ptrop = gepI->GetPtrVal();
                if(ptrop->GetOperandType() != BasicOperand::REG){
                    continue;
                }
                auto ptrreg = (RegOperand*)ptrop;
                auto ptrregno = ptrreg->GetRegNo();
                if(allocamap.find(ptrregno) != allocamap.end()){
                    allocamap.erase(ptrregno);
                }   
            }
        }
    }
    auto entrybb = (*C->block_map)[0];
    for(auto I : entrybb->Instruction_list){
        if(I->GetOpcode() != BasicInstruction::ALLOCA){
            break;
        }
        auto allocaI = (AllocaInstruction*)I;
        if(!allocaI->GetDims().empty()){
            continue;
        }
        auto allocaptr = allocaI->GetResult();
        auto allocaptrreg = (RegOperand*)allocaptr;
        auto allocaptrregno = allocaptrreg->GetRegNo();
        if(allocamap.find(allocaptrregno) != allocamap.end()){
            EraseSet.insert(I);
            auto aimbb = (*C->block_map)[allocamap[allocaptrregno]];
            int tempregno = -1;
            for(auto aimI : aimbb->Instruction_list){
                if(aimI->GetOpcode() == BasicInstruction::LOAD){
                    auto loadI = (LoadInstruction*)aimI;
                    auto ptrop = loadI->GetPointer();
                    if(ptrop->GetOperandType() != BasicOperand::REG){
                        continue;
                    }
                    auto ptrreg = (RegOperand*)ptrop;
                    auto ptrregno = ptrreg->GetRegNo();
                    if(ptrregno == allocaptrregno){
                        EraseSet.insert(aimI);
                        auto resultop = loadI->GetResult();
                        auto resultreg = (RegOperand*)resultop;
                        auto resultregno = resultreg->GetRegNo();
                        // replacemap[resultregno] = tempregno;
                        UnionFindMap[resultregno] = tempregno;
                    }
                }
                if(aimI->GetOpcode() == BasicInstruction::STORE){
                    auto storeI = (StoreInstruction*)aimI;
                    auto ptrop = storeI->GetPointer();
                    if(ptrop->GetOperandType() != BasicOperand::REG){
                        continue;
                    }
                    auto ptrreg = (RegOperand*)ptrop;
                    auto ptrregno = ptrreg->GetRegNo();
                    if(ptrregno == allocaptrregno){
                        EraseSet.insert(aimI);
                        auto resultop = storeI->GetValue();
                        auto resultreg = (RegOperand*)resultop;
                        auto resultregno = resultreg->GetRegNo();
                        tempregno = resultregno;
                    }
                }
            }
        }
    }
    for (int i = 0; i <= C->max_reg; ++i) {
        UnionFindMap[i] = UnionFind(i);
    }
    for (auto [id, bb] : *C->block_map) {
        auto tmp_Instruction_list = bb->Instruction_list;
        bb->Instruction_list.clear();
        for (auto &I : tmp_Instruction_list) {
            if (EraseSet.find(I) != EraseSet.end()) {
                continue;
            }
            I->ReplaceRegByMap(UnionFindMap);
            bb->InsertInstruction(1, I);
        }
    }
}
