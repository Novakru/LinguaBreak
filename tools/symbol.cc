#include "../include/symbol.h"

/* symbol */
Symbol *getSymbol(const std::string& name) {
    auto it = symbolMap.find(name);
    if (it != symbolMap.end()) {
        return it->second;  
    }

    Symbol* sym = new Symbol(name);
    symbolMap[name] = sym;
    return sym;
}

Symbol::Symbol(const std::string& name) : name(name) {}

std::string Symbol::getName() const {
    return name;
}


/* symbol table */
void SymbolTable::enter(Symbol* sym, void* value) {
    table[sym] = value;  

    if (!scopes.empty()) {
        scopes.back().push_back(sym); 
    }
}

void* SymbolTable::look(Symbol* sym) {
    auto it = table.find(sym);
    return (it != table.end()) ? it->second : nullptr;  
}


void SymbolTable::beginScope() {
    scopes.push_back({}); 
}

void SymbolTable::endScope() {
    if (!scopes.empty()) {
        for (Symbol* sym : scopes.back()) {
            table.erase(sym);  
        }
        scopes.pop_back();
    }
}

int SymbolTable::findScope(Symbol* sym) {
	int tabSize = table.size();
    for (int i = tabSize - 1; i >= 0; --i) {
        for (Symbol* s : scopes[i]) {
            if (s == sym) return i; 
        }
    }
    return -1; 
}

size_t SymbolTable::tableSize() const {
    return table.size();
}

Symbol* IdTable::add_id(std::string s) {
    auto it = id_table.find(s);
    if (it == id_table.end()) {
        Symbol* new_symbol = new Symbol(s);
        id_table[s] = new_symbol;
        return new_symbol;
    } else {
        return id_table[s];
    }
}