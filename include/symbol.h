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
        void enter(Symbol* sym, NodeAttribute value); 
        //void* look(Symbol* sym); 
        NodeAttribute look(Symbol* sym); 
        void beginScope(); 
        void endScope(); 
        int findScope(Symbol* sym);  
        size_t tableSize() const;
        size_t scopesSize() const;
    private:
        //std::unordered_map<Symbol*, void*> table;  
        std::unordered_map<Symbol*, NodeAttribute> table;
        std::vector<std::vector<Symbol*>> scopes;  
    };

class IdTable {
    private:
        std::unordered_map<std::string, Symbol*> id_table{};
    
    public:
        Symbol* add_id(std::string s);
};


#endif