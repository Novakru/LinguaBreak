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
        BuiltinType() { builtinKind = BuiltinKind::Void; }
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
        Type* T;
        BuiltinType* type;
        bool ConstTag;

		// constValue
        union ConstVal {
            bool BoolVal;
            int IntVal;
            float FloatVal;
            double DoubleVal;
        } val;
		std::string StrVal;

		// array
		std::vector<int> dims{};    
		// 对于数组的初始化值，我们将高维数组看作一维后再存储 eg.([3 x [4 x i32]] => [12 x i32])
		std::vector<int> IntInitVals{}; 
		std::vector<float> FloatInitVals{};
		std::vector<int> RealInitvalPos;

		NodeAttribute() { 
			ConstTag = false; 
			val.IntVal=0; 
			StrVal="";
			type = new BuiltinType(BuiltinType::BuiltinKind::Void);
		}
		std::string GetAttributeInfo();
		std::string GetConstValueInfo();
};


#endif