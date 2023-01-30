#pragma once
#include "Symbol.h"
#include "Token.h"
#include "Error.h"
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

class Lexer
{
private:
    std::filesystem::path Path;
    std::stringstream CodeStream;
    std::string linebuf;
    std::string::iterator lineptr;
    std::unordered_set<Token, KeyHash, Equal>::iterator it;
    std::unordered_set<Token, KeyHash, Equal> TokenTable;

    Error error;
    
    int row, col;
    TokenType curType;
    bool remain;
    
    bool isLegalch(char ch);

public:
    bool END;

    Lexer();
    ~Lexer();
    bool LoadFile(const char* FilePath);
    std::pair<TokenType, std::unordered_set<Token, KeyHash, Equal>::iterator> getToken();
    bool WriteFile();
    void ClearBuf();
    void Keep();
    void Err(ERROR etype, std::unordered_set<Token, KeyHash, Equal>::iterator it);
};
