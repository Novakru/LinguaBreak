#ifndef SYMBOL_H
#define SYMBOL_H
#include <string>
#include <vector>
#include <unordered_map>

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
    void enter(Symbol* sym, void* value); 
    void* look(Symbol* sym); 
    void beginScope(); 
    void endScope(); 
    int findScope(Symbol* sym);  
    size_t tableSize() const;
	
private:
    std::unordered_map<Symbol*, void*> table;  
    std::vector<std::vector<Symbol*>> scopes;  
};


#endif