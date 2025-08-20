#include "machine_pee.h"
#include <cstring>
#include <cstdint>

bool IsFloatInst(int opcode){
    if(opcode>=RISCV_FMV_S){
        return true;
    }else{
        return false;
    }
}

void MachinePeePass::Execute(){
    for(auto &func:unit->functions){
        MachineCFG* cfg=func->getMachineCFG();
        //bool changed=false;
        //do{
            //changed =  RedundantReplacementEliminate(cfg);
            ConstComputingEliminate(cfg);//仅删除指令，不影响其它指令
            ConstantFolding(cfg);    
        //}while(changed);

        DCE(cfg); //删除无用指令
    }
    return ;
}


/*
fmv    res, res
add(w) res, res,x0      addi(w) res, res,0
sub(w) res, res,x0
mul(w) res, res,1
div(w) res, res,1
*/
void MachinePeePass::ConstComputingEliminate(MachineCFG* cfg){
    std::unordered_set<int> inst_to_dlt; //the def_regno of the inst

        for (auto &[id,block]: cfg->block_map) {
            for (auto it = block->instructions.begin(); it != block->instructions.end();) {
                if((*it)->arch!=MachineBaseInstruction::Type::RiscV) {++it; continue;}
                auto inst = (RiscV64Instruction*)(*it);
                //(1) fmv res, res  ---> 直接删除此指令
                if (inst->getOpcode() == RISCV_FMV_S) {
                    if(inst->getRd().reg_no == inst->getRs1().reg_no &&
                        inst->getRd().is_virtual==inst->getRs1().is_virtual) {
                        it = block->instructions.erase(it);
                        continue;
                    }
                }

                //(2) add res, res, 0  ---> 直接删除此指令
                if(inst ->getOpcode() == RISCV_ADDI || inst ->getOpcode() == RISCV_ADDIW) {
                    if (inst->getRd().reg_no == inst->getRs1().reg_no && 
                        inst->getRd().is_virtual==inst->getRs1().is_virtual &&
                        inst->getImm() == 0) {
                        it = block->instructions.erase(it); 
                        continue;
                    }
                }else if( inst ->getOpcode() == RISCV_ADD ||  inst ->getOpcode() == RISCV_ADDW) {
                    if (inst->getRd().reg_no == inst->getRs1().reg_no &&
                        inst->getRd().is_virtual==inst->getRs1().is_virtual) {
                        auto rs2 = inst->getRs2();
                        if(!rs2.is_virtual && rs2.reg_no == 0) {//x0
                            it = block->instructions.erase(it); 
                            continue;
                        }else if(rs2.is_virtual){
                            int rs2_regno=rs2.reg_no;
                            if(cfg->lattice_map[rs2_regno]->status==LatticeStatus::CONST &&
                               cfg->lattice_map[rs2_regno]->val.intval == 0){
                                it = block->instructions.erase(it); 
                                continue;
                            }
                        }
                    }
                    else if (inst->getRd().reg_no == inst->getRs2().reg_no &&
                        inst->getRd().is_virtual==inst->getRs2().is_virtual) {
                        auto rs1 = inst->getRs1();
                        if(!rs1.is_virtual && rs1.reg_no == 0) {//x0
                            it = block->instructions.erase(it); 
                            continue;
                        }else if(rs1.is_virtual){
                            int rs1_regno=rs1.reg_no;
                            if(cfg->lattice_map[rs1_regno]->status==LatticeStatus::CONST &&
                               cfg->lattice_map[rs1_regno]->val.intval == 0){
                                it = block->instructions.erase(it); 
                                continue;
                            }
                        }
                    }
                }else if(inst ->getOpcode() == RISCV_SUB || inst ->getOpcode() == RISCV_SUBW){
                    if (inst->getRd().reg_no == inst->getRs1().reg_no &&
                        inst->getRd().is_virtual==inst->getRs1().is_virtual) {
                        auto rs2 = inst->getRs2();
                        if(!rs2.is_virtual && rs2.reg_no == 0) {//x0
                            it = block->instructions.erase(it); 
                            continue;
                        }else if(rs2.is_virtual){
                            int rs2_regno=rs2.reg_no;
                            if(cfg->lattice_map[rs2_regno]->status==LatticeStatus::CONST &&
                               cfg->lattice_map[rs2_regno]->val.intval == 0){
                                it = block->instructions.erase(it); 
                                continue;
                            }
                        }
                    }
                }else if(inst->getOpcode() == RISCV_FADD_S || inst->getOpcode()==RISCV_FADD_D){
                    if (inst->getRd().reg_no == inst->getRs1().reg_no &&
                        inst->getRd().is_virtual==inst->getRs1().is_virtual) {
                        auto rs2 = inst->getRs2();
                        if(rs2.is_virtual){
                            int rs2_regno=rs2.reg_no;
                            if(cfg->lattice_map[rs2_regno]->status==LatticeStatus::CONST &&
                               cfg->lattice_map[rs2_regno]->val.floatval == 0){
                                it = block->instructions.erase(it); 
                                continue;
                            }
                        }
                    }
                    else if (inst->getRd().reg_no == inst->getRs2().reg_no &&
                        inst->getRd().is_virtual==inst->getRs2().is_virtual) {
                        auto rs1 = inst->getRs1();
                        if(rs1.is_virtual){
                            int rs1_regno=rs1.reg_no;
                            if(cfg->lattice_map[rs1_regno]->status==LatticeStatus::CONST &&
                               cfg->lattice_map[rs1_regno]->val.floatval == 0){
                                it = block->instructions.erase(it); 
                                continue;
                            }
                        }
                    }
                }else if(inst ->getOpcode() == RISCV_FSUB_S || inst ->getOpcode() == RISCV_FSUB_D){
                    if (inst->getRd().reg_no == inst->getRs1().reg_no &&
                        inst->getRd().is_virtual==inst->getRs1().is_virtual) {
                        auto rs2 = inst->getRs2();
                        if(rs2.is_virtual){
                            int rs2_regno=rs2.reg_no;
                            if(cfg->lattice_map[rs2_regno]->status==LatticeStatus::CONST &&
                               cfg->lattice_map[rs2_regno]->val.floatval == 0){
                                it = block->instructions.erase(it); 
                                continue;
                            }
                        }
                    }
                }

                //（3） t0 <--- 1
                //    mul res, res,t0     ---> 自身乘1，直接删除指令 
                if(inst->getOpcode() == RISCV_MUL || inst ->getOpcode() == RISCV_MULW) {
                    if (inst->getRd().reg_no == inst->getRs1().reg_no &&
                        inst->getRd().is_virtual==inst->getRs1().is_virtual) {
                        auto rs2 = inst->getRs2();
                        if(rs2.is_virtual){
                            int rs2_regno=rs2.reg_no;
                            if(cfg->lattice_map[rs2_regno]->status==LatticeStatus::CONST &&
                               cfg->lattice_map[rs2_regno]->val.intval == 1){
                                it = block->instructions.erase(it); 
                                continue;
                            }
                        }
                    }
                    else if (inst->getRd().reg_no == inst->getRs2().reg_no &&
                        inst->getRd().is_virtual==inst->getRs2().is_virtual) {
                        auto rs1 = inst->getRs1();
                        if(rs1.is_virtual){
                            int rs1_regno=rs1.reg_no;
                            if(cfg->lattice_map[rs1_regno]->status==LatticeStatus::CONST &&
                               cfg->lattice_map[rs1_regno]->val.intval == 1){
                                it = block->instructions.erase(it); 
                                continue;
                            }
                        }
                    }
                }else if(inst->getOpcode() == RISCV_FMUL_S || inst ->getOpcode() == RISCV_FMUL_D) {
                    if (inst->getRd().reg_no == inst->getRs1().reg_no &&
                        inst->getRd().is_virtual==inst->getRs1().is_virtual) {
                        auto rs2 = inst->getRs2();
                        if(rs2.is_virtual){
                            int rs2_regno=rs2.reg_no;
                            if(cfg->lattice_map[rs2_regno]->status==LatticeStatus::CONST &&
                               cfg->lattice_map[rs2_regno]->val.floatval == 1){
                                it = block->instructions.erase(it); 
                                continue;
                            }
                        }
                    }
                    else if (inst->getRd().reg_no == inst->getRs2().reg_no &&
                        inst->getRd().is_virtual==inst->getRs2().is_virtual) {
                        auto rs1 = inst->getRs1();
                        if(rs1.is_virtual){
                            int rs1_regno=rs1.reg_no;
                            if(cfg->lattice_map[rs1_regno]->status==LatticeStatus::CONST &&
                               cfg->lattice_map[rs1_regno]->val.floatval == 1){
                                it = block->instructions.erase(it); 
                                continue;
                            }
                        }
                    }
                }

                //(4) t0 <--- 1
                //    div res, res, t0   ---> 自身除以1，直接删除指令 
                if(inst->getOpcode() == RISCV_DIV || inst ->getOpcode() == RISCV_DIVW) {
                    if(inst->getRd().reg_no == inst->getRs1().reg_no &&
                       inst->getRd().is_virtual==inst->getRs1().is_virtual&&
                       inst->getRs2().is_virtual){

                        int rs2_regno=inst->getRs2().reg_no;
                       if(cfg->lattice_map[rs2_regno]->status==LatticeStatus::CONST &&
                          cfg->lattice_map[rs2_regno]->val.intval==1){
                            it = block->instructions.erase(it); 
                            continue;
                        }
                    }
                }
                // (4) addi(w) t0, x0, k
                //     add(w)sub xxx, xx, t0  --->直接使用立即数k       ---> 常量折叠！！！
                    // if(!rs1.is_virtual && rs1.reg_no == 0 ){// addi t0, x0, k型，t0被使用，可直接用k替换
                    //     auto next_it = std::next(it);
                    //     if (next_it != block->instructions.end()) {
                    //         auto next_inst = (RiscV64Instruction*)(*next_it);
                    //         if(next_inst->getOpcode() == RISCV_ADD || next_inst->getOpcode() == RISCV_ADDW||
                    //            next_inst->getOpcode() == RISCV_SUB || next_inst->getOpcode() == RISCV_SUBW) {
                    //             if(next_inst->getRs2().reg_no== inst->getRd().reg_no) {
                    //                 if(next_inst->getOpcode() == RISCV_ADD) {
                    //                     next_inst->setOpcode(RISCV_ADDI,false);
                    //                     next_inst->setImm(inst->getImm());
                    //                 } else if(next_inst->getOpcode() == RISCV_ADDW) {
                    //                     next_inst->setOpcode(RISCV_ADDIW,false);
                    //                     next_inst->setImm(inst->getImm());
                    //                 } else if(next_inst->getOpcode() == RISCV_SUB) {
                    //                     next_inst->setOpcode(RISCV_ADDI,false);
                    //                     next_inst->setImm(-(inst->getImm()));
                    //                 } else if(next_inst->getOpcode() == RISCV_SUBW) {
                    //                     next_inst->setOpcode(RISCV_ADDIW,false);
                    //                     next_inst->setImm(-(inst->getImm()));
                    //                 }
                                    
                    //                 it = block->instructions.erase(it);
                    //                 ++it;
                    //                 continue;
                    //             }
                    //         }// store 0型： addi tx, x0, 0;  sw tx, imm(reg)  ---> sw x0, imm(reg)
                    //         else if(next_inst->getOpcode() == RISCV_SW && next_inst->getRs1().reg_no == inst->getRd().reg_no
                    //                  && inst->getImm()==0){
                    //                 next_inst->setRs1(inst->getRs1());
                    //                 it = block->instructions.erase(it);
                    //                 ++it;
                    //                 continue;
                    //         }

                    //     }
                    // }
                
                ++it;
            }
        }
}

