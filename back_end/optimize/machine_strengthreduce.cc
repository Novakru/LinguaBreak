#include "machine_strengthreduce.h"

void MachineStrengthReducePass::Execute() {
    ScalarStrengthReduction();
}

// 仅保留乘法的优化
// 除法、取模操作的强度削弱仅适用于无符号数
void MachineStrengthReducePass::ScalarStrengthReduction() {
    for (auto &func : unit->functions) {
        for (auto &block : func->blocks) {
            for (auto it = block->instructions.begin(); it != block->instructions.end();) {
                auto inst = (RiscV64Instruction*)(*it);
                //[1] 乘/除/模 的削弱
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
                        //else 
                        // if(next_inst->getOpcode() == RISCV_DIV || next_inst->getOpcode() == RISCV_DIVW){
                        //     if(next_inst->getRs2().reg_no == inst->getRd().reg_no && !inst->getRs1().is_virtual && inst->getRs1().reg_no == 0) {
                        //         int k = inst->getImm();
                        //         if(k > 0 && (k & (k - 1)) == 0 && k!=1) { // k是2的幂
                        //             int shift_amount = __builtin_ctz(k);  // 获取k的二进制表示中最低位1的索引
                        //             RiscV64Instruction *srai_inst = new RiscV64Instruction();
                        //             if(next_inst->getOpcode() == RISCV_DIV) {
                        //                 srai_inst->setOpcode(RISCV_SRAI,false);
                        //             } else if(next_inst->getOpcode() == RISCV_DIVW) {
                        //                 srai_inst->setOpcode(RISCV_SRAIW,false);
                        //             }
                        //             srai_inst->setRd(next_inst->getRd());
                        //             srai_inst->setRs1(next_inst->getRs1());
                        //             srai_inst->setImm(shift_amount);

                        //             it = block->instructions.erase(it);
                        //             *it = srai_inst;
                        //             ++it;
                        //             continue;
                        //         }
                        //     }
                        // }
                        /*
                        (1) 取模--->位与
                        addiw		t0,x0,k
                        rem			t0,s1,t0    ---> and t0,s1,k （目前方案：仅当k=2^n时才进行转换）
                        */
                        // else 
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
                }//[2] 加法的强度削弱
                else if(inst->getOpcode() == RISCV_ADD || inst->getOpcode() == RISCV_ADDW){
                    auto inst = (RiscV64Instruction*)(*it);
                    if(inst->getRd().reg_no == inst->getRs1().reg_no && inst->getRd().reg_no==inst->getRs2().reg_no ) {
                        // add x, x, x ---> x*2 ---> x<<1
                        RiscV64Instruction *slli_inst = new RiscV64Instruction();
                        if(inst->getOpcode() == RISCV_ADD) {
                            slli_inst->setOpcode(RISCV_SLLI,false);
                        } else if(inst->getOpcode() == RISCV_ADDW) {
                            slli_inst->setOpcode(RISCV_SLLIW,false);
                        }
                        slli_inst->setRd(inst->getRd());
                        slli_inst->setRs1(inst->getRs1());
                        slli_inst->setImm(1);

                        *it = slli_inst;
                        ++it;
                        continue;
                    }
                }
                ++it;
            }
        }
    }
}

