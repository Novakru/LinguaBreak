// ASTNode 派生类的 TypeCheck() 实现
#include "../../include/ast.h"
#include "semant.h"
//#include "../include/ir.h"
#include "../include/type.h"
//#include "../include/Instruction.h"
#include<map>
// /*
//     语义分析阶段需要对语法树节点上的类型和常量等信息进行标注, 即NodeAttribute类
//     同时还需要标注每个变量的作用域信息，即部分语法树节点中的scope变量
//     你可以在utils/ast_out.cc的输出函数中找到你需要关注哪些语法树节点中的NodeAttribute类及其他变量
//     以及对语义错误的代码输出报错信息
// */

// /*
//     错误检查的基本要求:
//     • 检查 main 函数是否存在 (根据SysY定义，如果不存在main函数应当报错)；
//     • 检查未声明变量，及在同一作用域下重复声明的变量；
//     • 条件判断和运算表达式：int 和 bool 隐式类型转换（例如int a=5，return a+!a）；
//     • 数值运算表达式：运算数类型是否正确 (如返回值为 void 的函数调用结果是否参与了其他表达式的计算)；
//     • 检查未声明函数，及函数形参是否与实参类型及数目匹配；
//     • 检查是否存在整型变量除以整型常量0的情况 (如对于表达式a/(5-4-1)，编译器应当给出警告或者直接报错)；

//     错误检查的进阶要求:
//     • 对数组维度进行相应的类型检查 (例如维度是否有浮点数，定义维度时是否为常量等)；
//     • 对float进行隐式类型转换以及其他float相关的检查 (例如模运算中是否有浮点类型变量等)；
// */
// //改写二维数组

//extern LLVMIR llvmIR;
extern std::map<std::pair<BuiltinType::BuiltinKind, BuiltinType::BuiltinKind>, NodeAttribute (*)(NodeAttribute, NodeAttribute, OpType::Op)> SemantBinaryNodeMap;
extern std::map<BuiltinType::BuiltinKind, NodeAttribute (*)(NodeAttribute, OpType::Op)> SemantSingleNodeMap;
extern NodeAttribute SemantError(NodeAttribute a, OpType::Op opcode);
SemantTable semant_table;
std::vector<std::string> error_msgs{}; // 将语义错误信息保存到该变量中
static int GlobalVarCnt = 0;
static int GlobalStrCnt = 0;
//std::map<std::string, VarAttribute> ConstGlobalMap;
//std::map<std::string, VarAttribute> StaticGlobalMap;
static bool MainTag = 0;
static int InWhileCount = 0;
//enum LLVMType { I32 = 1, FLOAT32 = 2, PTR = 3, VOID = 4, I8 = 5, I1 = 6, I64 = 7, DOUBLE = 8 };
//BasicInstruction:: LLVMType Type2LLvm[6] = {BasicInstruction::LLVMType::VOID, BasicInstruction::LLVMType::I32, BasicInstruction::LLVMType::FLOAT32,
 //                        BasicInstruction::LLVMType::I1,   BasicInstruction::LLVMType::PTR, BasicInstruction::LLVMType::DOUBLE};



void __Program::TypeCheck() {
    semant_table.symbol_table.beginScope();
    auto comp_vector = *comp_list;
    for (auto comp : comp_vector) {
        comp->TypeCheck();
    }
    //main函数的错误处理
    if (!MainTag) {
        error_msgs.push_back("There is no main function.\n");
    }
}


