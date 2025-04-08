#ifndef SYSY_TREE_H
#define SYSY_TREE_H

// AST definition
#include "symbol.h"
#include "type.h"
// #include "visitor.h" I think visitor patten is no use
#include <iostream>
#include <vector>
#include <variant>

class ASTNode {
public:
    int line; 

    virtual ~ASTNode() = default;
	virtual void codeIR() = 0;
    virtual void TypeCheck() = 0; 
    virtual void printAST(std::ostream &s, int pad) = 0;
};

/* basic class of expression */
class _ExprBase : public ASTNode {
public:
	virtual void codeIR() {}
    virtual void TypeCheck() {}
    virtual void printAST(std::ostream &s, int pad) {}
};
typedef _ExprBase *ExprBasePtr;

// Exp → AddExp
class Exp : public _ExprBase {
public:
	ExprBasePtr addExp;
	Exp(ExprBasePtr addExp) : addExp(addExp) {};
	
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// AddExp → MulExp | AddExp ('+' | '−') MulExp 
class Add2MulExp : public _ExprBase {
public:
	ExprBasePtr mulExp;

	// select Production 1.
    Add2MulExp(ExprBasePtr mulExp) : mulExp(mulExp) {}
	
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};


class BinaryAddExp : public _ExprBase {
public:
	ExprBasePtr addExp, mulExp;
	OpType op;

	// select Production 2.
    BinaryAddExp(ExprBasePtr addExp, OpType op, ExprBasePtr mulExp) : addExp(addExp), op(op), mulExp(mulExp) {}
	
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp 
class Mul2UnaryExp : public _ExprBase {
public:
    ExprBasePtr unaryExp;

    // select Production 1.
    Mul2UnaryExp(ExprBasePtr unaryExp) : unaryExp(unaryExp) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

class BinaryMulExp : public _ExprBase {
public:
    ExprBasePtr mulExp, unaryExp;
    OpType op;

    // select Production 2.
    BinaryMulExp(ExprBasePtr mulExp, OpType op, ExprBasePtr unaryExp) : mulExp(mulExp), op(op), unaryExp(unaryExp) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// RelExp → AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp
class Rel2AddExp : public _ExprBase {
public:
    ExprBasePtr addExp;

    // select Production 1.
    Rel2AddExp(ExprBasePtr addExp) : addExp(addExp) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

class BinaryRelExp : public _ExprBase {
public:
    ExprBasePtr relExp, addExp;
    OpType op;

    // select Production 2.
    BinaryRelExp(ExprBasePtr relExp, OpType op, ExprBasePtr addExp) : relExp(relExp), op(op), addExp(addExp) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};


// LAndExp → EqExp | LAndExp '&&' EqExp
class LAndExp : public _ExprBase {
public:
	ExprBasePtr lAndExp, eqExp;
	OpType op;

	// select Production 1.
    LAndExp(ExprBasePtr eqExp) : lAndExp(NULL), op(OpType::Void), eqExp(eqExp) {}
	// select Production 2.
    LAndExp(ExprBasePtr lAndExp, OpType op, ExprBasePtr eqExp) : lAndExp(lAndExp), op(op), eqExp(eqExp) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// LOrExp → LAndExp | LOrExp '||' LAndExp
class LAnd2EqExp : public _ExprBase {
public:
    ExprBasePtr eqExp;

    // select Production 1.
    LAnd2EqExp(ExprBasePtr eqExp) : eqExp(eqExp) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

class BinaryLAndExp : public _ExprBase {
public:
    ExprBasePtr lAndExp, eqExp;
    OpType op;

    // select Production 2.
    BinaryLAndExp(ExprBasePtr lAndExp, OpType op, ExprBasePtr eqExp) : lAndExp(lAndExp), op(op), eqExp(eqExp) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};


// ConstExp → AddExp
class ConstExp : public _ExprBase {
public:
	ExprBasePtr addExp;
    ConstExp(ExprBasePtr addExp) : addExp(addExp) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// LVal → Ident {'[' Exp ']'}
class Lval : public _ExprBase {
public:
    Symbol name;
    std::vector<ExprBasePtr> *dims;  // may be empty

    Lval(Symbol n, std::vector<ExprBasePtr> *d) : name(n), dims(d) {}
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// FuncRParams → Exp { ',' Exp }
class FuncRParams : public _ExprBase {
public:
    std::vector<ExprBasePtr> *rParams{};
    FuncRParams(std::vector<ExprBasePtr> *p) : rParams(p) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// UnaryExp → Ident '(' [FuncRParams] ')'
class FuncCall : public _ExprBase {
public:
    Symbol name;
    ExprBasePtr funcRParams;
    
