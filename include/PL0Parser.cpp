#include "PL0Parser.h"
#include <string.h>

//<prog> → program <id>；<block>
void Parser::ProcProg() {
    this->ProcId(true, SymbolType::PROGRAM);                             //<id>
    while (!lexer.END) {                      //;
        auto [type, value] = lexer.getToken();
        if (type == TokenType::SEMICOLON) {
            break;
        }
        else if (type == TokenType::CONST || type == TokenType::VAR || type == TokenType::PROCEDURE || type == TokenType::BEGIN) {
            lexer.Err(ERROR::MISSING_SEMICOLON, value);
            lexer.Keep();
            break;
        }
        else {
            lexer.Err(ERROR::EXPECTING_SEMICOLON, value);
        }
    }
    this->ProcBlock();                          //<block>
}

//<block> → [<condecl>][<vardecl>][<proc>]<body>
void Parser::ProcBlock() {
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        switch (type)
        {
        case TokenType::CONST:
            this->ProcCondecl();
            return;
        case TokenType::VAR:
            this->ProcVardecl();
            return;
        case TokenType::PROCEDURE:
            this->ProcProc();
            return;
        case TokenType::BEGIN:
            this->ProcBody();
            return;
        default:
            lexer.Err(ERROR::EXPECTING_BEGIN, value);
            break;
        }
    }
}

//<condecl> → const <const>{,<const>};
void Parser::ProcCondecl() {
    this->ProcConst();

    while (!lexer.END) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::VAR) {           //[<vardecl>]
            this->ProcVardecl();
            break;
        }
        else if (type == TokenType::PROCEDURE) {//[<proc>]
            this->ProcProc();
            break;
        }
        else if (type == TokenType::BEGIN) {
            this->ProcBody();
            break;
        }
        else {
            lexer.Err(ERROR::EXPECTING_BEGIN, value);
        }
    }
}

//<const> → <id>:=<integer>
void Parser::ProcConst() {
    this->ProcId(true, SymbolType::CONST);

    this->ProcBecomes();
    
    this->ProcInteger();

    while (!lexer.END) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::COMMA) {         //<integer>
            this->ProcConst();
            return;
        }
        else if (type == TokenType::IDENTIFIER) {
            lexer.Keep();
            lexer.Err(ERROR::MISSING_COMMA, value);
        }
        else if (type == TokenType::SEMICOLON) {
            return;
        }
        else if (type == TokenType::VAR || type == TokenType::PROCEDURE || type == TokenType::BEGIN) {
            lexer.Keep();
            lexer.Err(ERROR::MISSING_SEMICOLON, value);
            break;
        }
        else {
            lexer.Err(ERROR::EXPECTING_SEMICOLON, value);
        }
    }
}

//<vardecl> → var <id>{,<id>};
void Parser::ProcVardecl() {
    this->ProcVar();

    while (!lexer.END) {
        auto [type, value] = lexer.getToken();

        if (type == TokenType::PROCEDURE) {//[<proc>]
            this->ProcProc();
            break;
        }
        else if (type == TokenType::BEGIN) {
            this->ProcBody();
            break;
        }
        else {
            lexer.Err(ERROR::EXPECTING_BEGIN, value);
        }
    }
}

//<id>{,<id>};
void Parser::ProcVar() {
    this->ProcId(true, SymbolType::VAR);
    while (!lexer.END) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::COMMA) {         //var
            this->ProcId(true, SymbolType::VAR);
        }
        else if (type == TokenType::IDENTIFIER) {
            lexer.Keep();
            lexer.Err(ERROR::MISSING_COMMA, value);
            this->ProcId(true, SymbolType::VAR);
        }
        else if (type == TokenType::SEMICOLON) {
            return;
        }
        else if (type == TokenType::PROCEDURE || type == TokenType::BEGIN) {
            lexer.Keep();
            lexer.Err(ERROR::MISSING_SEMICOLON, value);
            break;
        }
        else {
            lexer.Err(ERROR::EXPECTING_SEMICOLON, value);
        }
    }
}

