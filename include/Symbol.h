#pragma once
#include <string>

enum class SymbolType {
    CONST,
    VAR,
    PROCEDURE,
    PROGRAM,
    PARAM,
    NONE
};


class Symbol {
public:
    std::string name;
    SymbolType type;
    int val, address, level, size;
    
    Symbol(const std::string& name, SymbolType type, int val = 0, int address = 0, int level = 0, int size = 0);
    ~Symbol();

    std::string toString() const;
};