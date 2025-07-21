#include "machine_peephole.h"

void MachinePeephole::Execute() {
    EliminateRedundantInstructions();
}

void MachinePeephole::EliminateRedundantInstructions() {
    // remove instructions like:
    // add x, x, 0
    // mul x, x, 1
    // sub x, x, 0
    // div x, x, 1
    for (auto &func : unit->functions) {
        for (auto &block : func->blocks) {
            for (auto it = block->instructions.begin(); it != block->instructions.end();) {
                auto inst = (RiscV64Instruction*)(*it);
                if(inst ->getOpcode() == RISCV_ADDI ||  inst ->getOpcode() == RISCV_ADDIW) {
                    if (inst->getRd().reg_no == inst->getRs1().reg_no && inst->getImm() == 0) {
                        it = block->instructions.erase(it); 
                        continue;
                    }
                ++it;
                continue;
                }else if( inst ->getOpcode() == RISCV_ADD || inst ->getOpcode() == RISCV_ADDW || inst ->getOpcode() == RISCV_SUB ||  inst ->getOpcode() == RISCV_SUBW) {
                    if (inst->getRd().reg_no == inst->getRs1().reg_no ) {
                        auto rs2 = inst->getRs2();
                        if(!rs2.is_virtual && rs2.reg_no == 0) {
                            it = block->instructions.erase(it); 
                            continue;
                        }
                    }
                ++it;
                continue;
                } else {
                    ++it;
                }
            }
        }
    }
}