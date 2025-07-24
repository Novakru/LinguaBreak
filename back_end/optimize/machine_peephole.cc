#include "machine_peephole.h"

void MachinePeephole::Execute() {
    EliminateRedundantInstructions();
    //FloatCompFusion();
    ScalarStrengthReduction();
}

void MachinePeephole::EliminateRedundantInstructions() {
    // remove instructions like:
    // fmv x, x     √
    // add x, x, 0  √
    // mul x, x, 1  √
    // sub x, x, 0  √
    // div x, x, 1  √
    // fmul+fadd / fmul+fsub --> fma/fnma
    for (auto &func : unit->functions) {
        for (auto &block : func->blocks) {
            for (auto it = block->instructions.begin(); it != block->instructions.end();) {
                auto inst = (RiscV64Instruction*)(*it);
                //(1) fmv x, x  ---> 直接删除此指令
                if (inst->getOpcode() == RISCV_FMV_S || inst->getOpcode() == RISCV_FMV_D ) {
                    if(inst->getRd().reg_no == inst->getRs1().reg_no) {
                        it = block->instructions.erase(it);
                        continue;
                    }
                }

                //(2) add x, x, 0  ---> 直接删除此指令
                if(inst ->getOpcode() == RISCV_ADDI || inst ->getOpcode() == RISCV_ADDIW) {
                    if (inst->getRd().reg_no == inst->getRs1().reg_no && inst->getImm() == 0) {
                        it = block->instructions.erase(it); 
                        continue;
                    }
                }else if( inst ->getOpcode() == RISCV_ADD || inst ->getOpcode() == RISCV_SUB ||
                          inst ->getOpcode() == RISCV_ADDW || inst ->getOpcode() == RISCV_SUBW ||
                          inst ->getOpcode() == RISCV_FADD_D || inst ->getOpcode() == RISCV_FADD_S ||
                          inst ->getOpcode() == RISCV_FSUB_D || inst ->getOpcode() == RISCV_FSUB_S) {
                    if (inst->getRd().reg_no == inst->getRs1().reg_no ) {
                        auto rs2 = inst->getRs2();
                        if(!rs2.is_virtual && rs2.reg_no == 0) {
                            it = block->instructions.erase(it); 
                            continue;
                        }
                    }
                }

                //（3）add t0, x0, 1
                //    mul xxx, xxx,t0     ---> 自身乘1，直接删除两条指令

                //    add t0, x0, 1
                //    mul xxx, xx, t0      ---> 乘1，改为加0（强度削弱）
                if(inst ->getOpcode() == RISCV_ADDI || inst ->getOpcode() == RISCV_ADDIW) {
                    auto rs1 = inst->getRs1();
                    auto rs2 = inst->getRs2();
                    if (!rs1.is_virtual && rs1.reg_no == 0 && inst->getImm() == 1){// add t0, x0, 1型
                        auto next_it = std::next(it);
                        if (next_it != block->instructions.end()) {
                            auto next_inst = (RiscV64Instruction*)(*next_it);
                            // （2.1）int 型
                            if (next_inst->getOpcode() == RISCV_MUL || next_inst->getOpcode() == RISCV_MULW ||
                                next_inst->getOpcode() == RISCV_MULH || next_inst->getOpcode() == RISCV_MULHSU ||
                                next_inst->getOpcode() == RISCV_MULHU||
                                next_inst->getOpcode() == RISCV_DIV || next_inst->getOpcode() == RISCV_DIVU ||
                                next_inst->getOpcode() == RISCV_DIVW) {
                                if(next_inst->getRd().reg_no == next_inst->getRs1().reg_no &&
                                   next_inst->getRs2().reg_no == inst->getRd().reg_no) {//mul xxx, xxx,t0型
                                    //删除两条指令
                                    it = block->instructions.erase(it);
                                    it = block->instructions.erase(it);
                                    continue;
                                }else if(next_inst->getRd().reg_no != next_inst->getRs1().reg_no &&
                                         next_inst->getRs2().reg_no == inst->getRd().reg_no){
                                    //删除第一条add指令，将mul 1 改为add 0
                                    it = block->instructions.erase(it);
                                    if(next_inst->getOpcode() == RISCV_MUL) {
                                        next_inst->setOpcode(RISCV_ADDI,false);
                                    } else if(next_inst->getOpcode() == RISCV_MULW) {
                                        next_inst->setOpcode(RISCV_ADDIW,false);
                                    }
                                    next_inst->setImm(0);
                                    ++it;
                                    continue;
                                }
                            }
                        }
                    } 
                // (4) addi(w) t0, x0, k
                //     add(w)sub xxx, xx, t0  --->直接使用立即数k
                    if(!rs1.is_virtual && rs1.reg_no == 0 ){// add t0, x0, k型，t0被使用，可直接用k替换
                        auto next_it = std::next(it);
                        if (next_it != block->instructions.end()) {
                            auto next_inst = (RiscV64Instruction*)(*next_it);
                            if(next_inst->getOpcode() == RISCV_ADD || next_inst->getOpcode() == RISCV_ADDW||
                               next_inst->getOpcode() == RISCV_SUB || next_inst->getOpcode() == RISCV_SUBW) {
                                if(next_inst->getRs2().reg_no== inst->getRd().reg_no) {
                                    if(next_inst->getOpcode() == RISCV_ADD) {
                                        next_inst->setOpcode(RISCV_ADDI,false);
                                        next_inst->setImm(inst->getImm());
                                    } else if(next_inst->getOpcode() == RISCV_ADDW) {
                                        next_inst->setOpcode(RISCV_ADDIW,false);
                                        next_inst->setImm(inst->getImm());
                                    } else if(next_inst->getOpcode() == RISCV_SUB) {
                                        next_inst->setOpcode(RISCV_ADDI,false);
                                        next_inst->setImm(-(inst->getImm()));
                                    } else if(next_inst->getOpcode() == RISCV_SUBW) {
                                        next_inst->setOpcode(RISCV_ADDIW,false);
                                        next_inst->setImm(-(inst->getImm()));
                                    }
                                    
                                    it = block->instructions.erase(it);
                                    ++it;
                                    continue;
                                }
                            }

                        }
                    }
                }

                ++it;
            }
        }
    }
}