/*
fmv ft1, ft0            ---> use ft0 to replace ft1
(f)add(i) res, rs1, 0   ---> use rs1 to replace res
(f)sub res, rs1, x0     ---> use rs1 to replace res
(f)mul res, rs1, 1      ---> use rs1 to replace res
(f)div res, rs1, 1      ---> use rs1 to replace res

 */
bool MachinePeePass::RedundantReplacementEliminate(MachineCFG* cfg){
    std::unordered_map<int,int> replaced_map;
    //标记信息
    for (auto &[id,block]: cfg->block_map) {
        for (auto it = block->instructions.begin(); it != block->instructions.end();) {
            if((*it)->arch!=MachineBaseInstruction::Type::RiscV) {++it; continue;}
            auto inst = (RiscV64Instruction*)(*it);
            auto def_regs = inst->GetWriteReg();
            if(def_regs.size() == 1 && def_regs[0]->is_virtual){ // multi_def的，无法在编译器替换
                if(cfg->def_map[def_regs[0]->reg_no].size() >1){
                    ++it;
                    continue;  
                }
            }

            if(inst->getOpcode()==RISCV_FMV_S){
                if(inst->getRd().is_virtual && inst->getRs1().is_virtual && 
                inst->getRd().reg_no != inst->getRs1().reg_no){
                    replaced_map[inst->getRd().reg_no]=inst->getRs1().reg_no;
                    it = block->instructions.erase(it);
                    continue;
                }
            }else if(inst->getOpcode()==RISCV_ADDI || inst->getOpcode()==RISCV_ADDIW){
                if(inst->getRd().is_virtual && inst->getRs1().is_virtual &&
                inst->getRd().reg_no != inst->getRs1().reg_no){
                    if(inst->getImm()==0){
                        replaced_map[inst->getRd().reg_no]=inst->getRs1().reg_no;
                        it = block->instructions.erase(it);
                        continue;
                    }
                }
            }else if(inst->getOpcode()==RISCV_ADD || inst->getOpcode()==RISCV_ADDW){
                    if(inst->getRd().is_virtual && inst->getRs1().is_virtual &&
                    inst->getRd().reg_no != inst->getRs1().reg_no){
                        auto const_status = getConstVal(inst->getRs2(),cfg);
                        if(const_status.first && const_status.second==0){
                            replaced_map[inst->getRd().reg_no]=inst->getRs1().reg_no;
                            it = block->instructions.erase(it);
                            continue;
                        }
                    }else if(inst->getRd().is_virtual && inst->getRs2().is_virtual &&
                    inst->getRd().reg_no != inst->getRs2().reg_no){
                        auto const_status = getConstVal(inst->getRs1(),cfg);
                        if(const_status.first && const_status.second==0){
                            replaced_map[inst->getRd().reg_no]=inst->getRs2().reg_no;
                            it = block->instructions.erase(it);
                            continue;
                        }
                    }
            }else if(inst->getOpcode()==RISCV_SUB || inst->getOpcode()==RISCV_SUBW){
                    if(inst->getRd().is_virtual && inst->getRs1().is_virtual &&
                    inst->getRd().reg_no != inst->getRs1().reg_no){
                        auto const_status = getConstVal(inst->getRs2(),cfg);
                        if(const_status.first && const_status.second==0){
                            replaced_map[inst->getRd().reg_no]=inst->getRs1().reg_no;
                            it = block->instructions.erase(it);
                            continue;
                        }
                    }
            }else if(inst->getOpcode()==RISCV_MUL || inst->getOpcode()==RISCV_MULW){
                    if(inst->getRd().is_virtual && inst->getRs1().is_virtual &&
                    inst->getRd().reg_no != inst->getRs1().reg_no){
                        auto const_status = getConstVal(inst->getRs2(),cfg);
                        if(const_status.first && const_status.second==1){
                            replaced_map[inst->getRd().reg_no]=inst->getRs1().reg_no;
                            it = block->instructions.erase(it);
                            continue;
                        }
                    }else if(inst->getRd().is_virtual && inst->getRs2().is_virtual &&
                    inst->getRd().reg_no != inst->getRs2().reg_no){
                        auto const_status = getConstVal(inst->getRs1(),cfg);
                        if(const_status.first && const_status.second==1){
                            replaced_map[inst->getRd().reg_no]=inst->getRs2().reg_no;
                            it = block->instructions.erase(it);
                            continue;
                        }
                    }
            }else if(inst->getOpcode()==RISCV_DIV || inst->getOpcode()==RISCV_DIVW){
                    if(inst->getRd().is_virtual && inst->getRs1().is_virtual &&
                    inst->getRd().reg_no != inst->getRs1().reg_no){
                        auto const_status = getConstVal(inst->getRs2(),cfg);
                        if(const_status.first && const_status.second==1){
                            replaced_map[inst->getRd().reg_no]=inst->getRs1().reg_no;
                            it = block->instructions.erase(it);
                            continue;
                        }
                    }
            }else if(inst->getOpcode()==RISCV_FADD_S){
                    if(inst->getRd().is_virtual && inst->getRs1().is_virtual &&
                    inst->getRd().reg_no != inst->getRs1().reg_no){
                        auto const_status = getFloatConstVal(inst->getRs2(),cfg);
                        if(const_status.first && const_status.second==0){
                            replaced_map[inst->getRd().reg_no]=inst->getRs1().reg_no;
                            it = block->instructions.erase(it);
                            continue;
                        }
                    }else if(inst->getRd().is_virtual && inst->getRs2().is_virtual &&
                    inst->getRd().reg_no != inst->getRs2().reg_no){
                        auto const_status = getFloatConstVal(inst->getRs1(),cfg);
                        if(const_status.first && const_status.second==0){
                            replaced_map[inst->getRd().reg_no]=inst->getRs2().reg_no;
                            it = block->instructions.erase(it);
                            continue;
                        }
                    }
            }else if(inst->getOpcode()==RISCV_FSUB_S){
                    if(inst->getRd().is_virtual && inst->getRs1().is_virtual &&
                    inst->getRd().reg_no != inst->getRs1().reg_no){
                        auto const_status = getFloatConstVal(inst->getRs2(),cfg);
                        if(const_status.first && const_status.second==0){
                            replaced_map[inst->getRd().reg_no]=inst->getRs1().reg_no;
                            it = block->instructions.erase(it);
                            continue;
                        }
                    }
            }else if(inst->getOpcode()==RISCV_FMUL_S){
                    if(inst->getRd().is_virtual && inst->getRs1().is_virtual &&
                    inst->getRd().reg_no != inst->getRs1().reg_no){
                        auto const_status = getFloatConstVal(inst->getRs2(),cfg);
                        if(const_status.first && const_status.second==1){
                            replaced_map[inst->getRd().reg_no]=inst->getRs1().reg_no;
                            it = block->instructions.erase(it);
                            continue;
                        }
                    }else if(inst->getRd().is_virtual && inst->getRs2().is_virtual &&
                    inst->getRd().reg_no != inst->getRs2().reg_no){
                        auto const_status = getFloatConstVal(inst->getRs1(),cfg);
                        if(const_status.first && const_status.second==1){
                            replaced_map[inst->getRd().reg_no]=inst->getRs2().reg_no;
                            it = block->instructions.erase(it);
                            continue;
                        }
                    }
            }else if(inst->getOpcode()==RISCV_FDIV_S){
                    if(inst->getRd().is_virtual && inst->getRs1().is_virtual &&
                    inst->getRd().reg_no != inst->getRs1().reg_no){
                        auto const_status = getFloatConstVal(inst->getRs2(),cfg);
                        if(const_status.first && const_status.second==1){
                            replaced_map[inst->getRd().reg_no]=inst->getRs1().reg_no;
                            it = block->instructions.erase(it);
                            continue;
                        }
                    }
            }
            ++it;
        }
    }

    //执行替换
    for(auto &[id,block]: cfg->block_map) {
        for (auto it = block->instructions.begin(); it != block->instructions.end();++it) {
            if((*it)->arch!=MachineBaseInstruction::Type::RiscV) {continue;}
            auto inst = (RiscV64Instruction*)(*it);
            if(OpTable[inst->getOpcode()].ins_formattype==RvOpInfo::CALL_type){continue;}
            auto read_regs=inst->GetReadReg();
            for(auto &reg:read_regs){
                if(reg->is_virtual && replaced_map.count(reg->reg_no)){
                    int replace_regno=replaced_map[reg->reg_no];
                    while(replaced_map.count(replace_regno)){
                        std::cout<<"Replaced "<<replace_regno;
                        replace_regno=replaced_map[replace_regno];
                        std::cout<<" by "<<replace_regno<<std::endl;
                    }
                    reg->reg_no=replace_regno;
                }
            }
        }
    }

    if(replaced_map.empty()) {return false;}
    return true;

}