	FuncCall(Symbol n, ExprBasePtr f) : name(n), funcRParams(f) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// UnaryExp →  UnaryOp UnaryExp 
// UnaryOp  → '+' | '−' | '!' 
class UnaryExp : public _ExprBase {
public:
	ExprBasePtr unaryExp;
	OpType unaryOp;

    UnaryExp(OpType unaryOp, ExprBasePtr unaryExp) : unaryExp(unaryExp), unaryOp(unaryOp) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// Number → IntConst | floatConst 
// Literal → IntConst | floatConst | StrConst
class Literal : public _ExprBase {
public:
    BuiltinType type;
    std::variant<int, float, std::string> val;

    Literal(int value) : type(BuiltinType::Int), val(value) {}
    Literal(float value) : type(BuiltinType::Float), val(value) {}
    Literal(const std::string& value) : type(BuiltinType::String), val(value) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// PrimaryExp → '(' Exp ')' | LVal | Number
class PrimaryExp : public _ExprBase {
public:
	ExprBasePtr exp;
    PrimaryExp(ExprBasePtr exp) : exp(exp) {}
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

class _Block;
typedef _Block *Block;

class _Stmt : public ASTNode {};
typedef _Stmt *Stmt;

// Stmt → LVal '=' Exp ';' 
class AssignStmt : public _Stmt {
public:
    ExprBasePtr lval;
    ExprBasePtr exp;

    AssignStmt(ExprBasePtr l, ExprBasePtr e) : lval(l), exp(e) {}
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// Stmt → [Exp] ';' 
class ExprStmt : public _Stmt {
public:
    ExprBasePtr exp;  // may be empty

	bool getExp() { return exp; }
    ExprStmt(ExprBasePtr e) : exp(e) {}
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// Stmt → Block
class BlockStmt : public _Stmt {
public:
    Block b;
    BlockStmt(Block b) : b(b) {}
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// Stmt → 'if' '( Cond ')' Stmt  ['else' Stmt] 
class IfStmt : public _Stmt {
public:
    ExprBasePtr Cond;
    Stmt ifStmt;
    Stmt elseStmt;   // may be empty

	Stmt getElseStmt() { return elseStmt; }
    IfStmt(ExprBasePtr c, Stmt i, Stmt t) : Cond(c), ifStmt(i), elseStmt(t) {}
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// Stmt → 'while' '(' Cond ')' Stmt
class WhileStmt : public _Stmt {
public:
    ExprBasePtr Cond;
    Stmt loopBody;

    WhileStmt(ExprBasePtr c, Stmt b) : Cond(c), loopBody(b) {}
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// Stmt → 'continue' ';'
class ContinueStmt : public _Stmt {
public:
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// Stmt → 'break' ';'
class BreakStmt : public _Stmt {
public:
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// Stmt → 'return' [Exp] 
class RetStmt : public _Stmt {
public:
    ExprBasePtr retExp; // may be empty
    RetStmt(ExprBasePtr r) : retExp(r) {}

	ExprBasePtr getExp() { return retExp; }
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

class _DeclBase;
typedef _DeclBase *DeclBase;

class _InitValBase : public ASTNode {
public:
	virtual ExprBasePtr getExp() = 0;
};
typedef _InitValBase *InitValBase;

// ConstInitVal → ConstExp  
//				 | '{' [ ConstInitVal { ',' ConstInitVal } ] '}' 
class ConstInitValList : public _InitValBase {
public:
    std::vector<InitValBase> *initval;
    ConstInitValList(std::vector<InitValBase> *i) : initval(i) {}
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
	ExprBasePtr getExp() {return nullptr;}
};

class ConstInitVal2Exp : public _InitValBase {
public:
    ExprBasePtr exp;

    ConstInitVal2Exp(ExprBasePtr e) : exp(e) {}
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
	ExprBasePtr getExp() {return exp;}
};

// InitVal → Exp | '{' [ InitVal { ',' InitVal } ] '}' 
class InitValList : public _InitValBase {
public:
    std::vector<InitValBase> *initval;

    InitValList(std::vector<InitValBase> *i) : initval(i) {}
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
	ExprBasePtr getExp() {return nullptr;}
};

class InitVal2Exp : public _InitValBase { // Multiple Inheritance
public:
    ExprBasePtr exp;