void MachinePeephole::FloatCompFusion(){
    for (auto &func : unit->functions) {
        for (auto &block : func->blocks) {
            for (auto it = block->instructions.begin(); it != block->instructions.end();) {
                auto inst = (RiscV64Instruction*)(*it);

                //（5）fma /fnma
                if(inst ->getOpcode() == RISCV_FMUL_S){
                    auto next_it = std::next(it);
                    if (next_it != block->instructions.end()) {
                        auto next_inst = (RiscV64Instruction*)(*next_it);
                        if(next_inst->getOpcode() == RISCV_FADD_S || next_inst->getOpcode() == RISCV_FSUB_S) {
                            if(next_inst->getRs2().reg_no == inst->getRd().reg_no) {
                                RiscV64Instruction *fma_inst = new RiscV64Instruction();
                                if(next_inst->getOpcode() == RISCV_FADD_S) {
                                    fma_inst->setOpcode(RISCV_FMADD_S,false);
                                } else {
                                    fma_inst->setOpcode(RISCV_FNMADD_S,false);
                                }
                                fma_inst->setRd(next_inst->getRd());
                                fma_inst->setRs1(inst->getRs1());
                                fma_inst->setRs2(inst->getRs2());
                                fma_inst->setRs3(next_inst->getRs1());

                                it = block->instructions.erase(it);
                                *it = fma_inst;
                                ++it;
                                continue;
                            }else if(next_inst->getRs1().reg_no == inst->getRd().reg_no){
                                RiscV64Instruction *fma_inst = new RiscV64Instruction();
                                if(next_inst->getOpcode() == RISCV_FADD_S) {
                                    fma_inst->setOpcode(RISCV_FMADD_S,false);
                                } else {
                                    fma_inst->setOpcode(RISCV_FMSUB_S,false);
                                }
                                fma_inst->setRd(next_inst->getRd());
                                fma_inst->setRs1(inst->getRs1());
                                fma_inst->setRs2(inst->getRs2());
                                fma_inst->setRs3(next_inst->getRs1());

                                it = block->instructions.erase(it);
                                *it = fma_inst;
                                ++it;
                                continue;
                            }
                        }
                    }
                }
                ++it;
            }
        }
    }
}