//<proc> → procedure <id>（[<id>{,<id>}]）;<block>{;<proc>}
void Parser::ProcProc() {
    this->ProcId(true, SymbolType::PROCEDURE);

    while (!lexer.END) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::LPAREN) {         //(
            break;
        }
        else if (type == TokenType::SEMICOLON || type == TokenType::IDENTIFIER) {
            lexer.Err(ERROR::MISSING_LPAREN, value);
            lexer.Keep();
            break;
        }
        else {
            lexer.Err(ERROR::EXPECTING_LPAREN, value);
        }
    }

    this->ProcProcParam();
    while (!lexer.END) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::SEMICOLON) {         //;
            break;
        }
        else if (type == TokenType::CONST || type == TokenType::VAR || type == TokenType::PROCEDURE || type == TokenType::BEGIN) {
            lexer.Err(ERROR::MISSING_SEMICOLON, value);
            lexer.Keep();
            break;
        }
        else {
            lexer.Err(ERROR::EXPECTING_SEMICOLON, value);
        }
    }

    this->ProcBlock();                              //<block>

    while (!lexer.END) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::SEMICOLON) {         //{;<proc>}
            while (!lexer.END) {
                auto [type, value] = lexer.getToken();
                if (type == TokenType::PROCEDURE) {
                    this->ProcProc();
                    return;
                }
                else {
                    lexer.Err(ERROR::EXPECTING_PROCEDURE, value);
                    lexer.Keep();
                    this->ProcProc();
                }
            }
            return;
        }
        else if (type == TokenType::PROCEDURE) {
            lexer.Keep();
            lexer.Err(ERROR::MISSING_SEMICOLON, value);
            this->ProcProc();
            return;
        }
        else if (type == TokenType::BEGIN) {
            this->ProcBody();
            return;
        }
        else {
            lexer.Err(ERROR::EXPECTING_BEGIN, value);
        }
    }
}

// [<id>{,<id>}])
void Parser::ProcProcParam() {
    while (!lexer.END) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::COMMA) { 
            lexer.Err(ERROR::MISSING_IDENTIFIER, value);
            this->ProcId(false, SymbolType::PARAM);
        }
        else if (type == TokenType::IDENTIFIER) {
            lexer.Keep();
            break;
        }
        else if (type == TokenType::RPAREN) {
            return;
        }
        else if (type == TokenType::SEMICOLON) {
            lexer.Err(ERROR::MISSING_RPAREN, value);
            lexer.Keep();
            return;
        }
        else {
            lexer.Err(ERROR::EXPECTING_RPAREN, value);
        }
    }
    this->ProcId(false, SymbolType::PARAM);
    while (!lexer.END) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::COMMA) { 
            this->ProcId(false, SymbolType::PARAM);
        }
        else if (type == TokenType::IDENTIFIER) {
            lexer.Keep();
            lexer.Err(ERROR::MISSING_COMMA, value);
            this->ProcId(false, SymbolType::PARAM);
        }
        else if (type == TokenType::RPAREN) {
            return;
        }
        else if (type == TokenType::SEMICOLON) {
            lexer.Err(ERROR::MISSING_RPAREN, value);
            lexer.Keep();
            return;
        }
        else {
            lexer.Err(ERROR::EXPECTING_RPAREN, value);
        }
    }
}

//<body> → begin <statement>{;<statement>}end
void Parser::ProcBody() {
    this->ProcMultiStatement();
}

/*
<statement> → <id> := <exp>
               |if <lexp> then <statement>[else <statement>]
               |while <lexp> do <statement>
               |call <id>（[<exp>{,<exp>}]）
               |<body>
               |read (<id>{，<id>})
               |write (<exp>{,<exp>})
*/
void Parser::ProcStatement() {
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        switch (type)
        {
        case TokenType::IDENTIFIER:
            this->ProcAssign();
            return;
        case TokenType::IF:
            this->ProcIf();
            return;
        case TokenType::WHILE:
            this->ProcWhile();
            return;
        case TokenType::CALL:
            this->ProcCall();
            return;
        case TokenType::BEGIN:
            this->ProcBody();
            return;
        case TokenType::READ:
            this->ProcRead();
            return;
        case TokenType::WRITE:
            this->ProcWrite();
            return;
        case TokenType::SEMICOLON:
            lexer.Keep();
            return;
        case TokenType::END:
        case TokenType::ELSE:
        case TokenType::DO:
        case TokenType::THEN:
            lexer.Err(ERROR::MISSING_STATEMENT, value);
            lexer.Keep();
            return;
        default:
            lexer.Err(ERROR::EXPECTING_STATEMENT, value);
        }
    }
}