    InitVal2Exp(ExprBasePtr e) : exp(e) {}
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
	ExprBasePtr getExp() {return exp;}
};

class _Def : public ASTNode {
public:
    int scope = -1;   
};
typedef _Def *Def;

class VarDef_no_init : public _Def {
public:
    Symbol name;
    std::vector<ExprBasePtr> *dims;
    // 如果dims为nullptr, 表示该变量不含数组下标, 你也可以通过其他方式判断，但需要修改SysY_parser.y已有的代码
    VarDef_no_init(Symbol n, std::vector<ExprBasePtr> *d) : name(n), dims(d) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

class VarDef : public _Def {
public:
    Symbol name;
    std::vector<ExprBasePtr> *dims;
    InitValBase init;
    VarDef(Symbol n, std::vector<ExprBasePtr> *d, InitValBase i) : name(n), dims(d), init(i) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

class ConstDef : public _Def {
public:
    Symbol name;
    std::vector<ExprBasePtr> *dims;

    InitValBase init;
    ConstDef(Symbol n, std::vector<ExprBasePtr> *d, InitValBase i) : name(n), dims(d), init(i) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// decl basic_class
class _DeclBase : public ASTNode {
public:
};

// var definition
class VarDecl : public _DeclBase {
public:
    Type type_decl;
    std::vector<Def> *var_def_list{};
    // construction
    VarDecl(Type t, std::vector<Def> *v) : type_decl(t), var_def_list(v) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// const var definition
class ConstDecl : public _DeclBase {
public:
    Type type_decl;
    std::vector<Def> *var_def_list{};
    // construction
    ConstDecl(Type t, std::vector<Def> *v) : type_decl(t), var_def_list(v) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

class _BlockItem : public ASTNode {
public:
};
typedef _BlockItem *BlockItem;

class BlockItem_Decl : public _BlockItem {
public:
    DeclBase decl;
    BlockItem_Decl(DeclBase d) : decl(d) {}
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

class BlockItem_Stmt : public _BlockItem {
public:
    Stmt stmt;
    BlockItem_Stmt(Stmt s) : stmt(s) {}
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// block
class _Block : public ASTNode {
public:
    std::vector<BlockItem> *item_list{};
    // construction
    _Block() {}
    _Block(std::vector<BlockItem> *i) : item_list(i) {}
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// FuncParam -> Type IDENT
// FuncParam -> Type IDENT [] {[Exp]}
class __FuncFParam : public ASTNode {
public:
    Type type_decl;
    std::vector<ExprBasePtr> *dims;
    // 如果dims为nullptr, 表示该变量不含数组下标, 你也可以通过其他方式判断，但需要修改SysY_parser.y已有的代码
    Symbol name;
    int scope = -1;    // 在语义分析阶段填入正确的作用域

    __FuncFParam(Type t, Symbol n, std::vector<ExprBasePtr> *d) {
        type_decl = t;
        name = n;
        dims = d;
    }
	// 函数声明时省略形参 int foo(int [][3]); 
    __FuncFParam(Type t, std::vector<ExprBasePtr> *d) {
        type_decl = t;
        dims = d;
    }
	// int foo(int, int);
    __FuncFParam(Type t) : type_decl(t) {}
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};
typedef __FuncFParam *FuncFParam;

// return_type name '(' [formals] ')' block
class __FuncDef : public ASTNode {
public:
    Type return_type;
    Symbol name;
    std::vector<FuncFParam> *formals;
    Block block;
    __FuncDef(Type t, Symbol functionName, std::vector<FuncFParam> *f, Block b) {
        formals = f;
        name = functionName;
        return_type = t;
        block = b;
    }
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};
typedef __FuncDef *FuncDef;

class __CompUnit : public ASTNode {
public:
};
typedef __CompUnit *CompUnit;

class CompUnit_Decl : public __CompUnit {
public:
    DeclBase decl;
    CompUnit_Decl(DeclBase d) : decl(d) {}
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

class CompUnit_FuncDef : public __CompUnit {
public:
    FuncDef func_def;
    CompUnit_FuncDef(FuncDef f) : func_def(f) {}
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

class __Program : public ASTNode {
public:
    std::vector<CompUnit> *comp_list;

    __Program(std::vector<CompUnit> *c) { comp_list = c; }
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};
typedef __Program *Program;

#endif