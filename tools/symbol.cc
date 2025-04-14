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

void SymbolTable::enter(Symbol* sym, NodeAttribute value) {
    //std::cout<<"ADD symbol "<<sym->getName()<<" in scope"<<scopes.size()-1<<std::endl;
    if (!scopes.empty()&&sym!=nullptr) {
        int cur_scope=scopes.size();
        scopes[cur_scope-1][sym]=value;
    }
}

NodeAttribute SymbolTable::look(Symbol* sym) {//获取当前作用域下，该symbol对应的node(比如name,type)
    int cur_scope=scopes.size();
    if(cur_scope!=0)
    {
        auto it=scopes[cur_scope-1].find(sym);
        //return (it != scopes[cur_scope-1].end()) ? it->second : NodeAttribute();  
        if(it != scopes[cur_scope-1].end())
        {
            //std::cout<<"Find symbol "<<sym->getName()<<std::endl;
            return it->second;
        }
    }
    //std::cout<<"Not Find symbol "<<sym->getName()<<std::endl;
    return NodeAttribute();  
}

void SymbolTable::beginScope() {//scopes的size增大1
    scopes.push_back({}); 
}

void SymbolTable::endScope() {//scopes的size减少1
    if (!scopes.empty()) {
        scopes.pop_back();
    }
}

int SymbolTable::findScope(Symbol* sym) {
    int tabSize=scopes.size();
    //std::cout<<"scopeSize="<<scopes.size()<<std::endl;
    //std::cout<<"sym ="<<(sym->getName())<<std::endl;
    for (int i = tabSize - 1; i >= 0; --i) {
        auto it=scopes[i].find(sym);
        if(it!=scopes[i].end())
        {
            return i;
        }
    }
    return -1; 
}

// size_t SymbolTable::tableSize() const {
//     return table.size();
// }
size_t SymbolTable::scopesSize() const
{
    return scopes.size();
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