//用立即数替换存储常量的寄存器
/*
三种类型：
    1. add的一个操作数是imm,且imm属于[-2048,2047]，直接改为addi,否则用li装载立即数
    2. sub/mul/div/rem等的一个操作数是imm，且imm属于[-2048,2047],用addi装载立即数
    3. sub/mul/div/rem等的一个操作数是imm，且imm不属于[-2048,2047],用li装载立即数
    PS: 若imm=0，直接用x0替换
思路：
    1. 无def_reg的指令，不删除，仅替换use_reg
    2. 有def_reg,且 !def_reg.is_virtual || !def_reg.is_const ，不删除，仅替换use_reg
    3. 有def_reg,且 def_reg.is_virtual && def_reg.is_const，删除指令
*/

// return : 0保留， 1 表示删除, 2 表示替换  --> 在此处保留，最后删除无用指令
int ReplacedOrDelete(RiscV64Instruction*inst,  std::vector<Register *> write_regs,MachineCFG* cfg){
    if(IsFloatInst(inst->getOpcode())){
        return 0; //暂不处理浮点数
    }
    if(write_regs.empty()) {
        return 2; 
    }else if(write_regs.size()==1){
        if(write_regs[0]->is_virtual && 
           cfg->lattice_map.count(write_regs[0]->reg_no) &&
           cfg->lattice_map[write_regs[0]->reg_no]->status==LatticeStatus::CONST) {
            // auto use_insts=cfg->use_map[write_regs[0]->reg_no];
            // for(auto &use_inst:use_insts){
            //     if(use_inst->arch!=MachineBaseInstruction::Type::RiscV) {continue;}
            //     if(IsFloatInst(((RiscV64Instruction*)use_inst)->getOpcode())) {return 0;}
            // }
            return 0; 
        }else{
            return 2; 
        }
    }else{
        assert(0);
    }
}

