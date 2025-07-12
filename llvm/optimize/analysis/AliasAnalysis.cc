#include "AliasAnalysis.h"

//取指针操作数对应的PtrInfo结构体
PtrInfo AliasAnalysisPass::GetPtrInfo(Operand op, CFG* cfg){
    if(op->GetOperandType()==BasicOperand::REG){
        int regno=((RegOperand*)op)->GetRegNo();
        return ptrmap[cfg][regno];
    }else if(op->GetOperandType()==BasicOperand::GLOBAL){
        PtrInfo global_info(PtrInfo::types::Global,op);
        global_info.root=op;
        return global_info;
    }else{
        assert(0);
    }
}

//判定两个GEP指令是否具有相同的ptr、constIndexes
bool AliasAnalysisPass::IsSameArraySameConstIndex(GetElementptrInstruction* inst1,GetElementptrInstruction* inst2){
    //SysY未支持指针的数据类型，因此部分数组赋值仅存在于函数调用传参时；
    //即过程内别名分析不存在二级数组，故GEP指令中的ptr一定就是初次alloc时的ptr
    auto ptr1=inst1->GetPtrVal();
    auto ptr2=inst2->GetPtrVal();
    if(ptr1!=ptr2){ return false;}

    int index1=inst1->ComputeIndex();
    int index2=inst2->ComputeIndex();
    if(index1!=-1 && index2!=-1 && index1!=index2){
        return false;
    }
    return true;//对于索引为reg的情况，无法判定，我们都认为存在别名冲突风险
}


/*
 过程内别名分析
1.GroundTruth：
    - 同数组不同静态索引的指针一定不存在别名冲突
    - SysY中产生指针的指令只有Getelementptr（源）和phi；第一代（源）ptr的别名集中仅它自己
2.Analysis:
    - 别名集中同源 --> MustAlias （衍生自同一个GEP指令产生的源ptr)
    - 根对应的GEP指令同ptr、同ConstIndex --> MustAlias（衍生自位置不同但内容相同的GEP指令各自产生的源ptr)
    - 其它情况 --> NoAlias
*/
AliasStatus AliasAnalysisPass::QueryAlias(Operand op1, Operand op2, CFG* cfg){
    if(op1==op2){
        return MustAlias;
    }

    PtrInfo info1=GetPtrInfo(op1,cfg);
    PtrInfo info2=GetPtrInfo(op2,cfg); 

    //情况0.5 （我们认为不会这样访问，不过提供查询结果）
    if(info1.root==op2||info2.root==op1){// a 与 a[3]的别名存在冲突
        return MustAlias;
    }

    //不同全局变量之间、全局变量和局部变量之间 不会发生别名冲突
    if(info1.type==PtrInfo::types::Global||info2.type==PtrInfo::types::Global){
        if(info1.type==PtrInfo::types::Global&&info2.type==PtrInfo::types::Global){
            if(info1.root!=info2.root){
                return NoAlias;
            }
        }else{
            return NoAlias;
        }
    }

    //情况1
    for(auto &op:info1.AliasOps){
        if(info2.AliasOps.count(op)){
            return MustAlias;
        }
    }

    //情况2
    Instruction inst1,inst2;
    for(auto &op:info1.AliasOps){
        int regno=((RegOperand*)op)->GetRegNo();
        if(cfg->def_map[regno]->GetOpcode()==BasicInstruction::LLVMIROpcode::GETELEMENTPTR){
            inst1=cfg->def_map[regno];
            break;
        }
    }
    for(auto &op:info2.AliasOps){
        int regno=((RegOperand*)op)->GetRegNo();
        if(cfg->def_map[regno]->GetOpcode()==BasicInstruction::LLVMIROpcode::GETELEMENTPTR){
            inst2=cfg->def_map[regno];
            break;
        }
    }
    //assert(inst1!=nullptr&&inst2!=nullptr); 即 info1.source!=Undef &&  info1.source!=Undef
    if(IsSameArraySameConstIndex((GetElementptrInstruction*)inst1,(GetElementptrInstruction*)inst2)){
        return MustAlias;
    }
    //其它
    return NoAlias;

}

