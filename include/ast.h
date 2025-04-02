#ifndef SYSY_TREE_H
#define SYSY_TREE_H

// AST definition
#include "symbol.h"
#include "type.h"
// #include "visitor.h" I think visitor patten is no use
#include <iostream>
#include <vector>

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
typedef _ExprBase *ExprBase;

// Exp → AddExp
class Exp : public _ExprBase {
public:
	ExprBase addExp;
	Exp(ExprBase addExp) : addExp(addExp) {};
	
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};
typedef _ExprBase* ExprBase;

// AddExp → MulExp | AddExp ('+' | '−') MulExp 
class AddExp : public _ExprBase {
public:
	ExprBase addExp, mulExp;
	OpType op;

	// select Production 1.
    AddExp(ExprBase mulExp) : addExp(NULL), op(OpType::Void), mulExp(mulExp) {}
	// select Production 2.
    AddExp(ExprBase addExp, OpType op, ExprBase mulExp) : addExp(addExp), op(op), mulExp(mulExp) {}
	
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp 
class MulExp : public _ExprBase {
public:
	ExprBase mulExp, unaryExp;
	OpType op;

	// select Production 1.
    MulExp(ExprBase unaryExp) : mulExp(NULL), op(OpType::Void), unaryExp(unaryExp) {}
	// select Production 2.
    MulExp(ExprBase mulExp, OpType op, ExprBase unaryExp) : mulExp(mulExp), op(op), unaryExp(unaryExp) {}
	
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// RelExp → AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp
class RelExp : public _ExprBase {
public:
	ExprBase relExp, addExp;
	OpType op;

	// select Production 1.
    RelExp(ExprBase addExp) : relExp(NULL), op(OpType::Void), addExp(addExp) {}
	// select Production 2.
    RelExp(ExprBase relExp, OpType op, ExprBase addExp) : relExp(relExp), op(op), addExp(addExp) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// LAndExp → EqExp | LAndExp '&&' EqExp
class LAndExp : public _ExprBase {
public:
	ExprBase lAndExp, eqExp;
	OpType op;

	// select Production 1.
    LAndExp(ExprBase eqExp) : lAndExp(NULL), op(OpType::Void), eqExp(eqExp) {}
	// select Production 2.
    LAndExp(ExprBase lAndExp, OpType op, ExprBase eqExp) : lAndExp(lAndExp), op(op), eqExp(eqExp) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// LOrExp → LAndExp | LOrExp '||' LAndExp
class LOrExp : public _ExprBase {
public:
	ExprBase lOrExp, lAndExp;
	OpType op;

	// select Production 1.
    LOrExp(ExprBase lAndExp) : lOrExp(NULL), op(OpType::Void), lAndExp(lAndExp) {}
	// select Production 2.
    LOrExp(ExprBase lOrExp, OpType op, ExprBase lAndExp) : lOrExp(lOrExp), op(op), lAndExp(lAndExp) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// ConstExp → AddExp
class ConstExp : public _ExprBase {
public:
	ExprBase addExp;
    ConstExp(ExprBase addExp) : addExp(addExp) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// LVal → Ident {'[' Exp ']'}
class Lval : public _ExprBase {
public:
    Symbol name;
    std::vector<ExprBase> *dims;  // may be empty

    Lval(Symbol n, std::vector<ExprBase> *d) : name(n), dims(d) {}
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// FuncRParams → Exp { ',' Exp }
class FuncRParams : public _ExprBase {
public:
    std::vector<ExprBase> *rParams{};
    FuncRParams(std::vector<ExprBase> *p) : rParams(p) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// UnaryExp → Ident '(' [FuncRParams] ')'
class FuncCall : public _ExprBase {
public:
    Symbol name;
    ExprBase funcRParams;
    
	FuncCall(Symbol n, ExprBase f) : name(n), funcRParams(f) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// UnaryExp →  UnaryOp UnaryExp 
// UnaryOp  → '+' | '−' | '!' 
class UnaryExp : public _ExprBase {
public:
	ExprBase unaryExp;
	OpType unaryOp;

    UnaryExp(OpType unaryOp, ExprBase unaryExp) : unaryExp(unaryExp), unaryOp(unaryOp) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

class IntConst : public _ExprBase {
public:
    int val;
    IntConst(int v) : val(v) {}
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

class FloatConst : public _ExprBase {
public:
    float val;
    FloatConst(float v) : val(v) {}
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// PrimaryExp → '(' Exp ')' | LVal | Number
class PrimaryExp : public _ExprBase {
public:
	ExprBase exp;
    PrimaryExp(ExprBase exp) : exp(exp) {}
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
    ExprBase lval;
    ExprBase exp;

    AssignStmt(ExprBase l, ExprBase e) : lval(l), exp(e) {}
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// Stmt → [Exp] ';' 
class ExprStmt : public _Stmt {
public:
    ExprBase exp;  // may be empty

