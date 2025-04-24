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
    std::vector<std::unordered_map<std::string, std::shared_ptr<SymbolInfo>>> scopes;
    
public:
    SymbolTable() {
        // Start with global scope
        enterScope();
    }
    
    void enterScope() {
        scopes.push_back(std::unordered_map<std::string, std::shared_ptr<SymbolInfo>>());
    }
    
    void exitScope() {
        if (scopes.size() > 1) { // Always keep at least the global scope
            scopes.pop_back();
        }
    }
    
    bool declare(const std::string& name, const std::string& type, SymbolType symbolType) {
        // Check if symbol already exists in current scope
        if (scopes.back().find(name) != scopes.back().end()) {
            return false; // Already exists in this scope
        }
        
        // Add to current scope
        scopes.back()[name] = std::make_shared<SymbolInfo>(name, type, symbolType);
        return true;
    }
    
    std::shared_ptr<SymbolInfo> lookup(const std::string& name) {
        // Search from innermost scope outward
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end()) {
                return found->second;
            }
        }
        return nullptr; // Not found
    }
    
    void print() const {
        std::cout << "Symbol Table:" << std::endl;
        for (size_t i = 0; i < scopes.size(); ++i) {
            std::cout << "  Scope " << i << ":" << std::endl;
            for (const auto& entry : scopes[i]) {
                std::cout << "    " << entry.first << " : " << entry.second->type 
                          << " (" << (entry.second->symbolType == SymbolType::VARIABLE ? "Variable" : "Function")
                          << ")" << std::endl;
            }
        }
    }
};

#endif