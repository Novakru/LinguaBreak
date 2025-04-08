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
    enum TypeKind {
        Builtin,
        Pointer,
		Array
    };
    TypeKind kind;
    virtual ~Type() = default;
};

class BuiltinType : public Type {
public:
    enum BuiltinKind { Int, Float, String };
    BuiltinKind builtinKind;
    BuiltinType(BuiltinKind kind) : builtinKind(kind) {
        this->kind = Type::Builtin;
    }
};

class PointerType : public Type {
public:
    Type* pointeeType;  // the type ptr point to 
    PointerType(Type* pointee) : pointeeType(pointee) {
        this->kind = Type::Pointer;
    }
};

class ArrayType : public Type {
public:
    Type* elementType;
    int length;         
    
    ArrayType(Type* elementType, int length)
        : elementType(elementType), length(length) {
        kind = Type::Array;
    }

    bool isFixedSize() const { return length >= 0; }
};

#endif