	bool getExp() { return exp; }
    ExprStmt(ExprBase e) : exp(e) {}
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
    ExprBase Cond;
    Stmt ifStmt;
    Stmt elseStmt;   // may be empty

	Stmt getElseStmt() { return elseStmt; }
    IfStmt(ExprBase c, Stmt i, Stmt t) : Cond(c), ifStmt(i), elseStmt(t) {}
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// Stmt → 'while' '(' Cond ')' Stmt
class WhileStmt : public _Stmt {
public:
    ExprBase Cond;
    Stmt loopBody;

    WhileStmt(ExprBase c, Stmt b) : Cond(c), loopBody(b) {}
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
    ExprBase retExp; // may be empty
    RetStmt(ExprBase r) : retExp(r) {}

	ExprBase getExp() { return retExp; }
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

class _DeclBase;
typedef _DeclBase *DeclBase;

class _InitValBase : public ASTNode {
public:
	virtual ExprBase getExp() = 0;
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
	ExprBase getExp() {return nullptr;}
};

// class ConstExp : public _InitValBase {
// public:
//     ExprBase exp;
//     ConstExp(ExprBase e) : exp(e) {}
//     void codeIR();
//     void TypeCheck();
//     void printAST(std::ostream &s, int pad);
// 	ExprBase getExp() {return exp;}
// };

// InitVal → Exp | '{' [ InitVal { ',' InitVal } ] '}' 
class InitValList : public _InitValBase {
public:
    std::vector<InitValBase> *initval;
    InitValList(std::vector<InitValBase> *i) : initval(i) {}
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
	ExprBase getExp() {return nullptr;}
};

// class Exp : public _InitValBase { // Multiple Inheritance
// public:
//     ExprBase exp;
//     Exp(ExprBase e) : exp(e) {}
//     void codeIR();
//     void TypeCheck();
//     void printAST(std::ostream &s, int pad);
// 	ExprBase getExp() {return exp;}
// };


class __Def : public ASTNode {
public:
    int scope = -1;    // 在语义分析阶段填入正确的作用域
};
typedef __Def *Def;

class VarDef_no_init : public __Def {
public:
    Symbol name;
    std::vector<ExprBase> *dims;
    // 如果dims为nullptr, 表示该变量不含数组下标, 你也可以通过其他方式判断，但需要修改SysY_parser.y已有的代码
    VarDef_no_init(Symbol n, std::vector<ExprBase> *d) : name(n), dims(d) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

class VarDef : public __Def {
public:
    Symbol name;
    std::vector<ExprBase> *dims;
    InitValBase init;
    VarDef(Symbol n, std::vector<ExprBase> *d, InitValBase i) : name(n), dims(d), init(i) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

class ConstDef : public __Def {
public:
    Symbol name;
    std::vector<ExprBase> *dims;
    // 如果dims为nullptr, 表示该变量不含数组下标, 你也可以通过其他方式判断，但需要修改SysY_parser.y已有的代码
    InitValBase init;
    ConstDef(Symbol n, std::vector<ExprBase> *d, InitValBase i) : name(n), dims(d), init(i) {}

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
    Type::ty type_decl;
    std::vector<Def> *var_def_list{};
    // construction
    VarDecl(Type::ty t, std::vector<Def> *v) : type_decl(t), var_def_list(v) {}

    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};

// const var definition
class ConstDecl : public _DeclBase {
public:
    Type::ty type_decl;
    std::vector<Def> *var_def_list{};
    // construction
    ConstDecl(Type::ty t, std::vector<Def> *v) : type_decl(t), var_def_list(v) {}

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
    Type::ty type_decl;
    std::vector<ExprBase> *dims;
    // 如果dims为nullptr, 表示该变量不含数组下标, 你也可以通过其他方式判断，但需要修改SysY_parser.y已有的代码
    Symbol name;
    int scope = -1;    // 在语义分析阶段填入正确的作用域

    __FuncFParam(Type::ty t, Symbol n, std::vector<ExprBase> *d) {
        type_decl = t;
        name = n;
        dims = d;
    }
	// 函数声明时省略形参 int foo(int [][3]); 
    __FuncFParam(Type::ty t, std::vector<ExprBase> *d) {
        type_decl = t;
        dims = d;
    }
	// int foo(int, int);
    __FuncFParam(Type::ty t) : type_decl(t) {}
    void codeIR();
    void TypeCheck();
    void printAST(std::ostream &s, int pad);
};
typedef __FuncFParam *FuncFParam;

// return_type name '(' [formals] ')' block
class __FuncDef : public ASTNode {
public:
    Type::ty return_type;
    Symbol name;
    std::vector<FuncFParam> *formals;
    Block block;
    __FuncDef(Type::ty t, Symbol functionName, std::vector<FuncFParam> *f, Block b) {
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