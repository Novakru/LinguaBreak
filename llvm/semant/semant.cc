// ASTNode 派生类的 TypeCheck() 实现
#include "../../include/ast.h"
#include "semant.h"
//#include "../include/ir.h"
#include "../include/type.h"
//#include "../include/Instruction.h"
#include<map>


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
        attribute = it->second(addExp->attribute, mulExp->attribute,op.optype);
       
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
        attribute = it->second(mulExp->attribute, unaryExp->attribute, op.optype);
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
        attribute = it->second(relExp->attribute, addExp->attribute, op.optype);
        
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
        attribute = it->second(eqExp->attribute, relExp->attribute,op.optype);
        
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
    attribute=SemantSingleNodeMap[unaryExp->attribute.type->builtinKind]
        (unaryExp->attribute,unaryOp.optype);
    
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