void Parser::ProcMultiStatement() {
    this->ProcStatement();
    while (!lexer.END) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::SEMICOLON) {         
            this->ProcStatement();
        }
        else if (type == TokenType::READ || type == TokenType::BEGIN || 
                type == TokenType::CALL || type == TokenType::WRITE || 
                type == TokenType::WHILE || type == TokenType::IF) {
                    lexer.Keep();
                    lexer.Err(ERROR::MISSING_SEMICOLON, value);
                    this->ProcStatement();
                }
        else if (type == TokenType::END) {
            return;
        }
        else {
            lexer.Err(ERROR::EXPECTING_END, value);
        }
    }
}

//<lexp> → <exp> <lop> <exp>|odd <exp>
void Parser::ProcLexp() {
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        switch (type)
        {
        case TokenType::ODD:
            this->ProcExp();
            return;
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::IDENTIFIER:
        case TokenType::NUMBER:
        case TokenType::LPAREN:
            lexer.Keep();
            this->ProcExp();
            this->ProcLop();
            this->ProcExp();
            return;
        case TokenType::THEN:
        case TokenType::DO:
            lexer.Keep();
            lexer.Err(ERROR::MISSING_LEXPR, value);
            return;
        case TokenType::END:
        case TokenType::SEMICOLON:
        case TokenType::BEGIN:
        case TokenType::IF:
        case TokenType::READ:
        case TokenType::WRITE:
        case TokenType::CALL:
        case TokenType::WHILE:
            lexer.Keep();
            lexer.Err(ERROR::MISSING_STATEMENT, value);
            return;
        default:
            lexer.Err(ERROR::EXPECTING_EXPR, value);
            lexer.Err(ERROR::EXPECTING_ODD, value);
        }
    }
}

//<exp> → [+|-]<term>{<aop><term>}
void Parser::ProcExp() {
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        switch (type) {
        case TokenType::IDENTIFIER:
        case TokenType::NUMBER:
        case TokenType::LPAREN:
            lexer.Keep();
        case TokenType::PLUS:
        case TokenType::MINUS:
            this->ProcTerm();
            break;
        case TokenType::ERROR:
            lexer.Err(ERROR::EXPECTING_EXPR, value);
            break;
        default:
            lexer.Keep();
            return;
        }
    }
}

//<term> -> <term>{<aop><term>}
void Parser::ProcTerm() {
    this->ProcFactor();
    while (!lexer.END) {                      
        auto [type, value] = lexer.getToken();
        switch (type) {
        case TokenType::PLUS:
        case TokenType::MINUS:
            this->ProcTerm();
            break;
        case TokenType::IDENTIFIER:
        case TokenType::NUMBER:
        case TokenType::LPAREN:
            lexer.Keep();
            lexer.Err(ERROR::MISSING_ADDOPERATOR, value);
            this->ProcTerm();
            break;
        case TokenType::ERROR:
            lexer.Err(ERROR::EXPECTING_ADDOPERATOR, value);
            lexer.Keep();
            return;
        default:
            lexer.Keep();
            return;
        }
    }
}

//<factor>→<id>|<integer>|(<exp>)
void Parser::ProcFactor() {
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::IDENTIFIER) {
            lexer.Keep();
            this->ProcId(false, SymbolType::NONE);
            break;
        }
        else if (type == TokenType::NUMBER) {
            lexer.Keep();
            this->ProcInteger();
            break;
        }
        else if (type == TokenType::LPAREN) {
            this->ProcExp();
            while (!lexer.END) {
                auto [type, value] = lexer.getToken();
                if (type == TokenType::ENDOFFILE) return;
                if (type == TokenType::RPAREN) {
                    break;
                }
                else if (type == TokenType::ERROR){
                    lexer.Err(ERROR::EXPECTING_RPAREN, value);
                }
                else {
                    lexer.Err(ERROR::MISSING_RPAREN, value);
                    lexer.Keep();
                    return;
                }
            }
            break;
        }
        else {
            lexer.Err(ERROR::EXPECTING_FACTOR, value);
        }
    }
    while (!lexer.END) {                      
        auto [type, value] = lexer.getToken();
        switch (type) {
        case TokenType::TIMES:
        case TokenType::SLASH:
            this->ProcFactor();
            break;
        case TokenType::IDENTIFIER:
        case TokenType::NUMBER:
        case TokenType::LPAREN:
            lexer.Keep();
            lexer.Err(ERROR::MISSING_MULOPERATOR, value);
            this->ProcFactor();
            break;
        case TokenType::ERROR:
            lexer.Err(ERROR::EXPECTING_MULOPERATOR, value);
            break;
        default:
            lexer.Keep();
            return;
        }
    }
}

