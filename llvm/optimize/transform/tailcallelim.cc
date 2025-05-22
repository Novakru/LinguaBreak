#include "tailcallelim.h"

bool TailCallElimPass::IsTailCallFunc(CFG *C) {
    auto FuncdefI = C->function_def;
    if (FuncdefI->formals.size() > 5) {
        return false;
    }
    // AllocaReg can't be use in call
    auto bb0 = (*C->block_map->begin()).second;
    int AllocaRegCnt = 0;
    std::unordered_map<int, int> AllocaReg;
    std::unordered_map<int, int> GEPMap;
    for (auto I : bb0->Instruction_list) {
        if (I->GetOpcode() != BasicInstruction::ALLOCA) {
            continue;
        }
        auto AllocaI = (AllocaInstruction *)I;
        if (AllocaI->GetDims().empty()) {
            continue;
        }
        AllocaReg[((RegOperand*)AllocaI->GetResult())->GetRegNo()] = ++AllocaRegCnt;
        // std::cout<<"asdads "<<AllocaI->GetResultRegNo()<<'\n';
    }

    // GETELEMENTPTR
    if (!AllocaRegCnt) {
        return true;
    }
    for (auto [id, bb] : *C->block_map) {
        for (auto I : bb->Instruction_list) {
            if (I->GetOpcode() != BasicInstruction::GETELEMENTPTR && I->GetOpcode() != BasicInstruction::CALL) {
                continue;
            }

            if (I->GetOpcode() == BasicInstruction::GETELEMENTPTR) {
                auto GetelementptrI = (GetElementptrInstruction *)I;
                auto PtrReg = ((RegOperand *)GetelementptrI->GetPtrVal())->GetRegNo();
                auto ResultReg = ((RegOperand*)GetelementptrI->GetResult())->GetRegNo();
                if (PtrReg == 0 || AllocaReg.find(PtrReg) == AllocaReg.end()) {
                    continue;
                }
                GEPMap[ResultReg] = PtrReg;
            } else {
                auto CallI = (CallInstruction *)I;
                for (auto args : CallI->GetParameterList()) {
                    auto args_regno = ((RegOperand *)args.second)->GetRegNo();
                    // std::cout<<args_regno<<'\n';
                    if (GEPMap.find(args_regno) != GEPMap.end()) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

void TailCallElimPass::TailCallElim(CFG *C) {
	bool TRECheck = IsTailCallFunc(C);
    if (!TRECheck) {
        return;
    }
    auto FuncdefI = C->function_def;
    auto bb0 = (*C->block_map->begin()).second;
    bool NeedtoInsertPTR = 0;
    std::deque<Instruction> StoreDeque;
    std::deque<Instruction> AllocaDeque;
    std::set<Instruction> EraseSet;
    std::unordered_map<int, RegOperand *> PtrUsed;
    // when exist call ptr, ret
    // FuncdefI->PrintIR(std::cout);
    //@NeedtoInsertPTR:check if ptr in function params need to be insert
    for (auto [id, bb] : *C->block_map) {
        if (bb->Instruction_list.back()->GetOpcode() != BasicInstruction::RET) {
            continue;
        }
        auto retI = (RetInstruction *)bb->Instruction_list.back();
        if (retI == nullptr) {
            continue;
        }
        for (auto I : bb->Instruction_list) {
            if (I->GetOpcode() != BasicInstruction::CALL) {
                continue;
            }
            auto callI = (CallInstruction *)I;
            if (callI == nullptr || callI->GetFunctionName() != FuncdefI->GetFunctionName()) {
                continue;
            }
            bool opt1 =
            callI->GetResult() != NULL && callI->GetResult()->GetFullName() == retI->GetRetVal()->GetFullName();
            bool opt2 = callI->GetResult() == NULL && retI->GetType() == BasicInstruction::LLVMType::VOID;
            if (opt1 || opt2) {
                auto list_size = callI->GetParameterList().size();
                for (auto i = 0; i < list_size; i++) {
                    auto callI_reg = (RegOperand *)(callI->GetParameterList()[i].second);
                    if (callI_reg->GetRegNo() == i) {
                        continue;
                    }    // funtion params id stand by i
                    if (FuncdefI->formals[i] == BasicInstruction::LLVMType::PTR) {
                        NeedtoInsertPTR = 1;
                        if (PtrUsed.find(i) == PtrUsed.end()) {
                            auto PtrReg = GetNewRegOperand(++C->max_reg);
                            if (PtrReg != nullptr) {
                                PtrUsed[i] = PtrReg;
                            }
                        }
                        // break;
                    }
                }
            }
        }
    }
    if (NeedtoInsertPTR) {
        // insert alloca and store instruction of ptr
        for (u_int32_t i = 0; i < FuncdefI->formals.size(); ++i) {
            if (FuncdefI->formals[i] == BasicInstruction::LLVMType::PTR && PtrUsed.find(i) != PtrUsed.end()) {
                auto PtrReg = PtrUsed[i];
                // std::cout<<PtrReg->GetFullName()<<'\n';
                // std::cout<<i<<" "<<FuncdefI->formals_reg[i]<<'\n';
                AllocaDeque.push_back(new AllocaInstruction(BasicInstruction::LLVMType::PTR, PtrReg));
                StoreDeque.push_back(new StoreInstruction(BasicInstruction::LLVMType::PTR, PtrReg, FuncdefI->formals_reg[i]));
            }
        }
        // insert load before new ptr
        for (auto [id, bb] : *C->block_map) {
            auto tmp_Instruction_list = bb->Instruction_list;
            bb->Instruction_list.clear();
            for (auto I : tmp_Instruction_list) {
                auto ResultOperands = I->GetNonResultOperands();
                bool NeedtoUpdate = 0;
                // jump call self() ret
                if (id != 0 && NeedtoInsertPTR && !ResultOperands.empty()) {
                    for (u_int32_t i = 0; i < ResultOperands.size(); ++i) {
                        auto ResultReg = ResultOperands[i];
                        for (u_int32_t j = 0; j < FuncdefI->formals_reg.size(); ++j) {
                            if (PtrUsed.find(j) == PtrUsed.end()) {
                                continue;
                            }
                            auto DefReg = FuncdefI->formals_reg[j];
                            if (ResultReg->GetFullName() == DefReg->GetFullName()) {
                                NeedtoUpdate = 1;
                                auto PtrReg = GetNewRegOperand(++C->max_reg);
                                bb->InsertInstruction(1, new LoadInstruction(BasicInstruction::LLVMType::PTR, PtrUsed[j], PtrReg));
                                ResultOperands[i] = PtrReg;
                                break;
                            }
                        }
                    }
                    if (NeedtoUpdate) {
                        I->SetNonResultOperands(ResultOperands);
                    }
                }
                bb->InsertInstruction(1, I);
            }
        }
    }

    while (!StoreDeque.empty()) {
        auto I = StoreDeque.back();
        if (I != nullptr) {
            bb0->InsertInstruction(0, I);
        }
        StoreDeque.pop_back();
    }
    for (auto *it : AllocaDeque) {
        if (it != nullptr) {
            bb0->InsertInstruction(0, it);
        }
    }
    while (!AllocaDeque.empty()) {
        AllocaDeque.pop_back();
    }
    for (auto [id, bb] : *C->block_map) {
        if (bb->Instruction_list.back()->GetOpcode() != BasicInstruction::RET || bb->Instruction_list.size() <= 1) {
            continue;
        }
        auto retI = (RetInstruction *)bb->Instruction_list.back();
        if (retI == nullptr) {
            continue;
        }
        auto I = *(--(--bb->Instruction_list.end()));
        if (I == nullptr || I->GetOpcode() != BasicInstruction::CALL) {
            continue;
        }
        auto callI = (CallInstruction *)I;
        if (callI == nullptr || callI->GetFunctionName() != FuncdefI->GetFunctionName()) {
            continue;
        }
        bool opt1 = callI->GetResult() != NULL && callI->GetResult()->GetFullName() == retI->GetRetVal()->GetFullName();
        bool opt2 = callI->GetResult() == NULL && retI->GetType() == BasicInstruction::LLVMType::VOID;
        if (opt1 || opt2) {
            // std::cout<<id<<'\n';
            // bb->printIR(std::cout);
            EraseSet.insert(callI);
            EraseSet.insert(retI);
            auto list_size = callI->GetParameterList().size();
            auto bb0_it = --bb0->Instruction_list.end();
            auto bb0_ptr_it = bb0->Instruction_list.begin();
            while ((*bb0_ptr_it)->GetOpcode() == BasicInstruction::ALLOCA) {
                bb0_ptr_it++;
            }
            // if exist alloca ptr,bb0_ptr_it=the end of alloca ptr
            for (auto i = 0; i < list_size; i++) {
                // std::cout<<i<<'\n';
                auto callI_type = callI->GetParameterList()[i].first;
                auto callI_reg = (RegOperand *)(callI->GetParameterList()[i].second);
                if (callI_reg->GetRegNo() == i || (callI_type == BasicInstruction::LLVMType::PTR && PtrUsed.find(i) == PtrUsed.end())) {
                    continue;
                }    // funtion params id stand by i
                Instruction allocaI;
                if (callI->GetParameterList()[i].first == BasicInstruction::LLVMType::PTR) {
                    bb0_ptr_it--;
                    allocaI = *bb0_ptr_it;
                } else {
                    bb0_it--;
                    allocaI = *bb0_it;
                    while (allocaI->GetOpcode() != BasicInstruction::ALLOCA) {
                        bb0_it--;
                        allocaI = *bb0_it;
                    }
                }
                callI->PrintIR(std::cout);
                allocaI->PrintIR(std::cout);
                if (allocaI == nullptr || allocaI->GetResult() == nullptr) {
                    continue;
                }
                auto storeI = new StoreInstruction(callI->GetParameterList()[i].first, allocaI->GetResult(),
                                                   callI->GetParameterList()[i].second);
                if (storeI != nullptr && bb != nullptr) {
                    bb->InsertInstruction(1, storeI);
                } else {
                    delete storeI;  // 如果插入失败，确保释放内存
                }
            }
            if (bb != nullptr) {
                auto brInst = new BrUncondInstruction(GetNewLabelOperand(1));
                if (brInst != nullptr) {
                    bb->InsertInstruction(1, brInst);
                } else {
                    delete brInst;
                }
            }
        }
    }
    // std::cout<<"HERE FUNCTION "<<FuncdefI->GetFunctionName()<<'\n';
    for (auto [id, bb] : *C->block_map) {
        auto tmp_Instruction_list = bb->Instruction_list;
        bb->Instruction_list.clear();
        for (auto I : tmp_Instruction_list) {
            if (EraseSet.find(I) != EraseSet.end()) {
                continue;
            }
            bb->InsertInstruction(1, I);
        }
        // bb->printIR(std::cout);
    }
    EraseSet.clear();
    PtrUsed.clear();
}

void TailCallElimPass::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
		TailCallElim(cfg);
		cfg->BuildCFG();
    }
}