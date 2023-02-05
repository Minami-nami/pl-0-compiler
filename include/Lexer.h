#pragma once
#include "Symbol.h"
#include "Token.h"
#include "Error.h"
#include <filesystem>
#include <iostream>
#include <sstream>
#include <unordered_set>

struct KeyHash_Token {
    size_t operator()(const Token& sym) const{
        return std::hash<std::string>()(sym.name);
    }
};

struct Equal_Token {
    bool operator()(const Token& l, const Token& r) const{
        return l.name == r.name && l.type == r.type && l.row == r.row && l.col == r.col;
    }
};

class Lexer
{
private:
    std::stringstream CodeStream;
    std::string linebuf;
    std::string::iterator lineptr;
    std::unordered_set<Token, KeyHash_Token, Equal_Token>::iterator it;
    std::unordered_set<Token, KeyHash_Token, Equal_Token> TokenTable;
    int row, col;
    TokenType curType;
    bool remain;
    
    bool isLegalch(char ch);

public:
    std::filesystem::path Path;
    Error error;
    bool END;
    Lexer();
    ~Lexer();
    bool LoadFile(const std::string& FilePath);
    std::pair<TokenType, std::unordered_set<Token, KeyHash_Token, Equal_Token>::iterator> getToken();
    bool write(const std::filesystem::path& WritePath);
    void ClearBuf();
    void Keep();
    void Err(ERROR etype, std::unordered_set<Token, KeyHash_Token, Equal_Token>::iterator it);
    void Err(ERROR etype, int real, int expected);
};
