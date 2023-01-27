#pragma once
#include <string>

enum class TokenType {
//KEYWORDS
    PROGRAM,        //  program
    PROCEDURE,      //  procedure
    CONST,          //  const 
    VAR,            //  var
    BEGIN,          //  begin
    END,            //  end
    CALL,           //  call
    ODD,            //  odd
    IF,             //  if
    THEN,           //  then
    ELSE,           //  else
    WHILE,          //  while
    DO,             //  do
    READ,           //  read
    WRITE,          //  write
//OPERATORS
    PLUS,           //  +
    MINUS,          //  -
    TIMES,          //  *
    SLASH,          //  /
    EQU,            //  =
    NEQ,            //  <>
    LES,            //  <
    LEQ,            //  <=
    GTR,            //  >
    GEQ,            //  >=
//DELIMITER
    LPAREN,         //  (
    RPAREN,         //  )
    COMMA,          //  ,
    SEMICOLON,      //  ;

    BECOMES,        //  :=
    IDENTIFIER,     //  l{l|d}
    NUMBER,         //  d{d}

    ERROR,
    ENDOFFILE
};

enum class SymbolType {
    CONST,
    VAR,
    PROCEDURE,
    PROGRAM,
    PARAM,
    NONE
};


TokenType isKeyword(const std::string& token);

class Token
{
public:
    std::string name;
    TokenType type;
    int row, col;

    Token(const std::string& name, TokenType type, int row, int col);
    ~Token();
    
    std::string toString() const;
};

class Symbol {
public:
    std::string name;
    SymbolType type;
    int val, address, level;
    
    Symbol(const std::string& name, SymbolType type, int val = 0, int address = 0, int level = 0);
    ~Symbol();

    std::string toString() const;
};