void Exp::TypeCheck() 
{
    addExp->TypeCheck();

    attribute = addExp->attribute;
}
void ConstExp::TypeCheck() 
{
    addExp->TypeCheck();
    attribute = addExp->attribute;
    if (!attribute.ConstTag) {    // addexp is not const
        error_msgs.push_back("Expression is not const " + std::to_string(addExp->line) + "\n");
    }
}
void AddExp::TypeCheck() 
{
    addExp->TypeCheck();
    mulExp->TypeCheck();
    auto key = std::make_pair(addExp->attribute.type->builtinKind, mulExp->attribute.type->builtinKind);
    auto it = SemantBinaryNodeMap.find(key);
    if (it != SemantBinaryNodeMap.end()) {
        if(op.optype==OpType::Add)
        {
            attribute = it->second(addExp->attribute, mulExp->attribute, OpType::Add);
        }
        else
        {
            attribute = it->second(addExp->attribute, mulExp->attribute, OpType::Sub);
        }
       
    } else
    {
        error_msgs.push_back("invalid operators in line " + std::to_string(addExp->attribute.line_number) + "\n");
    }
}
void MulExp::TypeCheck() 
{
    mulExp->TypeCheck();
    unaryExp->TypeCheck();
    auto key = std::make_pair(mulExp->attribute.type->builtinKind,unaryExp->attribute.type->builtinKind);
    auto it = SemantBinaryNodeMap.find(key);
    if (it != SemantBinaryNodeMap.end()) {
        if(op.optype==OpType::Mul)
        {
            attribute = it->second(mulExp->attribute, unaryExp->attribute, OpType::Mul);
        }
        else if(op.optype==OpType::Div)
        {
            attribute = it->second(mulExp->attribute, unaryExp->attribute, OpType::Div);
        }
        else
        {
            attribute = it->second(mulExp->attribute, unaryExp->attribute, OpType::Mod);
        }
        
    } else
    {
         error_msgs.push_back("invalid operators in line " + std::to_string(mulExp->attribute.line_number) + "\n");
    }
}
void RelExp::TypeCheck() 
{
    relExp->TypeCheck();
    addExp->TypeCheck();
    auto key = std::make_pair(relExp->attribute.type->builtinKind,addExp->attribute.type->builtinKind);
    auto it = SemantBinaryNodeMap.find(key);
    if (it != SemantBinaryNodeMap.end()) {
        if(op.optype==OpType::Le)
        {
            attribute = it->second(relExp->attribute, addExp->attribute, OpType::Le);
        }
        else if(op.optype==OpType::Lt)
        {
            attribute = it->second(relExp->attribute, addExp->attribute, OpType::Lt);
        }
        else if(op.optype==OpType::Ge)
        {
            attribute = it->second(relExp->attribute, addExp->attribute, OpType::Ge);
        }
        else if(op.optype==OpType::Gt)
        {
            attribute = it->second(relExp->attribute, addExp->attribute, OpType::Gt);
        }
       
    } else
    {
         error_msgs.push_back("invalid operators in line " + std::to_string(relExp->attribute.line_number) + "\n");
    }
}
void EqExp::TypeCheck() 
{
    eqExp->TypeCheck();
    relExp->TypeCheck();
    auto key = std::make_pair(eqExp->attribute.type->builtinKind,relExp->attribute.type->builtinKind);
    auto it = SemantBinaryNodeMap.find(key);
    if (it != SemantBinaryNodeMap.end()) {
        if(op.optype==OpType::Eq)
        {
            attribute = it->second(eqExp->attribute, relExp->attribute,OpType::Eq);
        }
        else
        {
            attribute = it->second(eqExp->attribute, relExp->attribute,OpType::Neq);
        }
        
    } else
    {
          error_msgs.push_back("invalid operators in line " + std::to_string(eqExp->attribute.line_number) + "\n");
    }
}
void LAndExp::TypeCheck() 
{
    lAndExp->TypeCheck();
    eqExp->TypeCheck();
    auto key = std::make_pair(lAndExp->attribute.type->builtinKind,eqExp->attribute.type->builtinKind);
    auto it = SemantBinaryNodeMap.find(key);
    if (it != SemantBinaryNodeMap.end()) {
        attribute = it->second(lAndExp->attribute, eqExp->attribute, OpType::And);
    } else
    {
         error_msgs.push_back("invalid operators in line " + std::to_string(lAndExp->attribute.line_number) + "\n");
    }
}
void LOrExp::TypeCheck() 
{
    lOrExp->TypeCheck();
    lAndExp->TypeCheck();
    auto key = std::make_pair(lOrExp->attribute.type->builtinKind,lAndExp->attribute.type->builtinKind);
    auto it = SemantBinaryNodeMap.find(key);
    if (it != SemantBinaryNodeMap.end()) {
        attribute = it->second(lOrExp->attribute, lAndExp->attribute, OpType::Or);
    } else
    {
          error_msgs.push_back("invalid operators in line " + std::to_string(lOrExp->attribute.line_number) + "\n");
    }
}
void Lval::TypeCheck() 
{

}
void FuncRParams::TypeCheck() {}
void FuncCall::TypeCheck() {}

void UnaryExp::TypeCheck() 
{
    unaryExp->TypeCheck();
    if(unaryOp.optype==OpType::Add)
    {
        attribute=SemantSingleNodeMap[unaryExp->attribute.type->builtinKind]
        (unaryExp->attribute,OpType::Add);
    }
    else if(unaryOp.optype==OpType::Sub)
    {
        attribute=SemantSingleNodeMap[unaryExp->attribute.type->builtinKind]
        (unaryExp->attribute,OpType::Sub);
    }
    else
    {
        attribute=SemantSingleNodeMap[unaryExp->attribute.type->builtinKind]
        (unaryExp->attribute,OpType::Not);
    }
    
}
void IntConst::TypeCheck() 
{
    attribute.type = new BuiltinType(BuiltinType::Int);
    attribute.ConstTag = true;
    attribute.val.IntVal = val;
}
void FloatConst::TypeCheck() 
{
    attribute.type = new BuiltinType(BuiltinType::Float);
    attribute.ConstTag = true;
    attribute.val.FloatVal = val;
}
void PrimaryExp::TypeCheck() 
{
    exp->TypeCheck();
    attribute = exp->attribute;
}
void AssignStmt::TypeCheck() {}
void ExprStmt::TypeCheck() {}
void BlockStmt::TypeCheck() {}
void IfStmt::TypeCheck() {}
void WhileStmt::TypeCheck() {}
void ContinueStmt::TypeCheck() {}
void BreakStmt::TypeCheck() {}
void RetStmt::TypeCheck() {}
void ConstInitValList::TypeCheck() {}
void ConstInitVal::TypeCheck() {}
void VarInitValList::TypeCheck() {}
void VarInitVal::TypeCheck() {}
void VarDef_no_init::TypeCheck() {}
void VarDef::TypeCheck() {}
void ConstDef::TypeCheck() {}
void VarDecl::TypeCheck() {}
void ConstDecl::TypeCheck() {}
void BlockItem_Decl::TypeCheck() {}
void BlockItem_Stmt::TypeCheck() {}
void __Block::TypeCheck() {}
void __FuncFParam::TypeCheck() {}
void __FuncDef::TypeCheck() {}
void CompUnit_Decl::TypeCheck() {}
void CompUnit_FuncDef::TypeCheck() {}
