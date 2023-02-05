#include "Symbol.h"
#include <unordered_map>
#include <filesystem>
#include <string>
#include <iostream>
#include <cstdio>

const std::unordered_map<SymbolType, std::string> TP = {
    {SymbolType::CONST,     "const"},       {SymbolType::VAR,       "var"}, 
    {SymbolType::PROCEDURE, "procedure"},   {SymbolType::PROGRAM,   "program"}
};

std::string Symbol::toString(const std::string& name) const {
    /* Program main:
     * __________________________________________________________________
     *      name     |    type    |   val/level   |  address  |   size   
     * ——————————————————————————————————————————————————————————————————
     * sample         sample       2               1           4
     * 
    */
    char bf[128];
    std::string typeinfo = TP.find(type)->second;
    int val_level = (type == SymbolType::CONST)? val: level;
    snprintf(bf, 128, "%-14s %-12s %-15d %-11d %-10d", name.c_str(), typeinfo.c_str(), val_level, address, size);
    return std::string(bf);
}

Symbol::Symbol(SymbolType type, int val, int address, int level, int size) {
    this->type = type;
    this->val = val;
    this->address = address;
    this->level = level;
    this->size = size;
}

Symbol::~Symbol() {
    
}

bool SymbolTable::insert(const std::string& Sname, const Symbol& s) {
    auto it = this->table.find(Sname);
    if (it == this->table.end()) {
        this->table.insert({Sname, s});
        return true;
    }
    else 
        return false;
}

bool SymbolTable::exist(const std::string& Sname) {
    return this->table.find(Sname) != this->table.end();
}

std::pair<std::string, Symbol*> SymbolTable::search(const std::string& Sname) {
    auto it = this->table.find(Sname);
    if (it != this->table.end())
        return std::make_pair(it->first, &(it->second));
    else return this->prev == nullptr? std::make_pair("", nullptr): this->prev->search(Sname);
}

void SymbolTable::write(std::ofstream& output) {
    if (prev == nullptr)
        output << "Program ";
    else 
        output << "Procedure ";
    output << name << ":\n";
    output << "__________________________________________________________________\n"
            "     name     |    type    |   val/level   |  address  |   size   \n"
            "——————————————————————————————————————————————————————————————————\n";
    for (auto&& [Sname, symbol]: table) {
        output << symbol.toString(Sname) << '\n';
    }
    for (auto&& [_, symboltable]: childs) {
        symboltable.write(output);
    }
}