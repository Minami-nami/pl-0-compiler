#include "Symbol.h"
#include <unordered_map>
#include <string>
#include <cstdio>

const std::unordered_map<std::string, TokenType> KeyWords = {
    {"program", TokenType::PROGRAM}, {"procedure", TokenType::PROCEDURE},
    {"const", TokenType::CONST}, {"var", TokenType::VAR}, {"if", TokenType::IF}, 
    {"then", TokenType::THEN}, {"else", TokenType::ELSE}, {"begin", TokenType::BEGIN}, {"end", TokenType::END}, 
    {"do", TokenType::DO}, {"while", TokenType::WHILE}, {"call", TokenType::CALL},
    {"odd", TokenType::ODD}, {"read", TokenType::READ}, {"write", TokenType::WRITE}
};

const std::string TP[6] = {
    "const", "var", "procedure", "program", "param", "none"
};

TokenType isKeyword(const std::string& token) {
    auto it = KeyWords.find(token);
    return (it == KeyWords.end()? TokenType::ERROR: it->second);
}

std::string Token::toString() const{
    char bf[128];
    snprintf(bf, 128, "< %-16s , %-2d >\t\t[行 %-3d, 列 %-3d]", name.c_str(), static_cast<int>(type), row, col);
    return std::string(bf);
}

Token::Token(const std::string& name, TokenType type, int row, int col)
{
    this->name = name;
    this->type = type;
    this->row  = row;
    this->col  = col;
}

Token::~Token()
{
}

std::string Symbol::toString() const {
    char bf[128];
    
    snprintf(bf, 128, "%-16s\t%-12s\t%d %d %d", name.c_str(), 
                        TP[static_cast<int>(type)].c_str(), 
                        val, address, level);
    return std::string(bf);
}

Symbol::Symbol(const std::string& name, SymbolType type, int val, int address, int level) {
    this->name = name;
    this->type = type;
    this->val = val;
    this->address = address;
    this->level = level;
}

Symbol::~Symbol() {
    
}