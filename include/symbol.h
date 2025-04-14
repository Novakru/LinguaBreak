#ifndef SYMBOL_H
#define SYMBOL_H
#include <string>
#include <vector>
#include <unordered_map>
#include"type.h"

class Symbol {
public:
    std::string getName() const; 
    explicit Symbol(const std::string& name);
    explicit Symbol() {};

private:
    std::string name;
};

Symbol *getSymbol(const std::string& name);
static std::unordered_map<std::string, Symbol*> symbolMap; 

class SymbolTable {
    public:
        //SymbolTable(){beginScope();}
        void enter(Symbol* sym, NodeAttribute value); 
        //void* look(Symbol* sym); 
        NodeAttribute look(Symbol* sym); 
        void beginScope(); 
        void endScope(); 
        int findScope(Symbol* sym);  
        //size_t tableSize() const;
        size_t scopesSize() const;
    private:
        //std::unordered_map<Symbol*, NodeAttribute> table;//疑点，如果多个作用域下都存储有某个变量，那么table中怎么存储？
        std::vector<std::unordered_map<Symbol*, NodeAttribute>> scopes;  
    };

class IdTable {
    private:
        std::unordered_map<std::string, Symbol*> id_table{};
    
    public:
        Symbol* add_id(std::string s);
};


#endif