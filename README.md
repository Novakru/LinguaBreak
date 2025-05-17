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

### 测试自己的中间代码
```bash
./bin/SysYc ./test_output/example/temp.sy -llvm ./test_output/example/temp.out.ll
clang test_output/example/temp.out.ll -c -o test_output/example/temp.o -w
clang -static  test_output/example/temp.o ./lib/libsysy_x86.a -o test_output/example/temp
./test_output/example/temp (< ./testcase/functional_test/Advanced/lisp2.in)
```

### 测试llvm-pass的优化效果
```bash
opt -opaque-pointers=1 -passes=tailcallelim input.ll -S -o output.ll # 以尾递归优化为例
opt -opaque-pointers=1 -passes=tailcallelim ./test_output/example/temp.ll -S -o ./test_output/example/temp-O1.ll
```


### 使用gdb调试
```bash
# 1. 确保你编译时加了 -g 开启调试信息
make clean-all && make CXXFLAGS="-g"

# 2. 使用 gdb 启动程序
gdb --args ./bin/SysYc ./test_output/example/temp.sy -llvm ./test_output/example/temp.out.ll -O1

# 3. 在 gdb 中运行程序
(gdb) run

# 4. 程序崩溃后输入：
(gdb) bt
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

### 绝世好书
https://understanding-llvm-transformation-passes.readthedocs.io/en/latest/index.html
