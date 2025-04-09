#include "../include/type.h"
#include <iostream>
#include <vector>

std::string OpType::GetOpTypeString(){
    switch (optype) {
        case OpType::Void:
            return "Void";
        case OpType::Add:
            return "+";
        case OpType::Sub:
            return "-";
        case OpType::Mul:
            return "*";
        case OpType::Div:
            return "/";
        case OpType::Mod:
            return "%";
        case OpType::And:
            return "&&";
        case OpType::Or:
            return "||";
        case OpType::Not:
            return "!";
        case OpType::Eq:
            return "==";
        case OpType::Neq:
            return "!=";
        case OpType::Lt:
            return "<";
        case OpType::Gt:
            return ">";
        case OpType::Le:
            return "<=";
        case OpType::Ge:
            return ">=";
        default:
            return "";
    }
}

std::string BuiltinType::getString(){
    std::string str="Builtin:";
        switch(builtinty){
            case Int:
                return str+"Int";
            case Float:
                return str+"Float";
            case String:
                return str+"String";
            case Bool:
                return str+"Bool";
            case Void:
                return str+"Void";
            default:
                break;
        }
    return "";
}
std::string PointerType::getString(){
    return ("Pointer:"+pointeeType->getString());
}
std::string ArrayType::getString(){
    return ("Array:"+elementType->getString());
}


std::string NodeAttribute::GetConstValueInfo(Type* ty) {
    // if (!ConstTag) {
    //     return "";
    // }

    // if (ty.type == Type::INT) {
    //     return "ConstValue: " + std::to_string(val.IntVal);
    // } else if (ty.type == Type::FLOAT) {
    //     return "ConstValue: " + std::to_string(val.FloatVal);
    // } else if (ty.type == Type::BOOL) {
    //     return "ConstValue: " + std::to_string(val.BoolVal);
    // } else {
    //     return "";
    // }
    return "";
}

std::string NodeAttribute::GetAttributeInfo() { 
    // return T.GetTypeInfo() + "   " + GetConstValueInfo(T); 
    return "";
}