//<lop> → =|<>|<|<=|>|>=
void Parser::ProcLop() {
    while (!lexer.END) {                      
        auto [type, value] = lexer.getToken();
        switch (type) {
        case TokenType::EQU:
        case TokenType::NEQ:
        case TokenType::LES:
        case TokenType::LEQ:
        case TokenType::GTR:
        case TokenType::GEQ:
            return;
        default:
            lexer.Keep();
            lexer.Err(ERROR::EXPECTING_CMPOPERATOR, value);
            return;
        }
    }
}

//<id> → l{l|d}
void Parser::ProcId(bool decl, SymbolType Stype) {
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::IDENTIFIER) {
            //TODO
            if (decl) {
                switch (Stype) {
                case SymbolType::CONST:

                    break;
                case SymbolType::PARAM:

                    break;
                case SymbolType::PROCEDURE:

                    break;
                case SymbolType::PROGRAM:

                    break;
                case SymbolType::VAR:

                    break;
                case SymbolType::NONE:

                    break;
                }
            }
            else {
                switch (Stype) {
                case SymbolType::CONST:

                    break;
                case SymbolType::PARAM:

                    break;
                case SymbolType::PROCEDURE:

                    break;
                case SymbolType::PROGRAM:

                    break;
                case SymbolType::VAR:

                    break;
                case SymbolType::NONE:

                    break;
                }
            }
            return;
        }
        else if (type == TokenType::COMMA || type == TokenType::SEMICOLON || type == TokenType::LPAREN) {
            lexer.Err(ERROR::MISSING_IDENTIFIER, value);
            lexer.Keep();
            return;
        }
        else if (type == TokenType::ERROR) {
            continue;
        }
        else if (isKeyword(value->name) != TokenType::ERROR) {
            lexer.Err(ERROR::EXPECTING_IDENTIFIER, value);
            lexer.Keep();
            return;
        }
        else {
            lexer.Err(ERROR::EXPECTING_IDENTIFIER, value);
        }
    }
}

//<integer> → d{d}
void Parser::ProcInteger() {
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::NUMBER) {
            //TODO
            int val = 0;
            try {
                val = std::stoi(value->name);
            }
            catch (std::out_of_range const& ex) {
                lexer.Err(ERROR::OUT_OF_RANGE, value);
                val = 0;
            }
            return;
        }
        else if (type == TokenType::COMMA || type == TokenType::SEMICOLON) {
            lexer.Err(ERROR::MISSING_INTEGER, value);
            lexer.Keep();
            return;
        }
        else if (isKeyword(value->name) != TokenType::ERROR) {
            lexer.Keep();
            return;
        }
        else {
            lexer.Err(ERROR::EXPECTING_INTEGER, value);
        }
    }
}

//if <lexp> then <statement>[else <statement>]
void Parser::ProcIf() {
    this->ProcLexp();

    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::THEN) {
            this->ProcStatement();
            break;
        }
        else if (type == TokenType::ELSE) {
            lexer.Err(ERROR::MISSING_THEN, value);
            lexer.Err(ERROR::MISSING_STATEMENT, value);
            lexer.Keep();
            break;
        }
        else if (type == TokenType::IDENTIFIER || type == TokenType::IF || type == TokenType::WHILE || type == TokenType::CALL
                || type == TokenType::BEGIN || type == TokenType::READ || type == TokenType::WRITE) {
            lexer.Keep();
            lexer.Err(ERROR::MISSING_THEN, value);
            this->ProcStatement();
            break;
        }
        else {
            lexer.Err(ERROR::EXPECTING_THEN, value);
        }
    }
    
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ELSE) {
            this->ProcStatement();
            return;
        }
        else if (type == TokenType::SEMICOLON || type == TokenType::END) {
            lexer.Keep();
            return;
        }
        else if (type == TokenType::IDENTIFIER || type == TokenType::IF || type == TokenType::WHILE || type == TokenType::CALL
                || type == TokenType::BEGIN || type == TokenType::READ || type == TokenType::WRITE) {
            lexer.Keep();
            lexer.Err(ERROR::MISSING_ELSE, value);
            break;
        }
        else {
            lexer.Err(ERROR::UNEXPECTED_TOKEN, value);
        }
    }
}