Operand AliasAnalysisPass::CalleeParamToCallerArgu(Operand op, CFG* callee_cfg, CallInstruction* CallI){
    if(op->GetOperandType()==BasicOperand::REG){
        auto callee_info = ReCallGraph[callee_cfg];
        if(callee_info.param_order.count(op)){//是形参，转换为caller的实参返回
            int order=callee_info.param_order[op];
            Operand argu=CallI->GetParameterList()[order].second;
            return argu;
        }else{//是local的，不予理会
            return nullptr;
        }
    }else if(op->GetOperandType()==BasicOperand::GLOBAL){//全局变量，直接返回
        return op;
    }
    return nullptr;
}

ModRefStatus AliasAnalysisPass::QueryInstModRef(Instruction inst, Operand op, CFG* cfg){
    if(inst->GetOpcode()==BasicInstruction::LLVMIROpcode::LOAD){
        LoadInstruction* load_inst = (LoadInstruction*)inst;
        Operand ptr=load_inst->GetPointer();
        if(QueryAlias(ptr,op,cfg)==MustAlias){
            return Ref;
        }
        return NoModRef;

    }else if(inst->GetOpcode()==BasicInstruction::LLVMIROpcode::STORE){
        StoreInstruction* store_inst = (StoreInstruction*)inst;
        Operand ptr=store_inst->GetPointer();
        if(QueryAlias(ptr,op,cfg)==MustAlias){
            return Mod;
        }
        return NoModRef;

    }else if(inst->GetOpcode()==BasicInstruction::LLVMIROpcode::CALL){
        CallInstruction* callI=(CallInstruction*)inst;
        auto callee_defI=llvmIR->FunctionNameTable[(callI)->GetFunctionName()];
        CFG* callee_cfg=llvmIR->llvm_cfg[callee_defI];
        RWInfo rwinfo=rwmap[callee_cfg];
        int read=0,write=0;

        for(auto &rop:rwinfo.ReadRoots){
            auto rop2=CalleeParamToCallerArgu(rop,callee_cfg,callI);
            if(rop2!=nullptr){
                //std::cout<<"[in QueryInstModRef CALL]  rop: "<<rop2->GetFullName()<<" op: "<<op->GetFullName()<<std::endl;
                if(QueryAlias(rop2,op,cfg)==MustAlias){
                    read=1;
                    break;
                }   
            }
            
        }
        for(auto &wop:rwinfo.WriteRoots){
            auto wop2=CalleeParamToCallerArgu(wop,callee_cfg,callI);
            if(wop2!=nullptr){
                //std::cout<<"[in QueryInstModRef CALL]  wop: "<<wop2->GetFullName()<<" op: "<<op->GetFullName()<<std::endl;
                if(QueryAlias(wop2,op,cfg)==MustAlias){
                    write=2;
                    break;
                }
            }
        }

        return static_cast<ModRefStatus>(read+write);
    }
    return NoModRef;
}


//从前往后，将别名汇聚
void AliasAnalysisPass::GatherPtrInfos(CFG* cfg, int regno){
    PtrInfo info=ptrmap[cfg][regno];
    std::vector<Instruction> use_insts = cfg->use_map[regno];
    for(auto &inst:use_insts){
        if(inst->GetOpcode()==BasicInstruction::LLVMIROpcode::PHI){
            int res_regno=inst->GetDefRegno();
            ptrmap[cfg][res_regno].AddAliass(info.AliasOps);//别名汇总
            if(ptrmap[cfg][res_regno].type==PtrInfo::types::Undefed){//类型传递
                ptrmap[cfg][res_regno].type=info.type;
            }
            ptrmap[cfg][res_regno].root=info.root;//祖宗传递
            GatherPtrInfos(cfg,res_regno);
        }
    }
    return ;
}

