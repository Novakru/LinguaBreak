%{
#include <fstream>
#include "../include/ast.h"
#include "../include/symbol.h"
#include "../include/type.h"

int yylex();
void yyerror(char *s, ...);
int error_num = 0;
extern int line;
// extern std::ofstream fout;
%}
// Token-Type token(value)
%union{
    char* error_msg;
    Symbol* symbol_token;
    double float_token; // 对于SysY的浮点常量，我们需要先以double类型计算，再在语法树节点创建的时候转为float
    int int_token;
}
// declare the terminals (leaf-node of AST has token)
// Token(value) Token_id
%token <symbol_token> IDENT
%token <float_token> FLOAT_CONST
%token <int_token> INT_CONST
%token LEQ GEQ EQ NE // <=   >=   ==   != 
%token AND OR // &&    ||
%token CONST IF ELSE WHILE NONE_TYPE INT FLOAT
%token RETURN BREAK CONTINUE
%token ERROR

//give the type of nonterminals (nonleaf-node of AST doesn't have token)
%type <int_token> tmp_int 
// TODO: THEN和ELSE用于处理if和else的移进-规约冲突
// TODO: build ast_tree
%%
tmp_int: INT_CONST {$$ = $1;}
%% 

void yyerror(char* s, ...)
{
    ++error_num;
    std::cout << "parser error in line " << line << std::endl;
}