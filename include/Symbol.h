#pragma once
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <fstream>

enum class SymbolType {
    CONST,
    VAR,
    PROCEDURE,
    PROGRAM
};

class Symbol {
public:
    SymbolType type;
    int val, address, level, size;
    
    Symbol(SymbolType type = SymbolType::VAR, int val = -1, int address = -1, int level = -1, int size = 1);
    ~Symbol();

    std::string toString(const std::string& name) const;
};

class SymbolTable {
public:
    std::string name;
    int level;
    SymbolTable* prev;
    std::unordered_map<std::string, SymbolTable> childs;
    bool insert(const std::string& Sname, const Symbol& s);
    bool exist(const std::string& Sname);
    std::pair<std::string, Symbol*> search(const std::string& Sname);
    void write(std::ofstream& output);
    SymbolTable(const std::string& _name, int _level = 0, SymbolTable* _prev = nullptr):name(_name), level(_level), prev(_prev) {}
private:
    std::unordered_map<std::string, Symbol> table;
};