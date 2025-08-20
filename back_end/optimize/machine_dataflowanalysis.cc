#include "machine_dataflowanalysis.h"

static inline int64_t sext32(int64_t x) { return (int64_t)(int32_t)x; }

void DataFlowAnalysisPass::Execute(){
    std::cout<<"Start !"<<std::endl;
    //SSA def-use链分析
    for(auto &func:unit->functions){
        MachineCFG* cfg=func->getMachineCFG();
        
        for(auto &[id,block]:cfg->block_map){
            for(auto it=block->instructions.begin();it!=block->instructions.end();++it){
                if((*it)->arch==MachineBaseInstruction::RiscV){
                    auto inst=(RiscV64Instruction*)(*it);
                    //标记use_map
                    auto read_regs=inst->GetReadReg();
                    for(auto &reg:read_regs){
                        if(reg->is_virtual){
                            cfg->use_map[reg->reg_no].insert(inst);
                        }
                    }
                    //标记def_map
                    auto write_regs=inst->GetWriteReg();
                    if(write_regs.size()==1){
                        if(write_regs[0]->is_virtual){
                            cfg->def_map[write_regs[0]->reg_no].insert(inst);
                            //如果被定义多次，直接判定为varient,后续也不必再进行分析
                            if(cfg->lattice_map.count(write_regs[0]->reg_no) ){
                                cfg->lattice_map[write_regs[0]->reg_no] ->status = LatticeStatus::VARIENT;
                            }else{
                                cfg->lattice_map[write_regs[0]->reg_no]=new LatticeStatus();
                            }
                        }
                    }
                }
            }
        }
    }
    std::cout<<"Middle !"<<std::endl;
    //常量状态传播（仅标记）
    for(auto &func:unit->functions){
        MachineCFG* cfg=func->getMachineCFG();
        ConstPropagation(cfg);
    }
    std::cout<<"End !"<<std::endl;
}


// 常量传播，仅标记在Register上，不进行替换/删除操作
void DataFlowAnalysisPass::ConstPropagationExecute(){
    for(auto &func:unit->functions){
        MachineCFG* cfg=func->getMachineCFG();
        
        bool changed=false;
        do{
            changed = ConstPropagation(cfg);
        }while(changed);
    }
}

/*
判断两种情况：
    - x0 ：返回value=0
    - virtural & const : 返回const_val
*/
std::pair<bool,int> getConstVal(Register reg, MachineCFG* cfg){
    if(reg.is_virtual){
        int regno=reg.reg_no;
        if(!cfg->lattice_map.count(regno)) {return {false,0};}
        if(cfg->lattice_map[regno]->status==LatticeStatus::CONST){
            int value=cfg->lattice_map[regno]->val.intval;
            return {true,value};
        }
        return {false,0};
    }else if(reg.reg_no==RISCV_x0){
        return {true,0};
    }
    return {false,0};
}

std::pair<bool,float> getFloatConstVal(Register reg, MachineCFG* cfg){
    if(reg.is_virtual){
        int regno=reg.reg_no;
        if(!cfg->lattice_map.count(regno)) {return {false,0};}
        if(cfg->lattice_map[regno]->status==LatticeStatus::CONST){
            int value=cfg->lattice_map[regno]->val.floatval;
            return {true,value};
        }
        return {false,0};
    }
    return {false,0};
}

LatticeStatus::Status computeBinaryStatus(Register r1,Register r2, MachineCFG* cfg){
    if(!r1.is_virtual){
        if(r1.reg_no==RISCV_x0){
            if(!r2.is_virtual){
                assert(r2.reg_no!=RISCV_x0);
                return LatticeStatus::UNDEF;
            }else{
                return cfg->lattice_map[r2.reg_no]->status;
            }
        }else{
            return LatticeStatus::UNDEF;
        }
    }

    if(!r2.is_virtual){
        if(r2.reg_no==RISCV_x0){
            if(!r1.is_virtual){
                assert(r1.reg_no!=RISCV_x0);
                return LatticeStatus::UNDEF;
            }else{
                return cfg->lattice_map[r1.reg_no]->status;
            }
        }else{
            return LatticeStatus::UNDEF;
        }
    }

    // r1, r2 都是virtual
    auto l1=cfg->lattice_map[r1.reg_no]!=nullptr ? cfg->lattice_map[r1.reg_no]->status :LatticeStatus::VARIENT;
    auto l2=cfg->lattice_map[r2.reg_no]!=nullptr ? cfg->lattice_map[r2.reg_no]->status :LatticeStatus::VARIENT;

    if( l1 == LatticeStatus::VARIENT || l2 == LatticeStatus::VARIENT){
        return LatticeStatus::VARIENT;
    }else if(l1 == LatticeStatus::CONST && l2 == LatticeStatus::CONST ){
        return LatticeStatus::CONST;
    }
    return LatticeStatus::UNDEF;
}


