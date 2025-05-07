#ifndef INST_SELECT_H
#define INST_SELECT_H
#include"../../basic/machine.h"
#include"../../../include/ir.h"
//参考原架构的MachineUnit，MachineSelector及其继承类;MachinePass与其中一个继承类RiscV64LowerFrame
//为了方便理解和调试，原来代码的类定义注释后附在文件后方

class MachineUnit {
public:
    // 指令选择时，对全局变量不作任何处理，直接保留到MachineUnit中
    //std::vector<Instruction> global_def{};//和llvmIR的一模一样，省略,直接使用IR->global_def即可
    std::vector<MachineFunction *> functions;
 
public:
    LLVMIR *IR;//指向中间代码阶段的信息
    MachineUnit(LLVMIR *IR) : IR(IR) {}
    virtual void SelectInstructionAndBuildCFG() = 0;
    virtual void LowerFrame() = 0;
    virtual void LowerStack() = 0;
};

class RiscV64Unit : public MachineUnit
{
public:
    void SelectInstructionAndBuildCFG();
    RiscV64Unit(LLVMIR *IR):MachineUnit(IR){}
    void LowerFrame();//替代原来的RiscV64LowerFrame(m_unit).Execute();
    void LowerStack();//替代原来的RiscV64LowerStack(m_unit).Execute();
    //template <class INSPTR> void ConvertAndAppend(INSPTR);//暂未实现,该类函数通过分析IR指令的操作数类型和语义，结合RISC-V指令集的特点，生成高效且符合ABI规范的机器码
};


// class MachineSelector {
//     // 指令选择基类
// protected:
//     MachineUnit *dest;
//     MachineFunction *cur_func;
//     MachineBlock *cur_block;
//     LLVMIR *IR;

// public:
//     MachineSelector(MachineUnit *dest, LLVMIR *IR) : dest(dest), IR(IR) {}
//     virtual void SelectInstructionAndBuildCFG() = 0;
//     MachineUnit *GetMachineUnit() { return dest; }
// };

// class RiscV64Selector : public MachineSelector {
// private:
//     int cur_offset;    // 局部变量在栈中的偏移
//     std::map<int, int> llvm_rv_allocas;
//     std::map<int, Register> llvm_rv_regtable;
//     std::map<Uint64, bool> global_imm_vsd;
//     std::map<Register, Instruction> cmp_context;
//     // 你需要保证在每个函数的指令选择结束后, cur_offset的值为局部变量所占栈空间的大小
    
//     // TODO(): 添加更多你需要的成员变量和函数
// public:
//     RiscV64Selector(MachineUnit *dest, LLVMIR *IR) : MachineSelector(dest, IR) {}
//     void SelectInstructionAndBuildCFG();
//     void ClearFunctionSelectState();
//     template <class INSPTR> void ConvertAndAppend(INSPTR);
//     Register GetllvmReg(int, MachineDataType);//新增
//     Register GetNewReg(MachineDataType, bool save_across_call = false);//新增
//     Register ExtractOp2Reg(BasicOperand *, MachineDataType);//新增
//     int ExtractOp2ImmI32(BasicOperand *);//新增
//     float ExtractOp2ImmF32(BasicOperand *);//新增
// };
// //保存全局变量与函数信息
// class MachineUnit {
// public:
//     // 指令选择时，对全局变量不作任何处理，直接保留到MachineUnit中
//     std::vector<Instruction> global_def{};

//     std::vector<MachineFunction *> functions;
// };

// class RiscV64Unit : public MachineUnit {};//可删除
// class RiscV64LowerFrame : public MachinePass {
// public:
//     RiscV64LowerFrame(MachineUnit *unit) : MachinePass(unit) {}
//     void Execute();
// };
// class MachinePass {
// protected:
//     MachineUnit *unit;
//     MachineFunction *current_func;
//     MachineBlock *cur_block;

// public:
//     virtual void Execute() = 0;
//     MachinePass(MachineUnit *unit) : unit(unit) {}
// };

// class RiscV64LowerStack : public MachinePass {
// public:
//     RiscV64LowerStack(MachineUnit *unit) : MachinePass(unit) {}
//     void Execute();

//     //新增
//     void InsertStackSetupInstructions(MachineBlock *block, int stackSize, bool restore_at_beginning, const std::unordered_set<int> &saveRegsSet,MachineFunction * func);
//     void InsertStackTeardownInstructions(MachineBlock *block, int stackSize, bool restore_at_beginning, const std::unordered_set<int> &saveRegsSet);
// };
#endif