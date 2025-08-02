#include "machine_strengthreduce.h"

void MachineStrengthReducePass::Execute() {
    ScalarStrengthReduction();
    GepStrengthReduction();
}

// 仅保留乘法的优化
// 除法、取模操作的强度削弱仅适用于无符号数
void MachineStrengthReducePass::ScalarStrengthReduction() {
    for (auto &func : unit->functions) {
        for (auto &block : func->blocks) {
            for (auto it = block->instructions.begin(); it != block->instructions.end();) {
                auto inst = (RiscV64Instruction*)(*it);
                //[1] 乘/除/模 的削弱
                if(inst->getOpcode() == RISCV_ADDI || inst->getOpcode() == RISCV_ADDIW ||
                   inst->getOpcode() == RISCV_LI || inst->getOpcode() == RISCV_LUI ) {
                    auto next_it = std::next(it);
                    if (next_it != block->instructions.end()) {
                        auto next_inst = (RiscV64Instruction*)(*next_it);
                        /*
                        (1) 乘法--->左移
                        addiw		t0,x0,k
                        mul			t0,s1,t0    ---> slli t0,s1,k （目前方案：仅当k=2^n时才进行转换）
                        */
                        if(next_inst->getOpcode() == RISCV_MUL || next_inst->getOpcode() == RISCV_MULW) {
                            bool flag=false;  int k=0;
                            if(inst->getOpcode() == RISCV_ADDI || inst->getOpcode() == RISCV_ADDIW){
                                if(next_inst->getRs2().reg_no == inst->getRd().reg_no && !inst->getRs1().is_virtual && inst->getRs1().reg_no == 0){
                                    flag=true;
                                    k = inst->getImm();
                                }
                            }else if(next_inst->getRs2().reg_no == inst->getRd().reg_no){
                                flag=true;
                                k = inst->getImm();
                                if(inst->getOpcode() == RISCV_LUI){
                                    k=(k<<12);
                                }
                            }
                            if(flag) {
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

// void MachineStrengthReducePass::GepStrengthReduction(){
//     // gep .... i32 1 指令消解优化
//     for (auto &func : unit->functions) {
//         for (auto &block : func->blocks) {
//             auto old_block=block->instructions;
//             block->instructions.clear();
//             for (auto it = old_block.begin(); it != old_block.end();) {
//                 auto inst1 = (RiscV64Instruction*)(*it);
//                 if(inst1->getOpcode()==RISCV_ADDIW && inst1->getRs1().reg_no==0){
//                     it = std::next(it);
//                     if(it!=old_block.end()){
//                         auto inst2=(RiscV64Instruction*)(*it);
//                         if(inst2->getOpcode()==RISCV_ADDIW && inst2->getRs1().reg_no==0){
//                             it = std::next(it);
//                             if(it!=old_block.end()){
//                                 auto inst3=(RiscV64Instruction*)(*it);
//                                 if(inst3->getOpcode()==RISCV_MUL
//                                    &&inst3->getRs1().reg_no==inst2->getRd().reg_no
//                                    &&inst3->getRs2().reg_no==inst1->getRd().reg_no){
//                                     it = std::next(it);
//                                     if(it!=old_block.end()){
//                                         auto inst4=(RiscV64Instruction*)(*it);
//                                         if(inst4->getOpcode()==RISCV_SLLI 
//                                            &&inst4->getRs1().reg_no==inst3->getRd().reg_no
//                                            &&inst4->getImm()==2){
//                                             it = std::next(it);
//                                             if(it!=old_block.end()){
//                                                 auto inst5=(RiscV64Instruction*)(*it);
//                                                 if(inst5->getOpcode()==RISCV_ADD && inst5->getRs2().reg_no==inst4->getRd().reg_no){
//                                                     int op1=inst1->getImm();
//                                                     int op2=inst2->getImm();
//                                                     int index=4*op1*op2;

//                                                     assert(index>=0);
//                                                     if(index>2047){
//                                                         RiscV64Instruction* li_inst = new RiscV64Instruction();
//                                                         li_inst->setOpcode(RISCV_LI,false);
//                                                         li_inst->setRd(inst4->getRd());
//                                                         li_inst->setImm(index);
//                                                         block->instructions.push_back(li_inst);

//                                                         block->instructions.push_back(inst5);//仅加入指令5
//                                                     }else{
//                                                         inst5->setOpcode(RISCV_ADDI,false);
//                                                         inst5->setImm(index);
//                                                         block->instructions.push_back(inst5);//仅加入指令5
//                                                     }
//                                                     ++it;
//                                                     continue;
//                                                 }else{
//                                                     block->instructions.push_back(inst1);
//                                                     block->instructions.push_back(inst2);
//                                                     block->instructions.push_back(inst3);
//                                                     block->instructions.push_back(inst4);
//                                                     continue;
//                                                 }
//                                             }
//                                         }else{
//                                             block->instructions.push_back(inst1);
//                                             block->instructions.push_back(inst2);
//                                             block->instructions.push_back(inst3);
//                                             continue;
//                                         }
//                                     }
//                                 }else{
//                                     block->instructions.push_back(inst1);
//                                     block->instructions.push_back(inst2);
//                                     continue;
//                                 }
//                             }
//                         }else{
//                             block->instructions.push_back(inst1);
//                             block->instructions.push_back(inst2);
//                             ++it; //当前it必然不可能匹配inst1了
//                             continue;
//                         }
//                     }
//                 }else{
//                     block->instructions.push_back(inst1);
//                     ++it;
//                 }
                
//             }
//         }
//     }
// }

void MachineStrengthReducePass::GepStrengthReduction() {
    for (auto &func : unit->functions) {
        for (auto &block : func->blocks) {
            auto old_block = block->instructions;
            block->instructions.clear();

            auto it = old_block.begin();
            while (it != old_block.end()) {
                auto inst1 = (RiscV64Instruction*)(*it);

                //一次取5条指令
                auto it2 = std::next(it);
                auto it3 = (it2 != old_block.end()) ? std::next(it2) : old_block.end();
                auto it4 = (it3 != old_block.end()) ? std::next(it3) : old_block.end();
                auto it5 = (it4 != old_block.end()) ? std::next(it4) : old_block.end();

                bool matched = false;
                if (it2 != old_block.end() && it3 != old_block.end() && it4 != old_block.end() && it5 != old_block.end()) {
                    auto inst2 = (RiscV64Instruction*)(*it2);
                    auto inst3 = (RiscV64Instruction*)(*it3);
                    auto inst4 = (RiscV64Instruction*)(*it4);
                    auto inst5 = (RiscV64Instruction*)(*it5);

                    if (inst1->getOpcode() == RISCV_ADDIW && inst1->getRs1().reg_no == 0 &&
                        inst2->getOpcode() == RISCV_ADDIW && inst2->getRs1().reg_no == 0 &&
                        inst3->getOpcode() == RISCV_MUL &&
                            inst3->getRs1().reg_no == inst2->getRd().reg_no &&
                            inst3->getRs2().reg_no == inst1->getRd().reg_no &&
                        inst4->getOpcode() == RISCV_SLLI &&
                            inst4->getRs1().reg_no == inst3->getRd().reg_no &&
                            inst4->getImm() == 2 &&
                        inst5->getOpcode() == RISCV_ADD &&
                            inst5->getRs2().reg_no == inst4->getRd().reg_no) 
                    {
                        // 完全匹配：做 strength reduction
                        int op1 = inst1->getImm();
                        int op2 = inst2->getImm();
                        int index = 4 * op1 * op2;

                        if (index > 2047) {
                            // 需要先 materialize 常数，再 add
                            RiscV64Instruction* li_inst = new RiscV64Instruction();
                            li_inst->setOpcode(RISCV_LI, false);
                            li_inst->setRd(inst4->getRd()); // 目标寄存器要合理设置
                            li_inst->setImm(index);
                            block->instructions.push_back(li_inst);

                            block->instructions.push_back(inst5);
                        } else {
                            // 直接用 ADDI（造一个新的，不改 inst5 原地）
                            RiscV64Instruction* new_addi = new RiscV64Instruction();
                            new_addi->setOpcode(RISCV_ADDI, false);
                            new_addi->setRd(inst5->getRd());
                            new_addi->setRs1(inst5->getRs1());
                            new_addi->setImm(index);
                            block->instructions.push_back(new_addi);
                        }

                        // 跳过这 5 条原始指令
                        std::advance(it, 5);
                        matched = true;
                    }
                }

                if (!matched) {
                    // 没匹配上：原样搬运当前这一条
                    block->instructions.push_back(inst1);
                    ++it;
                }
            }
        }
    }
}