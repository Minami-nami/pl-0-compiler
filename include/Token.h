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

bool StatementBegin(TokenType type);
bool StatementEnd(TokenType type);
bool BlockBegin(TokenType type);
bool BlockEnd(TokenType type);
bool cmpop(TokenType type);
bool addop(TokenType type);
bool mulop(TokenType type);
bool op(TokenType type);
bool FactorBegin(TokenType type);
bool FactorEnd(TokenType type);
bool TermBegin(TokenType type);
bool TermEnd(TokenType type);
bool ExpBegin(TokenType type);
bool ExpEnd(TokenType type);
bool LexpBegin(TokenType type);
bool LexpEnd(TokenType type);

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