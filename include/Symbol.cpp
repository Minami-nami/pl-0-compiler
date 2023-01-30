#include "Symbol.h"
#include <unordered_map>
#include <string>
#include <cstdio>

const std::unordered_map<SymbolType, std::string> TP = {
    {SymbolType::CONST,     "const"},   {SymbolType::VAR,   "var"},     {SymbolType::PROCEDURE, "procedure"},
    {SymbolType::PROGRAM,   "program"}, {SymbolType::PARAM, "param"},   {SymbolType::NONE,      "none"}
};

std::string Symbol::toString() const {
    /* __________________________________________________________________
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

Symbol::Symbol(const std::string& name, SymbolType type, int val, int address, int level, int size) {
    this->name = name;
    this->type = type;
    this->val = val;
    this->address = address;
    this->level = level;
    this->size = size;
}

Symbol::~Symbol() {
    
}