// int64_t getArithmeticImm(int a, int b, bool is_w, int comp){
//     if(is_w){

//     }else{
//         switch (comp) {
//             case 1:// + 

//         }
//     }
// }

//立即数替换仅支持12位以内的
bool DataFlowAnalysisPass::ConstPropagation(MachineCFG* cfg){
    bool changed = false;

    for(auto &[id,block]:cfg->block_map){
        for(auto it=block->instructions.begin();it!=block->instructions.end();++it){
            if((*it)->arch==MachineBaseInstruction::Type::RiscV){
                auto inst=(RiscV64Instruction*)(*it);
                auto def_reg=inst->GetWriteReg();

                if(def_reg.size()!=1){continue;}
                int res_regno=def_reg[0]->reg_no;
                //消除phi后非SSA，可能被定义多次；
                //所有被定义多次的都认为是VARIENT
                if(!(def_reg[0]->is_virtual &&
                   cfg->lattice_map[res_regno]->status==LatticeStatus::UNDEF)){continue;}
                
                // std::cout<<"[后端SCCP] def_reg: "<<res_regno<<"   ;  read_reg: ";
                // auto read_regs=inst->GetReadReg();
                // for(auto &read_reg:read_regs){
                //     if(read_reg->is_virtual){
                //         std::cout<<read_reg->reg_no<<" ";
                //     }
                // }std::cout<<std::endl;

                //仅对会定义reg，且res为Undef的进行传播
                switch(inst->getOpcode()){
                    case RISCV_ADDI:
                    case RISCV_ADDIW:{
                        //std::cout<<"It is a ADDI inst!"<<std::endl;
                        int imm=inst->getImm();
                        if(inst->getRs1().is_virtual){
                            int rs1_regno=inst->getRs1().reg_no;
                            if(cfg->lattice_map[rs1_regno]->status==LatticeStatus::CONST){
                                int const_val=cfg->lattice_map[rs1_regno]->val.intval+imm;
                                //if(const_val>=-2048&&const_val<=2047){
                                    cfg->lattice_map[res_regno]->status=LatticeStatus::CONST;
                                    cfg->lattice_map[res_regno]->val.intval=const_val;
                                    changed=true;
                                    break;
                                //}
                            }
                            cfg->lattice_map[res_regno]->status=cfg->lattice_map[rs1_regno]->status;
                            if(cfg->lattice_map[res_regno]->status!=LatticeStatus::CONST){
                                changed=true;
                            }
                        }else if(inst->getRs1().reg_no==RISCV_x0){
                            int const_val=imm;
                            cfg->lattice_map[res_regno]->status=LatticeStatus::CONST;
                            cfg->lattice_map[res_regno]->val.intval=const_val;
                            changed=true;
                        }
                        
                        break;
                    }
                    case RISCV_ADD:
                    case RISCV_ADDW:{
                        //std::cout<<"It is a ADD inst!"<<std::endl;
                        if((!inst->getRs1().is_virtual && inst->getRs1().reg_no!=RISCV_x0) ||
                           (!inst->getRs2().is_virtual && inst->getRs2().reg_no!=RISCV_x0)){
                            cfg->lattice_map[res_regno]->status = LatticeStatus::VARIENT;
                            changed=true;
                            break;
                        }
                        auto const1=getConstVal(inst->getRs1(),cfg);
                        auto const2=getConstVal(inst->getRs2(),cfg);
                        if(const1.first && const2.first ){
                            int const_val=const1.second + const2.second;
                            //if(const_val>=-2048 && const_val <=2047){
                                cfg->lattice_map[res_regno]->status=LatticeStatus::CONST;
                                cfg->lattice_map[res_regno]->val.intval= const_val;
                            // }else{
                            //     cfg->lattice_map[res_regno]->status = LatticeStatus::VARIENT;
                            // }
                        }else{
                            cfg->lattice_map[res_regno]->status=computeBinaryStatus(inst->getRs1(),inst->getRs2(),cfg);
                        }

                        if(cfg->lattice_map[res_regno]->status!=LatticeStatus::UNDEF){
                            changed=true;
                        }
                        break;
                    }
                    case RISCV_SUB:
                    case RISCV_SUBW:{
                        //std::cout<<"It is a SUB inst!"<<std::endl;
                        if((!inst->getRs1().is_virtual && inst->getRs1().reg_no!=RISCV_x0) ||
                           (!inst->getRs2().is_virtual && inst->getRs2().reg_no!=RISCV_x0)){
                            cfg->lattice_map[res_regno]->status = LatticeStatus::VARIENT;
                            changed=true;
                            break;
                        }
                        // 特殊：0-0 --> 0
                        if(inst->getRs1().reg_no==inst->getRs2().reg_no){
                            cfg->lattice_map[res_regno]->status=LatticeStatus::CONST;
                            cfg->lattice_map[res_regno]->val.intval= 0;
                            changed=true;
                            break;
                        }

                        auto const1=getConstVal(inst->getRs1(),cfg);
                        auto const2=getConstVal(inst->getRs2(),cfg);
                        if(const1.first && const2.first ){
                            int const_val=const1.second - const2.second;
                            //if(const_val>=-2048 && const_val <=2047){
                                cfg->lattice_map[res_regno]->status=LatticeStatus::CONST;
                                cfg->lattice_map[res_regno]->val.intval= const_val;
                            // }else{
                            //     cfg->lattice_map[res_regno]->status = LatticeStatus::VARIENT;
                            // }
                        }else{
                            cfg->lattice_map[res_regno]->status=computeBinaryStatus(inst->getRs1(),inst->getRs2(),cfg);
                        }

                        if(cfg->lattice_map[res_regno]->status!=LatticeStatus::UNDEF){
                            changed=true;
                        }
                        break;
                    }
                    case RISCV_MUL:
                    case RISCV_MULW:{
                        //std::cout<<"It is a MUL inst!"<<std::endl;
                        if((!inst->getRs1().is_virtual && inst->getRs1().reg_no!=RISCV_x0) ||
                           (!inst->getRs2().is_virtual && inst->getRs2().reg_no!=RISCV_x0)){
                            cfg->lattice_map[res_regno]->status = LatticeStatus::VARIENT;
                            changed=true;
                            break;
                        }

                        auto const1=getConstVal(inst->getRs1(),cfg);
                        auto const2=getConstVal(inst->getRs2(),cfg);
                        if(const1.first && const2.first ){
                            int const_val=const1.second * const2.second;
                            //if(const_val>=-2048 && const_val <=2047){
                                cfg->lattice_map[res_regno]->status=LatticeStatus::CONST;
                                cfg->lattice_map[res_regno]->val.intval= const_val;
                            // }else{
                            //     cfg->lattice_map[res_regno]->status = LatticeStatus::VARIENT;
                            // }
                        }// 特殊：x * 0 ---> 0
                        else if((const1.first && const1.second==0) || (const2.first && const2.second==0)){
                            cfg->lattice_map[res_regno]->status=LatticeStatus::CONST;
                            cfg->lattice_map[res_regno]->val.intval= 0;
                        }else{
                            cfg->lattice_map[res_regno]->status=computeBinaryStatus(inst->getRs1(),inst->getRs2(),cfg);
                        }

                        if(cfg->lattice_map[res_regno]->status!=LatticeStatus::UNDEF){
                            changed=true;
                        }
                        break;
                    }
                    case RISCV_DIV:
                    case RISCV_DIVW:{
                        //std::cout<<"It is a DIV inst!"<<std::endl;
                        if((!inst->getRs1().is_virtual && inst->getRs1().reg_no!=RISCV_x0) ||
                           (!inst->getRs2().is_virtual && inst->getRs2().reg_no!=RISCV_x0)){
                            cfg->lattice_map[res_regno]->status = LatticeStatus::VARIENT;
                            changed=true;
                            break;
                        }
                        //特殊： x / x --> 1
                        if(inst->getRs1().reg_no==inst->getRs2().reg_no){
                            cfg->lattice_map[res_regno]->status=LatticeStatus::CONST;
                            cfg->lattice_map[res_regno]->val.intval= 1;
                            changed=true;
                            break;
                        }

                        auto const1=getConstVal(inst->getRs1(),cfg);
                        auto const2=getConstVal(inst->getRs2(),cfg);
                        if(const1.first && const2.first ){
                            int const_val=const1.second / const2.second;
                            //if(const_val>=-2048 && const_val <=2047){
                                cfg->lattice_map[res_regno]->status=LatticeStatus::CONST;
                                cfg->lattice_map[res_regno]->val.intval= const_val;
                            // }else{
                            //     cfg->lattice_map[res_regno]->status = LatticeStatus::VARIENT;
                            // }
                        }else{
                            cfg->lattice_map[res_regno]->status=computeBinaryStatus(inst->getRs1(),inst->getRs2(),cfg);
                        }

                        if(cfg->lattice_map[res_regno]->status!=LatticeStatus::UNDEF){
                            changed=true;
                        }
                        break;
                    }
                    case RISCV_REM:
                    case RISCV_REMW:{
                        //std::cout<<"It is a REM inst!"<<std::endl;
                        if((!inst->getRs1().is_virtual && inst->getRs1().reg_no!=RISCV_x0) ||
                           (!inst->getRs2().is_virtual && inst->getRs2().reg_no!=RISCV_x0)){
                            cfg->lattice_map[res_regno]->status = LatticeStatus::VARIENT;
                            changed=true;
                            break;
                        }
                        // 特殊: x % x ---> 0
                        if(inst->getRs1().reg_no==inst->getRs2().reg_no){
                            cfg->lattice_map[res_regno]->status=LatticeStatus::CONST;
                            cfg->lattice_map[res_regno]->val.intval= 0;
                            changed=true;
                            break;
                        }

                        auto const1=getConstVal(inst->getRs1(),cfg);
                        auto const2=getConstVal(inst->getRs2(),cfg);
                        if(const1.first && const2.first ){
                            int const_val=const1.second % const2.second;
                            //if(const_val>=-2048 && const_val <=2047){
                                cfg->lattice_map[res_regno]->status=LatticeStatus::CONST;
                                cfg->lattice_map[res_regno]->val.intval= const_val;
                            // }else{
                            //     cfg->lattice_map[res_regno]->status = LatticeStatus::VARIENT;
                            // }
                        }else{
                            cfg->lattice_map[res_regno]->status=computeBinaryStatus(inst->getRs1(),inst->getRs2(),cfg);
                        }

                        if(cfg->lattice_map[res_regno]->status!=LatticeStatus::UNDEF){
                            changed=true;
                        }
                        break;
                    }
                    case RISCV_SLLI:
                    case RISCV_SLLIW:{
                        //std::cout<<"It is a SLLI inst!"<<std::endl;
                        int imm=inst->getImm();
                        if(inst->getRs1().is_virtual){
                            int rs1_regno=inst->getRs1().reg_no;
                            if(cfg->lattice_map[rs1_regno]->status==LatticeStatus::CONST){
                                int const_val=(cfg->lattice_map[rs1_regno]->val.intval)<<imm;
                                //if(const_val>=-2048&&const_val<=2047){
                                    cfg->lattice_map[res_regno]->status=LatticeStatus::CONST;
                                    cfg->lattice_map[res_regno]->val.intval=const_val;
                                    changed=true;
                                    break;
                                //}
                            }
                            cfg->lattice_map[res_regno]->status=cfg->lattice_map[rs1_regno]->status;
                            if(cfg->lattice_map[res_regno]->status!=LatticeStatus::CONST){
                                changed=true;
                            }
                        }else if(inst->getRs1().reg_no==RISCV_x0){
                            cfg->lattice_map[res_regno]->status=LatticeStatus::CONST;
                            cfg->lattice_map[res_regno]->val.intval=0;
                            changed=true;
                        }
                        break;
                    }
                    case RISCV_LI:{
                        int imm = inst->getImm();
                        cfg->lattice_map[res_regno]->status=LatticeStatus::CONST;
                        cfg->lattice_map[res_regno]->val.intval=imm;
                        changed=true;
                        break;
                    }
                    case RISCV_LUI:{
                        int imm = inst->getImm();
                        cfg->lattice_map[res_regno]->status=LatticeStatus::CONST;
                        cfg->lattice_map[res_regno]->val.intval=(imm<<12);
                        changed=true;
                        break;
                    }
                    case RISCV_FMV_S:{
                        if(!inst->getRs1().is_virtual){
                            cfg->lattice_map[res_regno]->status=LatticeStatus::VARIENT;
                            changed=true;
                            break;
                        }
                        int rs1_regno=inst->getRs1().reg_no;
                        if(cfg->lattice_map.count(rs1_regno)){
                            if(cfg->lattice_map[rs1_regno]->status==LatticeStatus::CONST){
                                cfg->lattice_map[res_regno]->status=LatticeStatus::CONST;
                                cfg->lattice_map[res_regno]->val.floatval=cfg->lattice_map[rs1_regno]->val.floatval;
                                changed=true;
                                break;
                            }
                            cfg->lattice_map[res_regno]->status = cfg->lattice_map[rs1_regno]->status;
                            if(cfg->lattice_map[res_regno]->status!=LatticeStatus::UNDEF){
                                changed=true;
                            }
                        }else{
                            cfg->lattice_map[res_regno]->status=LatticeStatus::VARIENT;
                            changed=true;
                        }

                        break;
                    }
                    case RISCV_FMV_W_X:{//int-->float
                        int rs1_regno=inst->getRs1().reg_no;
                        auto const_status = getConstVal(inst->getRs1(),cfg);
                        if(const_status.first){
                            cfg->lattice_map[res_regno]->status=LatticeStatus::CONST;
                            cfg->lattice_map[res_regno]->val.floatval=const_status.second;
                        }else if(!inst->getRs1().is_virtual){
                            cfg->lattice_map[res_regno]->status=LatticeStatus::VARIENT;
                        }else{
                            if(!cfg->lattice_map.count(rs1_regno)){
                                cfg->lattice_map[res_regno]->status=LatticeStatus::VARIENT;
                            }else{
                                cfg->lattice_map[res_regno]->status=cfg->lattice_map[rs1_regno]->status;
                            }
                        }

                        if(cfg->lattice_map[res_regno]->status!=LatticeStatus::UNDEF){
                            changed=true;
                        }
                        break;
                    }
                    case RISCV_FMV_X_W:{// float-->int
                        int rs1_regno=inst->getRs1().reg_no;
                        auto const_status = getConstVal(inst->getRs1(),cfg);
                        if(const_status.first){
                            cfg->lattice_map[res_regno]->status=LatticeStatus::CONST;
                            cfg->lattice_map[res_regno]->val.intval=const_status.second;
                        }else if(!inst->getRs1().is_virtual){
                            cfg->lattice_map[res_regno]->status=LatticeStatus::VARIENT;
                        }else{
                            if(!cfg->lattice_map.count(rs1_regno)){
                                cfg->lattice_map[res_regno]->status=LatticeStatus::VARIENT;
                            }else{
                                cfg->lattice_map[res_regno]->status=cfg->lattice_map[rs1_regno]->status;
                            }
                        }

                        if(cfg->lattice_map[res_regno]->status!=LatticeStatus::UNDEF){
                            changed=true;
                        }
                        break;
                    }
                    case RISCV_FADD_S:{
                        if((!inst->getRs1().is_virtual && inst->getRs1().reg_no!=RISCV_x0) ||
                           (!inst->getRs2().is_virtual && inst->getRs2().reg_no!=RISCV_x0)){
                            cfg->lattice_map[res_regno]->status = LatticeStatus::VARIENT;
                            changed=true;
                            break;
                        }
                        auto const1=getFloatConstVal(inst->getRs1(),cfg);
                        auto const2=getFloatConstVal(inst->getRs2(),cfg);
                        if(const1.first && const2.first ){
                            float const_val=const1.second + const2.second;
                            cfg->lattice_map[res_regno]->status=LatticeStatus::CONST;
                            cfg->lattice_map[res_regno]->val.floatval= const_val;
                        }else{
                            cfg->lattice_map[res_regno]->status=computeBinaryStatus(inst->getRs1(),inst->getRs2(),cfg);
                        }

                        if(cfg->lattice_map[res_regno]->status!=LatticeStatus::UNDEF){
                            changed=true;
                        }
                        break;
                    }
                    case RISCV_FMUL_S:{
                        if((!inst->getRs1().is_virtual && inst->getRs1().reg_no!=RISCV_x0) ||
                           (!inst->getRs2().is_virtual && inst->getRs2().reg_no!=RISCV_x0)){
                            cfg->lattice_map[res_regno]->status = LatticeStatus::VARIENT;
                            changed=true;
                            break;
                        }
                        auto const1=getFloatConstVal(inst->getRs1(),cfg);
                        auto const2=getFloatConstVal(inst->getRs2(),cfg);
                        if(const1.first && const2.first ){
                            float const_val=const1.second * const2.second;
                            cfg->lattice_map[res_regno]->status=LatticeStatus::CONST;
                            cfg->lattice_map[res_regno]->val.floatval= const_val;
                        }else{
                            cfg->lattice_map[res_regno]->status=computeBinaryStatus(inst->getRs1(),inst->getRs2(),cfg);
                        }

                        if(cfg->lattice_map[res_regno]->status!=LatticeStatus::UNDEF){
                            changed=true;
                        }
                        break;
                    }
                    case RISCV_FSUB_S:{
                        if((!inst->getRs1().is_virtual && inst->getRs1().reg_no!=RISCV_x0) ||
                           (!inst->getRs2().is_virtual && inst->getRs2().reg_no!=RISCV_x0)){
                            cfg->lattice_map[res_regno]->status = LatticeStatus::VARIENT;
                            changed=true;
                            break;
                        }
                        auto const1=getFloatConstVal(inst->getRs1(),cfg);
                        auto const2=getFloatConstVal(inst->getRs2(),cfg);
                        if(const1.first && const2.first ){
                            float const_val=const1.second - const2.second;
                            cfg->lattice_map[res_regno]->status=LatticeStatus::CONST;
                            cfg->lattice_map[res_regno]->val.floatval= const_val;
                        }else{
                            cfg->lattice_map[res_regno]->status=computeBinaryStatus(inst->getRs1(),inst->getRs2(),cfg);
                        }

                        if(cfg->lattice_map[res_regno]->status!=LatticeStatus::UNDEF){
                            changed=true;
                        }
                        break;
                    }
                    case RISCV_FDIV_S:{
                        if((!inst->getRs1().is_virtual && inst->getRs1().reg_no!=RISCV_x0) ||
                           (!inst->getRs2().is_virtual && inst->getRs2().reg_no!=RISCV_x0)){
                            cfg->lattice_map[res_regno]->status = LatticeStatus::VARIENT;
                            changed=true;
                            break;
                        }
                        auto const1=getFloatConstVal(inst->getRs1(),cfg);
                        auto const2=getFloatConstVal(inst->getRs2(),cfg);
                        if(const1.first && const2.first ){
                            float const_val=const1.second / const2.second;
                            cfg->lattice_map[res_regno]->status=LatticeStatus::CONST;
                            cfg->lattice_map[res_regno]->val.floatval= const_val;
                        }else{
                            cfg->lattice_map[res_regno]->status=computeBinaryStatus(inst->getRs1(),inst->getRs2(),cfg);
                        }

                        if(cfg->lattice_map[res_regno]->status!=LatticeStatus::UNDEF){
                            changed=true;
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
        }
    }

    return changed;
}