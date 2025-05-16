#include "../include/SymbolTable.h"

SymbolTable::SymbolTable() {
    // Start with global scope
    all_scopes_data.emplace_back(); // Create the global scope map
    active_scope_indices.push_back(0); // Global scope is at index 0 and is active
}

void SymbolTable::enterScope() {
    all_scopes_data.emplace_back(); // Add a new empty map for the new scope
    active_scope_indices.push_back(all_scopes_data.size() - 1); // Push its index onto the active stack
}

void SymbolTable::exitScope() {
    if (active_scope_indices.size() > 1) { // Always keep at least the global scope active
        active_scope_indices.pop_back(); // Remove from active stack, but not from all_scopes_data
    }
}

bool SymbolTable::declare(const std::string& name, const std::string& type, SymbolType symbolType) {
    if (active_scope_indices.empty()) {
        // This should ideally not happen if constructor ensures global scope
        return false; 
    }
    size_t current_scope_idx = active_scope_indices.back();
    auto& current_scope_map = all_scopes_data[current_scope_idx];
    
    // Check if symbol already exists in current scope
    if (current_scope_map.find(name) != current_scope_map.end()) {
        return false; // Already exists in this scope
    }
    
    // Add to current scope
    current_scope_map[name] = std::make_shared<SymbolInfo>(name, type, symbolType);
    return true;
}

std::shared_ptr<SymbolInfo> SymbolTable::lookup(const std::string& name) {
    // Search from innermost active scope outward
    for (auto it = active_scope_indices.rbegin(); it != active_scope_indices.rend(); ++it) {
        size_t scope_idx_to_check = *it;
        const auto& scope_map = all_scopes_data[scope_idx_to_check];
        auto found = scope_map.find(name);
        if (found != scope_map.end()) {
            return found->second;
        }
    }
    return nullptr; // Not found
}

void SymbolTable::print() const {
    std::cout << "Symbol Table:" << std::endl;
    for (size_t i = 0; i < all_scopes_data.size(); ++i) {
        std::cout << "  Scope " << i << ":" << std::endl;
        if (all_scopes_data[i].empty()) {
            std::cout << "    (empty)" << std::endl;
        } else {
            for (const auto& entry : all_scopes_data[i]) {
                std::cout << "    " << entry.first << " : " << entry.second->type 
                          << " (" << (entry.second->symbolType == SymbolType::VARIABLE ? "Variable" : "Function")
                          << ")" << std::endl;
            }
        }
    }
}