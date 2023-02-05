#include <unordered_map>
#include <string>
#include "Token.h"

const std::unordered_map<std::string, TokenType> KeyWords = {
    {"program", TokenType::PROGRAM},    {"procedure",   TokenType::PROCEDURE},  {"const",   TokenType::CONST},
    {"var",     TokenType::VAR},        {"if",          TokenType::IF},         {"then",    TokenType::THEN}, 
    {"else",    TokenType::ELSE},       {"begin",       TokenType::BEGIN},      {"end",     TokenType::END}, 
    {"do",      TokenType::DO},         {"while",       TokenType::WHILE},      {"call",    TokenType::CALL},
    {"odd",     TokenType::ODD},        {"read",        TokenType::READ},       {"write",   TokenType::WRITE}
};

const std::unordered_map<TokenType, std::string> TokenName = {
    {TokenType::PLUS,       "plus"},        {TokenType::MINUS,      "minus"},       {TokenType::TIMES,      "times"},
    {TokenType::SLASH,      "slash"},       {TokenType::EQU,        "equ"},         {TokenType::NEQ,        "neq"},
    {TokenType::LES,        "les"},         {TokenType::LEQ,        "leq"},         {TokenType::GTR,        "gtr"},
    {TokenType::GEQ,        "geq"},         {TokenType::LPAREN,     "lparen"},      {TokenType::RPAREN,     "rparen"},
    {TokenType::COMMA,      "comma"},       {TokenType::SEMICOLON,  "semicolon"},   {TokenType::BECOMES,    "becomes"},
    {TokenType::IDENTIFIER, "identifier"},  {TokenType::NUMBER,     "number"},      {TokenType::ERROR,      "error"},
    {TokenType::ENDOFFILE,  "eof"}
};

TokenType isKeyword(const std::string& token) {
    auto it = KeyWords.find(token);
    return (it == KeyWords.end()? TokenType::ERROR: it->second);
}

std::string Token::toString() const{
    /* _______________________________________________
     *      name     |    type    |   row   |   col   
     * ———————————————————————————————————————————————
     *     lololol        inde         2         1     
    */
    char bf[128];
    std::string typeinfo = name;
    if (isKeyword(typeinfo) == TokenType::ERROR) typeinfo = TokenName.find(type)->second;
    else typeinfo = "keyword";
    snprintf(bf, 128, "%-14s %-12s %-9d %-9d", name.c_str(), typeinfo.c_str(), row, col);
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

bool StatementBegin(TokenType type) {
    return type == TokenType::IDENTIFIER || type == TokenType::CALL || type == TokenType::BEGIN || type == TokenType::IF ||
           type == TokenType::WHILE || type == TokenType::WRITE || type == TokenType::READ;
};

bool StatementEnd(TokenType type) {
    return type == TokenType::END || type == TokenType::SEMICOLON;
};

bool BlockBegin(TokenType type) {
    return StatementBegin(type) || type == TokenType::CONST || type == TokenType::PROCEDURE || type == TokenType::VAR;
};

bool BlockEnd(TokenType type) {
    return type == TokenType::END;
};

bool cmpop(TokenType type) {
    return type == TokenType::EQU || type == TokenType::NEQ || type == TokenType::GTR ||
           type == TokenType::GEQ || type == TokenType::LES || type == TokenType::LEQ;
};

bool addop(TokenType type) {
    return type == TokenType::PLUS || type == TokenType::MINUS;
};

bool mulop(TokenType type) {
    return type == TokenType::TIMES || type == TokenType::SLASH;
};

bool op(TokenType type) {
    return cmpop(type) || addop(type) || mulop(type);
};

bool FactorBegin(TokenType type) {
    return type == TokenType::IDENTIFIER || type == TokenType::NUMBER || type == TokenType::LPAREN;
}

bool FactorEnd(TokenType type) {
    return TermEnd(type) || mulop(type);
}

bool TermBegin(TokenType type) {
    return FactorBegin(type);
}

bool TermEnd(TokenType type) {
   return ExpEnd(type) || addop(type);
}

bool ExpBegin(TokenType type) {
    return FactorBegin(type) || addop(type);
}

bool ExpEnd(TokenType type) {
    return StatementEnd(type) || cmpop(type) || LexpEnd(type) || type == TokenType::RPAREN || type == TokenType::COMMA;
}

bool LexpBegin(TokenType type) {
    return ExpBegin(type) || type == TokenType::ODD;
}

bool LexpEnd(TokenType type) {
    return type == TokenType::THEN || type == TokenType::DO;
}