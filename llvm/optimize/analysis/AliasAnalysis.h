#ifndef ALIASANALYSIS_H
#define ALIASANALYSIS_H
#include "../../include/ir.h"
#include "../pass.h"
#include "../lib_function_names.h"

/*
   README：
1. 此Pass依赖于Build_CFG时建立的def_map
2. 基础SysY中仅数组会用到Pointer，因此此AliasAnalysis仅考虑了Array的别名分析；若后续添加LoopParallel，会产生新的Pointer，需要进一步修缮
3. 调用方式：
    - 首先确保你查询的指针非nullptr
    - 查看两个ptr（RegOperand/GlobalOperand）是否别名冲突：
        if( QueryAlias(op1,op2, cfg) == MustAlias )...
    - 查看某指令(store/load/call)是否Mod/Ref某ptr：
        if( QueryInstModRef(inst,op,cfg) == Ref )...
4. 对于GEP指令Index为RegOperand的情况，我们无法直接判定为是否相同，统一认为存在别名冲突的风险，即MustAlias
5. 我们假定数组基址作为ptr不在分析范围之内
    即：
    %r1 = Alloc ...
    %r2 = getelementptr ... , ptr %r1, i32 0， i32 0, ...
    我们认为查询分析的Operand都是有实际意义的数组元素ptr，如%r2，而非数组声明时的起始地址%r1，尽管它们可能指向同一块内存
6. 对于call函数名称为lib_function_names中的指令，我们不做处理，并非是它们没有mod/ref，而是参数不会被查询到（见5）
*/
enum AliasStatus{ 
    NoAlias=0, 
    MustAlias=1 
};
enum ModRefStatus{ 
    NoModRef=0, 
    Ref=1, 
    Mod=2, 
    ModRef=3 
};

struct PtrInfo{
    enum sources{ Undef=0, Gep=1, Phi=2 }source;
    enum types{ Undefed=0, Global=1, Param=2, Local=3 }type;
    Operand root;//祖先指针，Global/Param/Local定义的数组基址
    std::unordered_set<Operand> AliasOps;//别名集，仅记录本身及其祖先

    PtrInfo(){ source=sources::Undef; type=types::Undefed; }
    PtrInfo(types ty, Operand op){ source=sources::Undef;  type=ty;  AliasOps.emplace(op);  }
    PtrInfo(sources so,types ty,  Operand op){ source=so;  type=ty;  AliasOps.emplace(op);  }
    void AddAliass(std::unordered_set<Operand> newOps){ AliasOps.insert(newOps.begin(),newOps.end());}

    void PrintDebugInfo();
};

struct RWInfo{
    std::unordered_set<Operand> ReadRoots;
    std::unordered_set<Operand> WriteRoots;

    RWInfo(){}
    void AddRead(Operand root){ ReadRoots.insert(root); }
    void AddWrite(Operand root){ WriteRoots.insert(root); }
};

struct CallInfo{
    std::unordered_map<Operand,int> param_order;//parameters ~ order in function_def_inst/call_inst
    std::unordered_map<CFG*,CallInstruction> callers;//caller ~ the call instruction
    CallInfo(){}
};

class AliasAnalysisPass : public IRPass { 
private:
    std::unordered_map<CFG*,std::unordered_map<int,PtrInfo>>ptrmap;//regno-->PtrInfo (only for RegOperand)
    std::unordered_map<CFG*,RWInfo>rwmap;//ref_operands,mod_operands
    std::unordered_map<CFG*,CallInfo>ReCallGraph;
    std::unordered_set<CFG*>LeafFuncs;

    void FindPhi();

    PtrInfo GetPtrInfo(Operand op, CFG* cfg);
    Operand CalleeParamToCallerArgu(Operand op, CFG* callee_cfg, CallInstruction* CallI);
    void RWInfoAnalysis();
    void GatherRWInfos(CFG*cfg);

    void GatherPtrInfos(CFG* cfg, int regno);
    void PtrPropagationAnalysis();
    bool IsSameArraySameConstIndex(GetElementptrInstruction* inst1,GetElementptrInstruction* inst2);
    
public:
    AliasAnalysisPass(LLVMIR *IR) : IRPass(IR) {}
    AliasStatus QueryAlias(Operand op1,Operand op2, CFG* cfg);
    ModRefStatus QueryInstModRef(Instruction inst, Operand op, CFG* cfg);
    void Execute();

    void Test();
    void PrintAAResult();

};

#endif
