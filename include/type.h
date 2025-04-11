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

class Type {
    public:
        enum TypeKind {
            Builtin=0,
            Pointer=6,
            Array=7
        }kind;
        virtual std::string getString()=0;
        virtual int getType()=0;
        virtual ~Type() = default;
};
    
class BuiltinType : public Type {
    public:
        enum BuiltinKind { Int =1, Float=2, String=3 , Bool=4, Void=5 }builtinKind;
        BuiltinType(BuiltinKind kind) : builtinKind(kind) {
            this->kind = Type::Builtin;
        }
        std::string getString();
        int getType();
};

class PointerType : public Type {
    public:
        Type* pointeeType;  // the type ptr point to 
        PointerType(Type* pointee) : pointeeType(pointee) {
            this->kind = Type::Pointer;
        }
        std::string getString();
        int getType();
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
        int getType();
        bool isFixedSize() const { return length >= 0; }
};

class NodeAttribute {
    public:
        int line_number = -1;
        BuiltinType* type;
        bool ConstTag;
        union ConstVal {
            bool BoolVal;
            int IntVal;
            float FloatVal;
            double DoubleVal;
        } val;
        std::string StrVal;
        NodeAttribute() { ConstTag = false; val.IntVal=0;StrVal="";}
        std::string GetAttributeInfo();
        std::string GetConstValueInfo();
};


#endif