void MachinePeephole::ScalarStrengthReduction() {
    for (auto &func : unit->functions) {
        for (auto &block : func->blocks) {
            for (auto it = block->instructions.begin(); it != block->instructions.end();) {
                auto inst = (RiscV64Instruction*)(*it);
                if(inst->getOpcode() == RISCV_ADDI || inst->getOpcode() == RISCV_ADDIW) {
                    auto next_it = std::next(it);
                    if (next_it != block->instructions.end()) {
                        auto next_inst = (RiscV64Instruction*)(*next_it);
                        /*
                        (1) 乘法--->左移
                        addiw		t0,x0,k
                        mul			t0,s1,t0    ---> slli t0,s1,k （目前方案：仅当k=2^n时才进行转换）
                        */
                        if(next_inst->getOpcode() == RISCV_MUL || next_inst->getOpcode() == RISCV_MULW) {
                            if(next_inst->getRs2().reg_no == inst->getRd().reg_no && !inst->getRs1().is_virtual && inst->getRs1().reg_no == 0) {
                                int k = inst->getImm();
                                if(k > 0 && (k & (k - 1)) == 0 && k!=1) { // k是2的幂
                                    int shift_amount = __builtin_ctz(k);  // 获取k的二进制表示中最低位1的索引
                                    RiscV64Instruction *slli_inst = new RiscV64Instruction();
                                    if(next_inst->getOpcode() == RISCV_MUL) {
                                        slli_inst->setOpcode(RISCV_SLLI,false);
                                    } else if(next_inst->getOpcode() == RISCV_MULW) {
                                        slli_inst->setOpcode(RISCV_SLLIW,false);
                                    }
                                    slli_inst->setRd(next_inst->getRd());
                                    slli_inst->setRs1(next_inst->getRs1());
                                    slli_inst->setImm(shift_amount);

                                    it = block->instructions.erase(it);
                                    *it = slli_inst;
                                    ++it;
                                    continue;
                                }
                            }
                        }
                        /*
                        (2) 除法--->右移
                        addiw		t0,x0,k
                        div			t0,s1,t0    ---> srai t0,s1,k （目前方案：仅当k=2^n时才进行转换）
                        */
                        else if(next_inst->getOpcode() == RISCV_DIV || next_inst->getOpcode() == RISCV_DIVW){
                            if(next_inst->getRs2().reg_no == inst->getRd().reg_no && !inst->getRs1().is_virtual && inst->getRs1().reg_no == 0) {
                                int k = inst->getImm();
                                if(k > 0 && (k & (k - 1)) == 0 && k!=1) { // k是2的幂
                                    int shift_amount = __builtin_ctz(k);  // 获取k的二进制表示中最低位1的索引
                                    RiscV64Instruction *srai_inst = new RiscV64Instruction();
                                    if(next_inst->getOpcode() == RISCV_DIV) {
                                        srai_inst->setOpcode(RISCV_SRAI,false);
                                    } else if(next_inst->getOpcode() == RISCV_DIVW) {
                                        srai_inst->setOpcode(RISCV_SRAIW,false);
                                    }
                                    srai_inst->setRd(next_inst->getRd());
                                    srai_inst->setRs1(next_inst->getRs1());
                                    srai_inst->setImm(shift_amount);

                                    it = block->instructions.erase(it);
                                    *it = srai_inst;
                                    ++it;
                                    continue;
                                }
                            }
                        }
                        /*
                        (1) 取模--->位与
                        addiw		t0,x0,k
                        rem			t0,s1,t0    ---> and t0,s1,k （目前方案：仅当k=2^n时才进行转换）
                        */
                        //else 
                        // if(next_inst->getOpcode() == RISCV_REM || next_inst->getOpcode() == RISCV_REMW){
                        //     if(next_inst->getRs2().reg_no == inst->getRd().reg_no && !inst->getRs1().is_virtual && inst->getRs1().reg_no == 0) {
                        //         int k = inst->getImm();
                        //         if(k > 0 && (k & (k - 1)) == 0 && k!=1) { // k是2的幂
                        //             int mod_amount = k - 1; // 计算模数
                        //             RiscV64Instruction *and_inst = new RiscV64Instruction();
                        //             and_inst->setOpcode(RISCV_ANDI,false);
                        //             and_inst->setRd(next_inst->getRd());
                        //             and_inst->setRs1(next_inst->getRs1());
                        //             and_inst->setImm(mod_amount);

                        //             it = block->instructions.erase(it);
                        //             *it = and_inst;
                        //             ++it;
                        //             continue;
                        //         }
                        //     }
                        // }
                    }
                }
                ++it;
            }
        }
    }
}