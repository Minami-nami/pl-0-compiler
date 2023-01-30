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