//别名分析信息收集：沿数据流图记录并汇聚别名集
void AliasAnalysisPass::PtrPropagationAnalysis(){
    for(auto &[defI,cfg]:llvmIR->llvm_cfg){
        //【0】记录数组性质
        CallInfo callinfo;
        // (1)函数参数
        auto params = defI->GetNonResultOperands();
        auto param_types = defI->formals;
        for(int i=0;i<params.size();i++){
            if(param_types[i]==BasicInstruction::LLVMType::PTR){
                int regno = ((RegOperand*)params[i])->GetRegNo();
                PtrInfo info(PtrInfo::sources::Undef,PtrInfo::types::Param,params[i]);
                info.root=params[i];
                ptrmap[cfg][regno]=info;//维护ptrmap

                callinfo.param_order[params[i]]=i;//记录函数形参与序号的映射
            }
        }
        ReCallGraph[cfg]=callinfo;
        //（2）函数内定义
        for(auto &[id,block]:*(cfg->block_map)){
            for(auto &inst: block->Instruction_list){
                if(inst->GetOpcode()==BasicInstruction::LLVMIROpcode::ALLOCA){
                    assert(((AllocaInstruction*)inst)->GetDims().size()>0);
                    Operand PtrOp = inst->GetResult();
                    int regno = ((RegOperand*)PtrOp)->GetRegNo();
                    PtrInfo info(PtrInfo::sources::Undef,PtrInfo::types::Local,PtrOp);
                    info.root=PtrOp;
                    ptrmap[cfg][regno]=info;
                }
            }
        }

        //【1】遍历所有cfg，记录会访问的ptr操作数（即由GEP、PHI指令产生）的信息
        for(auto &[regno,inst]:cfg->def_map){
            if(inst->GetOpcode()==BasicInstruction::LLVMIROpcode::GETELEMENTPTR){
                Operand ptr = ((GetElementptrInstruction*)inst)->GetPtrVal();
                PtrInfo ptr_info = GetPtrInfo(ptr,cfg);
                //类型传递
                PtrInfo info(PtrInfo::sources::Gep, ptr_info.type ,inst->GetResult());
                info.root=ptr;
                ptrmap[cfg][regno]=info;
            }else if(inst->GetOpcode()==BasicInstruction::LLVMIROpcode::PHI){
                if(((PhiInstruction*)inst)->GetResultType()==BasicInstruction::LLVMType::PTR){
                    PtrInfo info(PtrInfo::sources::Phi,PtrInfo::types::Undefed,inst->GetResult());
                    ptrmap[cfg][regno]=info;
                }
            }
        }
        //【2】汇聚非根ptr的所有别名
        for(auto &[regno,info]:ptrmap[cfg]){
            if(info.source==PtrInfo::sources::Gep){
                GatherPtrInfos(cfg,regno);
            }
        }
    }
}

//分析每个CFG的读写情况；并通过调用流汇总
void AliasAnalysisPass::RWInfoAnalysis(){
    //【1】收集每个CFG的读写信息，同时建立反向CallGraph
    for(auto &[defI,cfg]:llvmIR->llvm_cfg){
        RWInfo rwinfo;
        LeafFuncs.insert(cfg);
        for(auto &[id,block]:*(cfg->block_map)){
            for(auto &inst:block->Instruction_list){
                if(inst->GetOpcode()==BasicInstruction::LLVMIROpcode::LOAD){
                    Operand ptr=((LoadInstruction*)inst)->GetPointer();
                    PtrInfo info=GetPtrInfo(ptr,cfg);
                    if(info.type==PtrInfo::types::Global||info.type==PtrInfo::types::Param){
                        rwinfo.AddRead(info.root);
                    }
                }else if(inst->GetOpcode()==BasicInstruction::LLVMIROpcode::STORE){
                    Operand ptr=((StoreInstruction*)inst)->GetPointer();
                    PtrInfo info=GetPtrInfo(ptr,cfg);
                    if(info.type==PtrInfo::types::Global||info.type==PtrInfo::types::Param){
                        rwinfo.AddWrite(info.root);
                    }
                }else if(inst->GetOpcode()==BasicInstruction::LLVMIROpcode::CALL){
                    auto func_name=((CallInstruction*)inst)->GetFunctionName();
                    if(!lib_function_names.count(func_name)){
                        auto func_def=llvmIR->FunctionNameTable[func_name];
                        auto son_cfg=llvmIR->llvm_cfg[func_def];
                        if(son_cfg!=cfg){//递归函数只记一次
                            ReCallGraph[son_cfg].callers[cfg]=(*(CallInstruction*)inst);//构建反向CallGraph
                            LeafFuncs.erase(cfg);//调用了其他函数的函数不能作为leaf
                        }
                        
                    }
                }
            }
        }
        rwmap[cfg]=rwinfo;
    }

    //【2】将子函数的读写信息汇聚到父函数中
    for(auto &cfg:LeafFuncs){
        GatherRWInfos(cfg);
    }
}

