### How to complile LinguaBreak
```bash
make lexer # input: sysy_lexer.l output: sysy_lexer.cc
make parser # input: sysy_parser.y output: sysy_parser.tab.cc and sysy_parser.tab.hh
```

### How to run LiguaBreak
```bash 
./bin/SysYc [input_file] -option [output_file]
./bin/SysYc ./testcase/example/temp.sy -lexer ./testcase/example/lexer_out.txt 
```

### How to output file struct
```bash
tree -L 3
```

### 使用自动测试脚本
```bash
mkdir -p test_output/functional_testIR/
python grade.py 3 0 
```

```
Your Syntax Tree Structure
│
├── ASTNode (base)
│   ├── line: int
│   ├── attribute: NodeAttribute
│   ├── codeIR(): virtual
│   ├── TypeCheck(): virtual
│   ├── printAST(): virtual
│
├── __ExprBase (ExprNode)
│   ├── Exp
│   │   └── addExp: ExprBase
│   ├── ConstExp
│   │   └── addExp: ExprBase
│   ├── Binary Expressions
│   │   ├── AddExp (left, op, right)
│   │   ├── MulExp (left, op, right)
│   │   ├── RelExp (left, op, right)
│   │   ├── EqExp (left, op, right)
│   │   ├── LAndExp (left, op, right)
│   │   └── LOrExp (left, op, right)
│   ├── Lval (VariableNode + ArefNode)
│   │   ├── name: Symbol*
│   │   └── dims: vector<ExprBase>*
│   ├── FuncCall (FuncallNode)
│   │   ├── name: Symbol*
│   │   └── funcRParams: ExprBase
│   ├── UnaryExp
│   │   ├── unaryOp: OpType
│   │   └── unaryExp: ExprBase
│   ├── Literals
│   │   ├── IntConst (IntegerLiteralNode)
│   │   ├── FloatConst (FloatLiteralNode)
│   │   └── PrimaryExp (wrapper)
│
├── __Stmt (StmtNode)
│   ├── AssignStmt (AssignNode)
│   │   ├── lval: ExprBase
│   │   └── exp: ExprBase
│   ├── ExprStmt (ExprStmtNode)
│   │   └── exp: ExprBase
│   ├── BlockStmt (BlockNode)
│   │   └── b: Block
│   ├── IfStmt (IfNode)
│   │   ├── Cond: ExprBase
│   │   ├── ifStmt: Stmt
│   │   └── elseStmt: Stmt
│   ├── WhileStmt (WhileNode)
│   │   ├── Cond: ExprBase
│   │   └── loopBody: Stmt
│   ├── ContinueStmt (ContinueNode)
│   ├── BreakStmt (BreakNode)
│   └── RetStmt (ReturnNode)
│       └── retExp: ExprBase
│
├── __Def
│   ├── VarDef/VarDef_no_init
│   │   ├── name: Symbol*
│   │   ├── dims: vector<ExprBase>*
│   │   └── init: InitValBase
│   └── ConstDef
│       ├── name: Symbol*
│       ├── dims: vector<ExprBase>*
│       └── init: InitValBase
│
├── __DeclBase
│   ├── VarDecl
│   │   ├── type_decl: Type*
│   │   └── var_def_list: vector<Def>*
│   └── ConstDecl
│       ├── type_decl: Type*
│       └── var_def_list: vector<Def>*
│
├── __Block (BlockNode)
│   └── item_list: vector<BlockItem>*
│
├── __FuncFParam
│   ├── type_decl: Type*
│   ├── dims: vector<ExprBase>*
│   └── name: Symbol*
│
├── __FuncDef
│   ├── return_type: Type*
│   ├── name: Symbol*
│   ├── formals: vector<FuncFParam>*
│   └── block: Block
│
└── __Program
    └── comp_list: vector<CompUnit>*
```
