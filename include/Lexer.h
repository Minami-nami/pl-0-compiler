#pragma once
#include "Symbol.h"
#include <filesystem>
#include <iostream>
#include <sstream>
#include <unordered_set>

struct KeyHash {
    size_t operator()(const Token& sym) const{
        return std::hash<std::string>()(sym.name);
    }
};

struct Equal {
    bool operator()(const Token& l, const Token& r) const{
        return l.name == r.name && l.type == r.type && l.row == r.row && l.col == r.col;
    }
};

enum class LEXERROR {
    ILEGAL_CHARACTER,
    ILEGAL_IDENTIFIER,
    MISSING_EQUAL_AFTER_COLON
};

class Lexer
{
private:
    std::filesystem::path Path;
    std::stringstream CodeStream;
    std::unordered_set<Token, KeyHash, Equal>::iterator it;
    std::unordered_set<Token, KeyHash, Equal> TokenTable;
    
    int row, col;
    TokenType curType;
    bool remain;
    
    
    bool isLegalch(char ch);
    void error(LEXERROR type, const std::string& token);

public:
    Lexer();
    ~Lexer();
    bool LoadFile(const char* FilePath);
    std::pair<TokenType, std::unordered_set<Token, KeyHash, Equal>::iterator> getToken();
    bool WriteFile();
    bool End();
    void Keep();
};