bool MachinePeePass::ReplaceReg(Register reg, MachineBlock*block,MachineCFG* cfg){
    // return : true 表示用x0，false 表示仍用原来寄存器
    //非virtual&const,不替换
    if(!reg.is_virtual || 
        !cfg->lattice_map.count(reg.reg_no) ||
        cfg->lattice_map[reg.reg_no]->status!=LatticeStatus::CONST){
        return false;
    }

    //int 
    if(reg.type.data_type==MachineDataType::INT){
        int const_val=cfg->lattice_map[reg.reg_no]->val.intval;
        if(const_val==0) {return true;}//用x0
        if(const_val>=-2048 && const_val<=2047){//用addi
            RiscV64Instruction *addi_inst = new RiscV64Instruction();
            //if(reg.getDataWidth()==32){
                addi_inst->setOpcode(RISCV_ADDI,false);
            // }else if(reg.getDataWidth()==64){
            //     addi_inst->setOpcode(RISCV_ADDIW,false);
            // }else{
            //     assert(0);
            // }
            addi_inst->setRd(Register(reg));
            addi_inst->setRs1(RISCVregs[RISCV_x0]);
            addi_inst->setImm(const_val);
            block->instructions.push_back(addi_inst);
            return false;
        }else{//不在12位范围内
            RiscV64Instruction *li_inst = new RiscV64Instruction();
            li_inst->setOpcode(RISCV_LI,false);
            li_inst->setRd(Register(reg));
            li_inst->setImm(const_val);
            block->instructions.push_back(li_inst);
            return false;
        }
    }else{//float
        float const_val=cfg->lattice_map[reg.reg_no]->val.floatval;
        uint32_t bits;
        static_assert(sizeof(bits)==sizeof(float));
        std::memcpy(&bits, &const_val, sizeof(bits));//将float做位级重解释成uint32_t

        auto int_reg=new Register();
        if(const_val==0) {//用x0
            *int_reg=RISCVregs[RISCV_x0];
        } else if(const_val>=-2048 && const_val<=2047){//用addi
        //else if(fits_simm12((int32_t)bits)){//用addi
            RiscV64Instruction *addi_inst = new RiscV64Instruction();
            //if(reg.getDataWidth()==32){
            addi_inst->setOpcode(RISCV_ADDI,false);
            // }else if(reg.getDataWidth()==64){
            //     addi_inst->setOpcode(RISCV_ADDIW,false);
            // }else{
            //     assert(0);
            // }
            *int_reg = unit->functions[0]->GetNewRegister(MachineDataType::INT,MachineDataType::B64);
            addi_inst->setRd(*int_reg);//新建寄存器对所有函数统一regno
            addi_inst->setRs1(RISCVregs[RISCV_x0]);
            addi_inst->setImm(const_val);
            block->instructions.push_back(addi_inst);
        }else{//不在12位范围内
            RiscV64Instruction *li_inst = new RiscV64Instruction();
            li_inst->setOpcode(RISCV_LI,false);
            *int_reg = unit->functions[0]->GetNewRegister(MachineDataType::INT,MachineDataType::B64);
            li_inst->setRd(*int_reg);
            li_inst->setImm(const_val);
            block->instructions.push_back(li_inst);
        }
        //int-->float类型转换
        RiscV64Instruction *i2f_inst = new RiscV64Instruction();
        i2f_inst->setOpcode(RISCV_FMV_W_X,false);
        i2f_inst->setRd(Register(reg));
        i2f_inst->setRs1(Register(*int_reg)); 
        block->instructions.push_back(i2f_inst);
        return false;
    }
    
    return false;
}



