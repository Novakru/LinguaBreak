#ifndef TYPE_H
#define TYPE_H

#include <iostream>
#include <vector>
class OpType{
public:
    enum Op{
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
    }optype;

    OpType() { optype = Void; }
    OpType(Op op) { optype = op; }
    std::string GetOpTypeString();
};


// class Type {
// public:
//     // 我们认为数组的类型为PTR
//     // 为了方便写浮点数指针，我们把PTR细化成整型指针和浮点数指针
//     enum ty { VOID = 0, INT = 1, FLOAT = 2, BOOL = 3, INT_PTR = 4, FLOAT_PTR = 5, DOUBLE = 6 } type;
//     std::string GetTypeInfo();
//     Type() { type = VOID; }
// };

// enum ty{
//     Void=0,
//     Int=1,
//     Float=2,
//     Bool=3,
//     String=4,


// }type;

class Type {
    public:
        enum TypeKind {
            Builtin,
            Pointer,
            Array
        }ty;
    TypeKind kind;
    virtual std::string getString()=0;
    virtual ~Type() = default;
};
    
class BuiltinType : public Type {
public:
    enum BuiltinKind {  Int, Float, String , Bool, Void }builtinty;
    BuiltinKind builtinKind;
    BuiltinType(BuiltinKind kind) : builtinKind(kind) {
        this->kind = Type::Builtin;
    }
    std::string getString();
};

class PointerType : public Type {
public:
    Type* pointeeType;  // the type ptr point to 
    PointerType(Type* pointee) : pointeeType(pointee) {
        this->kind = Type::Pointer;
    }
    std::string getString();
};
    
class ArrayType : public Type {
public:
    Type* elementType;
    int length;         
        
    ArrayType(Type* elementType, int length)
        : elementType(elementType), length(length) {
        this->kind = Type::Array;
    }
    std::string getString();
    bool isFixedSize() const { return length >= 0; }
};

class NodeAttribute {
public:
    int line_number = -1;
    //Type* T;
    //Type* type;
    BuiltinType* type;
    bool ConstTag;
    union ConstVal {
        bool BoolVal;
        int IntVal;
        float FloatVal;
        double DoubleVal;
    } val;
    NodeAttribute() { ConstTag = false; val.IntVal=0;}
    std::string GetAttributeInfo();
    std::string GetConstValueInfo(Type* ty);
};


#endif