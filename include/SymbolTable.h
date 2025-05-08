#ifndef __SYMBOLTABLE_H__
#define __SYMBOLTABLE_H__

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

enum class SymbolType {
    VARIABLE,
    FUNCTION
};

struct SymbolInfo {
    std::string name;
    std::string type;
    SymbolType symbolType;
    
    SymbolInfo(const std::string& name, const std::string& type, SymbolType symbolType)
        : name(name), type(type), symbolType(symbolType) {}
};

class SymbolTable {
private:
    std::vector<std::unordered_map<std::string, std::shared_ptr<SymbolInfo>>> all_scopes_data;
    std::vector<size_t> active_scope_indices; // Stack of indices into all_scopes_data
    
public:
    SymbolTable();
    void enterScope();
    void exitScope();
    bool declare(const std::string& name, const std::string& type, SymbolType symbolType);
    std::shared_ptr<SymbolInfo> lookup(const std::string& name);
    void print() const;
};

#endif