//对整型做常量替换
void MachinePeePass::ConstantFolding(MachineCFG* cfg){

    for (auto &[id,block]: cfg->block_map) {
        auto old_insts=block->instructions;
        block->instructions.clear();
        for (auto it = old_insts.begin(); it != old_insts.end();++it) {
            if((*it)->arch!=MachineBaseInstruction::Type::RiscV) {
                block->instructions.push_back(*it);
                continue;
            }
            auto inst = (RiscV64Instruction*)(*it);
            if(OpTable[inst->getOpcode()].ins_formattype==RvOpInfo::CALL_type){
                block->instructions.push_back(*it);
                continue;
            }

            std::cout<<"Block "<<block->getLabelId()<<" inst: "<<OpTable[inst->getOpcode()].name<<std::endl;
            int status=ReplacedOrDelete(inst,inst->GetWriteReg(),cfg);
            if(status==1){//删除
                continue;
            }else if(status==0){//保留
                block->instructions.push_back(inst);
                
            }else{//替换
                if(inst->getOpcode()==RISCV_ADD || inst->getOpcode()==RISCV_ADDW){
                    auto rs1=inst->getRs1();
                    auto rs2=inst->getRs2();
                    bool rs1_is_const=false,rs2_is_const=false;
                    int rs1_const_val,rs2_const_val;
                    if(rs1.is_virtual && cfg->lattice_map.count(rs1.reg_no) &&
                       cfg->lattice_map[rs1.reg_no]->status==LatticeStatus::CONST){

                        if(cfg->lattice_map[rs1.reg_no]->val.intval >=-2048 &&
                           cfg->lattice_map[rs1.reg_no]->val.intval <=2047){
                            rs1_const_val=cfg->lattice_map[rs1.reg_no]->val.intval;
                            //改为addi指令
                            if(inst->getOpcode()==RISCV_ADD){
                                inst->setOpcode(RISCV_ADDI,false);
                            }else{
                                inst->setOpcode(RISCV_ADDIW,false);
                            }
                            inst->setImm(rs1_const_val);
                            inst->setRs1(Register(rs2));
                            block->instructions.push_back(inst);
                        }else{
                            ReplaceReg(rs1,block,cfg);//添加li指令装载立即数，仍使用原寄存器
                            block->instructions.push_back(inst);
                        }

                    }else if(rs2.is_virtual && cfg->lattice_map.count(rs2.reg_no) &&
                       cfg->lattice_map[rs2.reg_no]->status==LatticeStatus::CONST){
                        if(cfg->lattice_map[rs2.reg_no]->val.intval >=-2048 &&
                            cfg->lattice_map[rs2.reg_no]->val.intval <=2047){
                            rs2_const_val=cfg->lattice_map[rs2.reg_no]->val.intval;
                            //改为addi指令
                            if(inst->getOpcode()==RISCV_ADD){
                                inst->setOpcode(RISCV_ADDI,false);
                            }else{
                                inst->setOpcode(RISCV_ADDIW,false);
                            }
                            inst->setImm(rs2_const_val);
                            block->instructions.push_back(inst);
                        }else{
                            ReplaceReg(rs2,block,cfg);//添加li指令装载立即数，仍使用原寄存器
                            block->instructions.push_back(inst);
                        }
                    }else{
                        block->instructions.push_back(inst);
                    }
                }else{
                    auto read_regs=inst->GetReadReg();
                    for(auto &reg:read_regs){
                        if(ReplaceReg(*reg,block,cfg)){
                            reg->is_virtual=false;
                            reg->reg_no=RISCV_x0;
                        }
                    }
                    block->instructions.push_back(inst);
                }
            }
        }
    }
}