//while <lexp> do <statement>
void Parser::ProcWhile() {
    this->ProcLexp();

    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::DO) {
            break;
        }
        else if (type == TokenType::SEMICOLON || type == TokenType::END) {
            lexer.Keep();
            lexer.Err(ERROR::MISSING_DO, value);
            lexer.Err(ERROR::MISSING_STATEMENT, value);
            return;
        }
        else if (type == TokenType::IDENTIFIER || type == TokenType::IF || type == TokenType::WHILE || type == TokenType::CALL
                || type == TokenType::BEGIN || type == TokenType::READ || type == TokenType::WRITE) {
            lexer.Keep();
            lexer.Err(ERROR::MISSING_DO, value);
            break;
        }
        else {
            lexer.Err(ERROR::EXPECTING_DO, value);
        }
    }

    this->ProcStatement();
}

//call <id>（[<exp>{,<exp>}]）
void Parser::ProcCall() {
    this->ProcId(false, SymbolType::PROCEDURE);

    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::LPAREN) {
            break;
        }
        else if (type == TokenType::SEMICOLON || type == TokenType::END) {
            lexer.Err(ERROR::MISSING_LPAREN, value);
            lexer.Keep();
            break;
        }
        else {
            lexer.Err(ERROR::EXPECTING_LPAREN, value);
        }
    }


    this->ProcCallParam();
}

//[<exp>{,<exp>}])
void Parser::ProcCallParam() {
    this->ProcExp();
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::COMMA) {
            this->ProcExp();
        }
        else if (type == TokenType::RPAREN) {
            break;
        }
        else if (type == TokenType::SEMICOLON || type == TokenType::END) {
            lexer.Err(ERROR::MISSING_RPAREN, value);
            lexer.Keep();
            break;
        }
        else {
            lexer.Err(ERROR::EXPECTING_RPAREN, value);
        }
    }

}

void Parser::ProcRead() {
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::LPAREN) {
            break;
        }
        else if (type == TokenType::SEMICOLON || type == TokenType::END) {
            lexer.Err(ERROR::MISSING_LPAREN, value);
            lexer.Keep();
            break;
        }
        else {
            lexer.Err(ERROR::EXPECTING_LPAREN, value);
        }
    }
    this->ProcReadParam();
}

//<id>{，<id>})
void Parser::ProcReadParam() {
    this->ProcId(false, SymbolType::VAR);
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::COMMA) {
            this->ProcId(false, SymbolType::VAR);
        }
        else if (type == TokenType::RPAREN) {
            break;
        }
        else if (type == TokenType::SEMICOLON || type == TokenType::END) {
            lexer.Err(ERROR::MISSING_RPAREN, value);
            lexer.Keep();
            break;
        }
        else {
            lexer.Err(ERROR::EXPECTING_RPAREN, value);
        }
    }
}

//write (<exp>{,<exp>})
void Parser::ProcWrite() {
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::LPAREN) {
            break;
        }
        else if (type == TokenType::SEMICOLON || type == TokenType::END) {
            lexer.Err(ERROR::MISSING_LPAREN, value);
            lexer.Keep();
            break;
        }
        else {
            lexer.Err(ERROR::EXPECTING_LPAREN, value);
        }
    }
    this->ProcWriteParam();
}

//<exp>{,<exp>})
void Parser::ProcWriteParam() {
    this->ProcExp();
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::COMMA) {
            this->ProcExp();
        }
        else if (type == TokenType::RPAREN) {
            break;
        }
        else if (type == TokenType::SEMICOLON || type == TokenType::END) {
            lexer.Err(ERROR::MISSING_RPAREN, value);
            lexer.Keep();
            break;
        }
        else {
            lexer.Err(ERROR::EXPECTING_RPAREN, value);
        }
    }
}

void Parser::ProcAssign() {
    this->ProcBecomes();
    this->ProcExp();
}

void Parser::ProcBecomes() {
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::BECOMES) {
            return;
        }
        else if (type == TokenType::EQU) {
            lexer.Err(ERROR::MISSING_COLON, value);
            return;
        }
        else {
            lexer.Err(ERROR::MISSING_BECOMES, value);
            lexer.Keep();
            return;
        }
    }
}

void Parser::loadFile(const char* FilePath){
    lexer.LoadFile(FilePath);
}

void Parser::analyze(){
    /*TODO*/
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::PROGRAM) {
            ProcProg();
            break;
        }
        else lexer.Err(ERROR::EXPECTING_PROGRAM, value);
    }
    lexer.WriteFile();
}

Parser::Parser()
{
}

Parser::~Parser()
{
}