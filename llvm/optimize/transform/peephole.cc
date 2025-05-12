#include "peephole.h"
#include <functional>

void SrcEqResultInstEliminate(CFG *C);
void ImmResultReplace(CFG *C);

void PeepholePass::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        InstSimplify(cfg);
    }
}

void PeepholePass::InstSimplify(CFG *C){
    SrcEqResultInstEliminate(C);
    ImmResultReplace(C);
}

void PeepholePass::SrcEqResultInstEliminateExecute(){
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        SrcEqResultInstEliminate(cfg);
    }
}

void PeepholePass::ImmResultReplaceExecute(){
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        ImmResultReplace(cfg);
    }
}

void ImmResultReplace(CFG *C){
    std::set<Instruction> EraseSet;
    std::map<Operand, Operand> replacemap;
    bool changed = true;
    while(changed){
        changed = false;
        EraseSet.clear();
        replacemap.clear();
        for(auto [id, bb] : *C->block_map){
            for(auto I : bb->Instruction_list){
                auto opcode = I->GetOpcode();
                if(opcode == BasicInstruction::ADD 
                        || opcode == BasicInstruction::SUB 
                        || opcode == BasicInstruction::MUL 
                        || opcode == BasicInstruction::DIV 
                        || opcode == BasicInstruction::MOD 
                        || opcode == BasicInstruction::BITXOR
                        || opcode == BasicInstruction::FADD 
                        || opcode == BasicInstruction::FSUB 
                        || opcode == BasicInstruction::FMUL 
                        || opcode == BasicInstruction::FDIV){
                    auto ArithI = (ArithmeticInstruction*)I;
                    auto resultop = ArithI->GetResult();
                    auto op1 = ArithI->GetOperand1();
                    auto op2 = ArithI->GetOperand2();
                    if(op1->GetOperandType() != BasicOperand::REG && op2->GetOperandType() != BasicOperand::REG){
                        EraseSet.insert(I);
                        auto type = ArithI->GetDataType();
                        changed = 1;
                        if(type == BasicInstruction::I32){
                            auto imm1 = ((ImmI32Operand*)op1)->GetIntImmVal();
                            auto imm2 = ((ImmI32Operand*)op2)->GetIntImmVal();
                            int ans = 0;
                            switch (opcode)
                            {
                            case BasicInstruction::ADD:
                                ans = imm1 + imm2;
                                break;
                            case BasicInstruction::SUB:
                                ans = imm1 - imm2;
                                break;
                            case BasicInstruction::MUL:
                                ans = imm1 * imm2;
                                break;
                            case BasicInstruction::DIV:
                                ans = imm1 / imm2;
                                break;
                            case BasicInstruction::MOD:
                                ans = imm1 % imm2;
                                break;
                            case BasicInstruction::BITXOR:
                                ans = imm1 ^ imm2;
                                break;
                            default:
                                break;
                            }
                            replacemap[resultop] = new ImmI32Operand(ans);
                        }else {
                            auto imm1 = ((ImmF32Operand*)op1)->GetFloatVal();
                            auto imm2 = ((ImmF32Operand*)op2)->GetFloatVal();
                            float ans = 0;
                            switch (opcode)
                            {
                            case BasicInstruction::FADD:
                                ans = imm1 + imm2;
                                break;
                            case BasicInstruction::FSUB:
                                ans = imm1 - imm2;
                                break;
                            case BasicInstruction::FMUL:
                                ans = imm1 * imm2;
                                break;
                            case BasicInstruction::FDIV:
                                ans = imm1 / imm2;
                                break;
                            default:
                                break;
                            }
                            replacemap[resultop] = new ImmF32Operand(ans);
                        }
                    }
                }else if(I->GetOpcode() == BasicInstruction::ICMP){
                    auto icmpI = (IcmpInstruction*)I;
                    auto resultop = icmpI->GetResult();
                    auto op1 = icmpI->GetOp1();
                    auto op2 = icmpI->GetOp2();
                    auto cond = icmpI->GetCond();
                    if(op1->GetOperandType() != BasicOperand::REG && op2->GetOperandType() != BasicOperand::REG){
                        EraseSet.insert(I);
                        changed = 1;
                        int ans = 0;
                        auto imm1 = ((ImmI32Operand*)op1)->GetIntImmVal();
                        auto imm2 = ((ImmI32Operand*)op2)->GetIntImmVal();
                        switch (cond)
                        {
                        case BasicInstruction::eq:
                            ans = imm1 == imm2;
                            break;
                        case BasicInstruction::ne:
                            ans = imm1 != imm2;
                            break;
                        case BasicInstruction::ugt:
                            ans = imm1 > imm2;
                            break;
                        case BasicInstruction::uge:
                            ans = imm1 >= imm2;
                            break;
                        case BasicInstruction::ult:
                            ans = imm1 < imm2;
                            break;
                        case BasicInstruction::ule:
                            ans = imm1 <= imm2;
                            break;
                        case BasicInstruction::sgt:
                            ans = imm1 > imm2;
                            break;
                        case BasicInstruction::sge:
                            ans = imm1 >= imm2;
                            break;
                        case BasicInstruction::slt:
                            ans = imm1 < imm2;
                            break;
                        case BasicInstruction::sle:
                            ans = imm1 <= imm2;
                            break;
                        default:
                            break;
                        }
                        replacemap[resultop] = new ImmI32Operand(ans);
                    }
                }else if(I->GetOpcode() == BasicInstruction::FCMP){
                    auto fcmpI = (FcmpInstruction*)I;
                    auto resultop = fcmpI->GetResult();
                    auto op1 = fcmpI->GetOp1();
                    auto op2 = fcmpI->GetOp2();
                    auto cond = fcmpI->GetCond();
                    if(op1->GetOperandType() != BasicOperand::REG && op2->GetOperandType() != BasicOperand::REG){
                        EraseSet.insert(I);
                        changed = 1;
                        int ans = 0;
                        auto imm1 = ((ImmF32Operand*)op1)->GetFloatVal();
                        auto imm2 = ((ImmF32Operand*)op2)->GetFloatVal();
                        switch (cond)
                        {
                        case BasicInstruction::FALSE:
                            ans = 0;
                            break;
                        case BasicInstruction::OEQ:
                        case BasicInstruction::UEQ:
                            ans = imm1 == imm2;
                            break;
                        case BasicInstruction::OGT:
                        case BasicInstruction::UGT:
                            ans = imm1 > imm2;
                            break;
                        case BasicInstruction::OGE:
                        case BasicInstruction::UGE:
                            ans = imm1 >= imm2;
                            break;
                        case BasicInstruction::OLT:
                        case BasicInstruction::ULT:
                            ans = imm1 < imm2;
                            break;
                        case BasicInstruction::OLE:
                        case BasicInstruction::ULE:
                            ans = imm1 <= imm2;
                            break;
                        case BasicInstruction::ONE:
                        case BasicInstruction::UNE:
                            ans = imm1 != imm2;
                            break;
                        case BasicInstruction::ORD:
                            ans = 1;
                            break;
                        case BasicInstruction::UNO:
                            ans = 1;
                            break;
                        case BasicInstruction::TRUE:
                            ans = 1;
                            break;
                        default:
                            break;
                        }
                        replacemap[resultop] = new ImmI32Operand(ans);
                    }
                }
            }
        }
        for (auto [id, bb] : *C->block_map) {
            auto tmp_Instruction_list = bb->Instruction_list;
            bb->Instruction_list.clear();
            for (auto &I : tmp_Instruction_list) {
                if (EraseSet.find(I) != EraseSet.end()) {
                    // I->PrintIR(std::cerr);
                    continue;
                }
                auto opvec = I->GetNonResultOperands();
                // if(I->GetResult()!=nullptr && I->GetResult()->GetFullName() == "%r45"){
                //     I->PrintIR(std::cerr);
                // }
                for(auto &op : opvec){
                    if(op->GetOperandType() == BasicOperand::REG && replacemap.find(op) != replacemap.end()){
                        op = replacemap[op];
                    }
                }
                I->SetNonResultOperands(opvec);
                
                bb->InsertInstruction(1, I);
            }
        }
    }
}

/*eliminate the instructions like
%rx = %ry + 0(replace all the use of %rx with %ry) %ry can be i32 or float
%rx = %ry - 0(replace all the use of %rx with %ry) %ry can be i32 or float
%rx = %ry * 1(replace all the use of %rx with %ry) %ry can be i32 or float
%rx = %ry / 1(replace all the use of %rx with %ry) %ry can be i32 or float

I->ReplaceRegByMap(), I->GetNonResultOperands(), I->SetNonResultOperands() is Useful
*/
void SrcEqResultInstEliminate(CFG *C) {
	TODO("SrcEqResultInstEliminate");
}