void MachinePeePass::DCE(MachineCFG* cfg){
    std::unordered_set<int> used_regno;
    for (auto &[id,block]: cfg->block_map) {
        for(auto it=block->instructions.begin();it!=block->instructions.end();++it){
            if((*it)->arch==MachineBaseInstruction::RiscV){
                auto inst=(RiscV64Instruction*)(*it);
                auto read_regs=inst->GetReadReg();
                for(auto &reg:read_regs){
                    if(reg->is_virtual){
                        used_regno.insert(reg->reg_no);
                    }
                }
            }
        }
    }
    for (auto &[id,block]: cfg->block_map) {
        for(auto it=block->instructions.begin();it!=block->instructions.end();){
            if((*it)->arch!=MachineBaseInstruction::RiscV){++it;continue;}
            auto inst=(RiscV64Instruction*)(*it);
            auto write_regs=inst->GetWriteReg();
            if(write_regs.size()==1){
                if(write_regs[0]->is_virtual && !used_regno.count(write_regs[0]->reg_no)){
                    it = block->instructions.erase(it);
                    continue;
                }else{
                    ++it;
                }
            } else{
                ++it;
            }
        }
    }
    
}   

void MachinePeePass::FloatCompFusion(){
    for (auto &func : unit->functions) {
        for (auto &block : func->blocks) {
            for (auto it = block->instructions.begin(); it != block->instructions.end();) {
                auto inst = (RiscV64Instruction*)(*it);

                //（5）fma /fnma
                if(inst ->getOpcode() == RISCV_FMUL_S){
                    auto next_it = std::next(it);
                    if (next_it != block->instructions.end()) {
                        auto next_inst = (RiscV64Instruction*)(*next_it);
						// FMA 指令单次舍入，精度较高，所以导致了浮点数输出结果错误，暂时关闭
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