### How to complile LinguaBreak
```bash
make lexer # input: sysy_lexer.l output: sysy_lexer.cc
make parser # input: sysy_parser.y output: sysy_parser.tab.cc and sysy_parser.tab.hh
```

### How to run LiguaBreak
```bash 
./bin/SysYc [input_file] -option [output_file]
./bin/SysYc ./test/example/temp.sy -lexer ./test/example/lexer_out.txt 
```

### How to output file struct
```bash
tree -L 3
```


```
Node
├── AST
├── ExprNode                    （表示表达式的节点）
│   ├── AbstractAssignNode      （赋值）
│   │   ├── AssignNode          （赋值表达式 `=`）
│   ├── BinaryOpNode            （二元运算表达式 `x+y`、`x-y` 等）
│   │   ├── LogicalAndNode      （`&&`）
│   │   ├── LogicalOrNode       （`||`）
│   ├── FuncallNode             （函数调用表达式）
│   ├── LHSNode                 （能够成为赋值目标的节点）
│   │   ├── ArefNode            （数组表达式 `a[i]`）
│   ├── VariableNode            （变量表达式）
│   ├── LiteralNode             （字面量）
│   │   ├── IntegerLiteralNode  （整数字面量）
|	|	├── FloatLiteralNode	（浮点数字面量）
│   │   ├── StringLiteralNode   （字符串字面量）
│   ├── UnaryOpNode             （一元运算表达式 `+x`、`-x` 等）
├── StmtNode                    （表示语句的节点）
│   ├── BlockNode               （程序块 `{...}`）
│   ├── BreakNode               （`break` 语句）
│   ├── ContinueNode            （`continue` 语句）
│   ├── ExprStmtNode            （单独构成语句的表达式）
│   ├── IfNode                  （`if` 语句）
│   ├── ReturnNode              （`return` 语句）
│   ├── WhileNode               （`while` 语句）
├── TypeNode                    （存储类型的节点）

```