//将子函数的读写情况汇总到父函数中，作为call指令mod/ref的依据
void AliasAnalysisPass::GatherRWInfos(CFG*cfg){
    RWInfo rwinfo=rwmap[cfg];
    CallInfo* callinfo=&ReCallGraph[cfg];

    for(auto &[caller_cfg,callI]:callinfo->callers){
        RWInfo* caller_rwinfo=&rwmap[caller_cfg];
        auto arguments=callI.GetParameterList();
        //read   只要全局变量和函数参数中的PTR
        for(auto &rop:rwinfo.ReadRoots){
            if(rop->GetOperandType()==BasicOperand::GLOBAL){
                caller_rwinfo->AddRead(rop);
            }else if(rop->GetOperandType()==BasicOperand::REG){
                PtrInfo ptrinfo=GetPtrInfo(rop,cfg);
                if(ptrinfo.type==PtrInfo::types::Param){
                    assert(arguments[callinfo->param_order[rop]].first==BasicInstruction::LLVMType::PTR);
                    Operand arguOp=arguments[callinfo->param_order[rop]].second;//形参映射到实参

                    PtrInfo caller_ptrinfo=GetPtrInfo(arguOp,caller_cfg);//记录实参的root
                    caller_rwinfo->AddRead(caller_ptrinfo.root);
                }
            }
        }
        //write
        for(auto &wop:rwinfo.WriteRoots){
            if(wop->GetOperandType()==BasicOperand::GLOBAL){
                caller_rwinfo->AddWrite(wop);
            }else if(wop->GetOperandType()==BasicOperand::REG){
                PtrInfo ptrinfo=GetPtrInfo(wop,cfg);
                if(ptrinfo.type==PtrInfo::types::Param){
                    assert(arguments[callinfo->param_order[wop]].first==BasicInstruction::LLVMType::PTR);
                    Operand arguOp=arguments[callinfo->param_order[wop]].second;//形参映射到实参

                    PtrInfo caller_ptrinfo=GetPtrInfo(arguOp,caller_cfg);//记录实参的root
                    caller_rwinfo->AddWrite(caller_ptrinfo.root);
                }
            }
        }

        //沿调用链反向传播汇总
        GatherRWInfos(caller_cfg);
    }
    return ;
}



//寻找phi传递的ptr
void AliasAnalysisPass::FindPhi(){
    for(auto &[defI,cfg]:llvmIR->llvm_cfg){
        for(auto &[id,block]:*(cfg->block_map)){
            for(auto &inst:block->Instruction_list){
                if(inst->GetOpcode()==BasicInstruction::LLVMIROpcode::PHI){
                    auto phi=(PhiInstruction*)inst;
                    if(phi->GetResultType()==BasicInstruction::LLVMType::PTR){
                        std::cout<<" phi in function "<<defI->GetFunctionName()<<std::endl;
                        return ;
                    }

                }
            }
        }
    }
    std::cout<<"No Phi"<<std::endl;
    return ;
}


void AliasAnalysisPass::Execute(){
    PtrPropagationAnalysis();
    RWInfoAnalysis();

    // test
    // PrintAAResult();
    // Test();
}

/* -------------------------------  TEST ----------------------------------*/

std::string sour[3]={"Undef","Gep","Phi"};
std::string tys[4]={"Undefed","Global","Param","Local"};
std::string alias_status[2]={"NoAlias","MustAlias"};
std::string modref_status[4]={"NoModRef","Ref","Mod","ModRef"};

void PtrInfo::PrintDebugInfo(){
    std::cout<<"source: "<<sour[source]<<" type: "<<tys[type]<<" root: "<<root->GetFullName() <<std::endl;
    std::cout<<"Aliases: "<<std::endl;
    for(auto& op:AliasOps){
        std::cout<<"         "<<op->GetFullName()<<std::endl;
    }

}


