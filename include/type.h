#ifndef TYPE_H
#define TYPE_H

enum class OpType {
	// not defined 
	Void,

    // arithmetic
    Add,        // +
    Sub,        // -
    Mul,        // *
    Div,        // /
    Mod,        // %

    // logic
    And,        // &&
    Or,         // ||
    Not,        // !

    // compare 
    Eq,         // ==
    Neq,        // !=
    Lt,         // <
    Gt,         // >
    Le,         // <=
    Ge          // >=
};

class Type {
public:
    // 我们认为数组的类型为PTR
    // 为了方便写浮点数指针，我们把PTR细化成整型指针和浮点数指针
    enum ty { VOID = 0, INT = 1, FLOAT = 2, BOOL = 3, INT_PTR = 4, FLOAT_PTR = 5, DOUBLE = 6 } type;
    std::string GetTypeInfo();
    Type() { type = VOID; }
};

#endif