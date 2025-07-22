#include "machine_peephole.h"

void MachinePeephole::Execute() {
    EliminateRedundantInstructions();
}

void MachinePeephole::EliminateRedundantInstructions() {
    // remove instructions like:
    // fmv x, x     √
    // add x, x, 0  √
    // mul x, x, 1  √
    // sub x, x, 0  √
    // div x, x, 1
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

/*
TODO：
- 上述这些,float的补全
- 其它类型的冗余指令删除
*/
/*
1. addi t0, x0, 0
   use t0   ---> 直接用x0替换t0  （其他位置用t0的话...） 【必须要用数据流信息】

*/