void AliasAnalysisPass::PrintAAResult() {

    std::cout<<" ----------- CallInfo ------------- "<<std::endl;
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        defI->PrintIR(std::cout);
        auto callinfo=ReCallGraph[cfg];

        std::cout<<"para_order:  "<<std::endl;
        for(auto&[op,order]:callinfo.param_order){
            std::cout<<"Operand: "<<op->GetFullName()<<" order: "<<std::to_string(order)<<std::endl;
        }

        std::cout<<"callers:  "<<std::endl;
        for(auto&[caller,xx]:callinfo.callers){
            std::cout<<caller->function_def->GetFunctionName()<<"    ";
        }std::cout<<std::endl;
        std::cout<<"-------------" <<std::endl;

    }

    std::cout<<" ----------- PtrInfo ------------- "<<std::endl;
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        defI->PrintIR(std::cout);
        for (auto [regno, info] : ptrmap[cfg]) {
            std::cout << "REG: %r" << regno << "\n";
            info.PrintDebugInfo();
            std::cout<<"-----------"<<std::endl;
        }
    }

    std::cout<<" ----------- RWInfo -------------- "<<std::endl;
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        defI->PrintIR(std::cout);
        auto cfgrwinfo = rwmap[cfg];

        std::cout << "read :   ";
        for (auto op : cfgrwinfo.ReadRoots) {
            std::cout << op << " ";
        }
        std::cout << "\n";

        std::cout << "write :   ";
        for (auto op : cfgrwinfo.WriteRoots) {
            std::cout << op << " ";
        }
        std::cout << "\n" <<"\n";
    }

    std::cout<<" ----------- Leafs -------------- "<<std::endl;
    for(auto cfg:LeafFuncs){
        std::cout << cfg->function_def->GetFunctionName()<<std::endl;
    }
}


void AliasAnalysisPass::Test() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {

        if(defI->GetFunctionName()!="insert"){
            continue;
        }

        defI->PrintIR(std::cout);

        std::cout<<" ----------- AliasResult -------------- "<<std::endl;
        std::set<Operand> ptrset;
        for (auto [id, bb] : *(cfg->block_map)) {
            for (auto I : bb->Instruction_list) {
                if (I->GetOpcode() == BasicInstruction::LLVMIROpcode::GETELEMENTPTR) {
                    ptrset.insert(I->GetResult());
                }
                // } else if (I->GetOpcode() == BasicInstruction::LLVMIROpcode::LOAD) {
                //     ptrset.insert(((LoadInstruction *)I)->GetPointer());
                // } else if (I->GetOpcode() == BasicInstruction::LLVMIROpcode::STORE) {
                //     ptrset.insert(((StoreInstruction *)I)->GetPointer());
                // }
            }
        }
        for (auto ptr1 : ptrset) {
            for (auto ptr2 : ptrset) {
                auto res=QueryAlias(ptr1, ptr2, cfg);
                if(res==MustAlias){
                    std::cout << ptr1->GetFullName() << " " << ptr2->GetFullName() << " " << alias_status[res] << "\n";
                }
                
            }
        }

        std::cout<<" ----------- ModRefResult -------------- "<<std::endl;
        for (auto ptr : ptrset) {
            PtrInfo ptrinfo=GetPtrInfo(ptr,cfg);
            if(ptrinfo.source==PtrInfo::sources::Undef&&ptrinfo.type!=PtrInfo::types::Global){
                continue;
            }
            for (auto [id, bb] : *(cfg->block_map)) {
                for (auto I : bb->Instruction_list) {
                    if (I->GetOpcode() == BasicInstruction::LLVMIROpcode::CALL) {
                        if(((CallInstruction*)I)->GetFunctionName()=="radixSort"){
                            I->PrintIR(std::cout);
                            std::cout << ptr->GetFullName() << " " << modref_status[QueryInstModRef(I, ptr, cfg)] << "\n";
                        }
                        if(!lib_function_names.count(((CallInstruction*)I)->GetFunctionName())){
                            I->PrintIR(std::cout);
                            std::cout << ptr->GetFullName() << " " << modref_status[QueryInstModRef(I, ptr, cfg)] << "\n";
                        }


                    } else if (I->GetOpcode() == BasicInstruction::LLVMIROpcode::LOAD) {
                        I->PrintIR(std::cout);
                        std::cout << ptr->GetFullName() << " " << modref_status[QueryInstModRef(I, ptr, cfg)] << "\n";
                    } else if (I->GetOpcode() == BasicInstruction::LLVMIROpcode::STORE) {
                        I->PrintIR(std::cout);
                        std::cout << ptr->GetFullName() << " " << modref_status[QueryInstModRef(I, ptr, cfg)] << "\n";
                    }
                }
            }
        }
    }
}   