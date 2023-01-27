#include "PL0Parser.h"
#include <string.h>

void Parser::ProcError(SYNERROR type, std::unordered_set<Token, KeyHash, Equal>::iterator it){
    int row = it->row, col = it->col;
    char errornamebf[64];
    switch (type) {
        case SYNERROR::EXPECTING_PROGRAM:
            strcpy(errornamebf, "EXPECTING_PROGRAM: ");
            break;
        case SYNERROR::EXPECTING_SEMICOLON:
            strcpy(errornamebf, "EXPECTING_SEMICOLON: ");
            break;
        case SYNERROR::EXPECTING_BEGIN:
            strcpy(errornamebf, "EXPECTING_BEGIN: ");
            break;
        case SYNERROR::EXPECTING_END:
            strcpy(errornamebf, "EXPECTING_END: ");
            break;
        case SYNERROR::EXPECTING_CONST:
            strcpy(errornamebf, "EXPECTING_CONST: ");
            break;
        case SYNERROR::EXPECTING_IDENTIFIER:
            strcpy(errornamebf, "EXPECTING_IDENTIFIER: ");
            break;
        case SYNERROR::EXPECTING_INTEGER:
            strcpy(errornamebf, "EXPECTING_INTEGER: ");
            break;
        case SYNERROR::EXPECTING_LPAREN:
            strcpy(errornamebf, "EXPECTING_LPAREN: ");
            break;
        case SYNERROR::EXPECTING_RPAREN:
            strcpy(errornamebf, "EXPECTING_RPAREN: ");
            break;
        case SYNERROR::EXPECTING_STATEMENT:
            strcpy(errornamebf, "EXPECTING_STATEMENT: ");
            break;
        case SYNERROR::EXPECTING_CMPOPERATOR:
            strcpy(errornamebf, "EXPECTING_CMPOPERATOR: ");
            break;
        case SYNERROR::EXPECTING_ADDOPERATOR:
            strcpy(errornamebf, "EXPECTING_ADDOPERATOR: ");
            break;
        case SYNERROR::EXPECTING_MULOPERATOR:
            strcpy(errornamebf, "EXPECTING_MULOPERATOR: ");
            break;
        case SYNERROR::EXPECTING_THEN:
            strcpy(errornamebf, "EXPECTING_THEN: ");
            break;
        case SYNERROR::EXPECTING_ODD:
            strcpy(errornamebf, "EXPECTING_ODD: ");
            break;
        case SYNERROR::EXPECTING_EXPR:
            strcpy(errornamebf, "EXPECTING_EXPR: ");
            break;
        case SYNERROR::EXPECTING_DO:
            strcpy(errornamebf, "EXPECTING_DO: ");
            break;
        case SYNERROR::MISSING_SEMICOLON:
            strcpy(errornamebf, "MISSING_SEMICOLON: ");
            break;
        case SYNERROR::MISSING_COMMA:
            strcpy(errornamebf, "MISSING_COMMA: ");
            break;
        case SYNERROR::MISSING_IDENTIFIER:
            strcpy(errornamebf, "MISSING_IDENTIFIER: ");
            break;
        case SYNERROR::MISSING_INTEGER:
            strcpy(errornamebf, "MISSING_INTEGER: ");
            break;
        case SYNERROR::MISSING_STATEMENT:
            strcpy(errornamebf, "MISSING_STATEMENT: ");
            break;
        case SYNERROR::MISSING_CMPOPERATOR:
            strcpy(errornamebf, "MISSING_CMPOPERATOR: ");
            break;
        case SYNERROR::MISSING_ADDOPERATOR:
            strcpy(errornamebf, "MISSING_ADDOPERATOR: ");
            break;
        case SYNERROR::MISSING_MULOPERATOR:
            strcpy(errornamebf, "MISSING_MULOPERATOR: ");
            break;
        case SYNERROR::MISSING_LEXPR:
            strcpy(errornamebf, "MISSING_LEXPR: ");
            break;
        case SYNERROR::MISSING_EXPR:
            strcpy(errornamebf, "MISSING_EXPR: ");
            break;
        case SYNERROR::MISSING_LPAREN:
            strcpy(errornamebf, "MISSING_LPAREN: ");
            break;
        case SYNERROR::MISSING_RPAREN:
            strcpy(errornamebf, "MISSING_RPAREN: ");
            break;
        case SYNERROR::UNEXPECTED_TOKEN:
            strcpy(errornamebf, "UNEXPECTED_TOKEN: ");
            break;
    }
    printf("ERROR::PARSER::%-24s%-16s [行 %-3d, 列 %-3d]\n", errornamebf, it->name.c_str(), row, col);
}

//<prog> → program <id>；<block>
void Parser::ProcProg() {
    this->ProcId(true, SymbolType::PROGRAM);                             //<id>
    while (!lexer.End()) {                      //;
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
        if (type != TokenType::SEMICOLON) {
            this->ProcError(SYNERROR::EXPECTING_SEMICOLON, value);
            continue;
        }
        else break;
    }
    this->ProcBlock();                          //<block>
}

//<block> → [<condecl>][<vardecl>][<proc>]<body>
void Parser::ProcBlock() {
    while (!lexer.End()) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
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
            this->ProcError(SYNERROR::EXPECTING_BEGIN, value);
            break;
        }
    }
}

//<condecl> → const <const>{,<const>};
void Parser::ProcCondecl() {
    this->ProcConst();

    while (!lexer.End()) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
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
            this->ProcError(SYNERROR::EXPECTING_BEGIN, value);
        }
    }
}

//<const> → <id>:=<integer>
void Parser::ProcConst() {
    this->ProcId(true, SymbolType::CONST);

    this->ProcBecomes();
    
    this->ProcInteger();

    while (!lexer.End()) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
        if (type == TokenType::COMMA) {         //<integer>
            this->ProcConst();
            return;
        }
        else if (type == TokenType::SEMICOLON) {
            return;
        }
        else {
            this->ProcError(SYNERROR::EXPECTING_SEMICOLON, value);
        }
    }
}

//<vardecl> → var <id>{,<id>};
void Parser::ProcVardecl() {
    this->ProcVar();

    while (!lexer.End()) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;

        if (type == TokenType::PROCEDURE) {//[<proc>]
            this->ProcProc();
            break;
        }
        else if (type == TokenType::BEGIN) {
            this->ProcBody();
            break;
        }
        else {
            this->ProcError(SYNERROR::EXPECTING_BEGIN, value);
        }
    }
}

//<id>{,<id>};
void Parser::ProcVar() {
    this->ProcId(true, SymbolType::VAR);
    while (!lexer.End()) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
        if (type == TokenType::COMMA) {         //var
            this->ProcId(true, SymbolType::VAR);
        }
        else if (type == TokenType::IDENTIFIER) {
            lexer.Keep();
            this->ProcError(SYNERROR::MISSING_COMMA, value);
            this->ProcId(true, SymbolType::VAR);
        }
        else if (type == TokenType::SEMICOLON) {
            return;
        }
        else if (type != TokenType::ERROR && isKeyword(value->name) != TokenType::ERROR){
            lexer.Keep();
            return;
        }
        else {
            this->ProcError(SYNERROR::EXPECTING_SEMICOLON, value);
        }
    }
}

//<proc> → procedure <id>（[<id>{,<id>}]）;<block>{;<proc>}
void Parser::ProcProc() {
    this->ProcId(true, SymbolType::PROCEDURE);

    while (!lexer.End()) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
        if (type == TokenType::LPAREN) {         //(
            break;
        }
        else if (type == TokenType::SEMICOLON || type == TokenType::IDENTIFIER) {
            this->ProcError(SYNERROR::MISSING_LPAREN, value);
            lexer.Keep();
            break;
        }
        else {
            this->ProcError(SYNERROR::EXPECTING_LPAREN, value);
        }
    }

    this->ProcProcParam();
    while (!lexer.End()) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
        if (type == TokenType::SEMICOLON) {         //;
            break;
        }
        else {
            this->ProcError(SYNERROR::EXPECTING_SEMICOLON, value);
        }
    }

    this->ProcBlock();                              //<block>

    while (!lexer.End()) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
        if (type == TokenType::SEMICOLON) {         //{;<proc>}
            this->ProcProc();
            break;
        }
        else if (type == TokenType::BEGIN) {
            lexer.Keep();
            this->ProcBody();
            return;
        }
        else {
            this->ProcError(SYNERROR::EXPECTING_BEGIN, value);
        }
    }
}

// [<id>{,<id>}])
void Parser::ProcProcParam() {
    while (!lexer.End()) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
        if (type == TokenType::IDENTIFIER) { 
            lexer.Keep();
            this->ProcId(false, SymbolType::PARAM);
            break;
        }
        else if (type == TokenType::RPAREN) {
            return;
        }
        else if (type == TokenType::SEMICOLON) {
            this->ProcError(SYNERROR::MISSING_RPAREN, value);
            lexer.Keep();
            return;
        }
        else {
            this->ProcError(SYNERROR::EXPECTING_RPAREN, value);
        }
    }
    while (!lexer.End()) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
        if (type == TokenType::COMMA) {         
            this->ProcProcParam();
            return;
        }
        else if (type == TokenType::RPAREN) {
            return;
        }
        else {
            this->ProcError(SYNERROR::EXPECTING_RPAREN, value);
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
    while (!lexer.End()) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
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
            this->ProcError(SYNERROR::MISSING_STATEMENT, value);
            lexer.Keep();
            return;
        default:
            this->ProcError(SYNERROR::EXPECTING_STATEMENT, value);
        }
    }
}

void Parser::ProcMultiStatement() {
    this->ProcStatement();
    while (!lexer.End()) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
        if (type == TokenType::SEMICOLON) {         
            this->ProcStatement();
        }
        else if (type == TokenType::READ || type == TokenType::BEGIN || 
                type == TokenType::CALL || type == TokenType::WRITE || 
                type == TokenType::WHILE || type == TokenType::IF) {
                    lexer.Keep();
                    this->ProcError(SYNERROR::MISSING_SEMICOLON, value);
                    this->ProcStatement();
                }
        else if (type == TokenType::END) {
            return;
        }
        else {
            this->ProcError(SYNERROR::EXPECTING_END, value);
        }
    }
}

//<lexp> → <exp> <lop> <exp>|odd <exp>
void Parser::ProcLexp() {
    while (!lexer.End()) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
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
        case TokenType::END:
        case TokenType::SEMICOLON:
            lexer.Keep();
            this->ProcError(SYNERROR::MISSING_LEXPR, value);
            return;
        case TokenType::BEGIN:
        case TokenType::IF:
        case TokenType::READ:
        case TokenType::WRITE:
        case TokenType::CALL:
        case TokenType::WHILE:
            lexer.Keep();
            this->ProcError(SYNERROR::MISSING_STATEMENT, value);
            return;
        default:
            this->ProcError(SYNERROR::EXPECTING_EXPR, value);
            this->ProcError(SYNERROR::EXPECTING_ODD, value);
        }
    }
}

//<exp> → [+|-]<term>{<aop><term>}
void Parser::ProcExp() {
    while (!lexer.End()) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
        switch (type) {
        case TokenType::ERROR:
            this->ProcError(SYNERROR::EXPECTING_EXPR, value);
            break;
        case TokenType::IDENTIFIER:
        case TokenType::NUMBER:
        case TokenType::LPAREN:
            lexer.Keep();
        case TokenType::PLUS:
        case TokenType::MINUS:
            this->ProcTerm();
            return;
        default:
            lexer.Keep();
            return;
        }
    }
}

//<term>{<aop><term>}
void Parser::ProcTerm() {
    this->ProcFactor();
    while (!lexer.End()) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
        switch (type) {
        case TokenType::PLUS:
        case TokenType::MINUS:
            this->ProcTerm();
            return;
        case TokenType::IDENTIFIER:
        case TokenType::NUMBER:
        case TokenType::LPAREN:
            lexer.Keep();
            this->ProcError(SYNERROR::MISSING_ADDOPERATOR, value);
            this->ProcTerm();
            return;
        default:
            lexer.Keep();
            return;
        }
    }
}

//<factor>→<id>|<integer>|(<exp>)
void Parser::ProcFactor() {
    while (!lexer.End()) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
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
            while (!lexer.End()) {
                auto [type, value] = lexer.getToken();
                if (type == TokenType::ENDOFFILE) return;
                if (type == TokenType::RPAREN) {
                    break;
                }
                else if (type == TokenType::ERROR){
                    this->ProcError(SYNERROR::EXPECTING_RPAREN, value);
                }
                else {
                    this->ProcError(SYNERROR::MISSING_RPAREN, value);
                    lexer.Keep();
                    return;
                }
            }
            break;
        }
        else {
            this->ProcError(SYNERROR::EXPECTING_IDENTIFIER, value);
            this->ProcError(SYNERROR::EXPECTING_INTEGER, value);
            this->ProcError(SYNERROR::EXPECTING_LPAREN, value);
        }
    }
    while (!lexer.End()) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
        switch (type) {
        case TokenType::TIMES:
        case TokenType::SLASH:
            this->ProcFactor();
            return;
        case TokenType::IDENTIFIER:
        case TokenType::NUMBER:
        case TokenType::LPAREN:
            lexer.Keep();
            return;
            /*this->ProcError(SYNERROR::MISSING_MULOPERATOR, value);
            lexer.Keep();
            this->ProcFactor();*/
            return;
        case TokenType::ERROR:
            this->ProcError(SYNERROR::EXPECTING_MULOPERATOR, value);
            break;
        default:
            lexer.Keep();
            return;
        }
    }
}

//<lop> → =|<>|<|<=|>|>=
void Parser::ProcLop() {
    while (!lexer.End()) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
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
            this->ProcError(SYNERROR::EXPECTING_CMPOPERATOR, value);
            return;
        }
    }
}

//<id> → l{l|d}
void Parser::ProcId(bool decl, SymbolType Stype) {
    while (!lexer.End()) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
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
            this->ProcError(SYNERROR::MISSING_IDENTIFIER, value);
            lexer.Keep();
            return;
        }
        else if (type == TokenType::ERROR) {
            this->ProcError(SYNERROR::EXPECTING_IDENTIFIER, value);
        }
        else if (isKeyword(value->name) != TokenType::ERROR) {
            lexer.Keep();
            return;
        }
        else {
            this->ProcError(SYNERROR::EXPECTING_IDENTIFIER, value);
        }
    }
}

//<integer> → d{d}
void Parser::ProcInteger() {
    while (!lexer.End()) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
        if (type == TokenType::NUMBER) {
            //TODO
            return;
        }
        else if (type == TokenType::COMMA || type == TokenType::SEMICOLON) {
            this->ProcError(SYNERROR::MISSING_INTEGER, value);
            lexer.Keep();
            return;
        }
        else if (isKeyword(value->name) != TokenType::ERROR) {
            lexer.Keep();
            return;
        }
        else {
            this->ProcError(SYNERROR::EXPECTING_INTEGER, value);
        }
    }
}

//if <lexp> then <statement>[else <statement>]
void Parser::ProcIf() {
    this->ProcLexp();

    while (!lexer.End()) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
        if (type == TokenType::THEN) {
            break;
        }
        else {
            this->ProcError(SYNERROR::EXPECTING_THEN, value);
        }
    }

    this->ProcStatement();
    
    if (!lexer.End()) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
        if (type == TokenType::ELSE) {
            this->ProcStatement();
        }
        else if (type == TokenType::SEMICOLON || type == TokenType::END) {
            lexer.Keep();
            return;
        }
        else {
            this->ProcError(SYNERROR::UNEXPECTED_TOKEN, value);
        }
    }
}

//while <lexp> do <statement>
void Parser::ProcWhile() {
    this->ProcLexp();

    while (!lexer.End()) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
        if (type == TokenType::DO) {
            break;
        }
        else if (type != TokenType::ERROR && isKeyword(value->name) != TokenType::ERROR) {
            lexer.Keep();
            return;
        }
        else if (type == TokenType::SEMICOLON || type == TokenType::END) {
            lexer.Keep();
            return;
        }
        else {
            this->ProcError(SYNERROR::EXPECTING_DO, value);
        }
    }

    this->ProcStatement();
}

//call <id>（[<exp>{,<exp>}]）
void Parser::ProcCall() {
    this->ProcId(false, SymbolType::PROCEDURE);

    while (!lexer.End()) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
        if (type == TokenType::LPAREN) {
            break;
        }
        else if (type == TokenType::SEMICOLON || type == TokenType::END) {
            this->ProcError(SYNERROR::MISSING_LPAREN, value);
            lexer.Keep();
            break;
        }
        else {
            this->ProcError(SYNERROR::EXPECTING_LPAREN, value);
        }
    }


    this->ProcCallParam();
}

//[<exp>{,<exp>}])
void Parser::ProcCallParam() {
    this->ProcExp();
    while (!lexer.End()) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
        if (type == TokenType::COMMA) {
            this->ProcExp();
        }
        else if (type == TokenType::RPAREN) {
            break;
        }
        else if (type == TokenType::SEMICOLON || type == TokenType::END) {
            this->ProcError(SYNERROR::MISSING_RPAREN, value);
            lexer.Keep();
            break;
        }
        else {
            this->ProcError(SYNERROR::EXPECTING_RPAREN, value);
        }
    }

}

void Parser::ProcRead() {
    while (!lexer.End()) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
        if (type == TokenType::LPAREN) {
            break;
        }
        else if (type == TokenType::SEMICOLON || type == TokenType::END) {
            this->ProcError(SYNERROR::MISSING_LPAREN, value);
            lexer.Keep();
            break;
        }
        else {
            this->ProcError(SYNERROR::EXPECTING_LPAREN, value);
        }
    }
    this->ProcReadParam();
}

//<id>{，<id>})
void Parser::ProcReadParam() {
    this->ProcId(false, SymbolType::VAR);
    while (!lexer.End()) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
        if (type == TokenType::COMMA) {
            this->ProcId(false, SymbolType::VAR);
        }
        else if (type == TokenType::RPAREN) {
            break;
        }
        else if (type == TokenType::SEMICOLON || type == TokenType::END) {
            this->ProcError(SYNERROR::MISSING_RPAREN, value);
            lexer.Keep();
            break;
        }
        else {
            this->ProcError(SYNERROR::EXPECTING_RPAREN, value);
        }
    }
}

//write (<exp>{,<exp>})
void Parser::ProcWrite() {
    while (!lexer.End()) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
        if (type == TokenType::LPAREN) {
            break;
        }
        else if (type == TokenType::SEMICOLON || type == TokenType::END) {
            this->ProcError(SYNERROR::MISSING_LPAREN, value);
            lexer.Keep();
            break;
        }
        else {
            this->ProcError(SYNERROR::EXPECTING_LPAREN, value);
        }
    }
    this->ProcWriteParam();
}

//<exp>{,<exp>})
void Parser::ProcWriteParam() {
    this->ProcExp();
    while (!lexer.End()) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
        if (type == TokenType::COMMA) {
            this->ProcExp();
        }
        else if (type == TokenType::RPAREN) {
            break;
        }
        else if (type == TokenType::SEMICOLON || type == TokenType::END) {
            this->ProcError(SYNERROR::MISSING_RPAREN, value);
            lexer.Keep();
            break;
        }
        else {
            this->ProcError(SYNERROR::EXPECTING_RPAREN, value);
        }
    }
}

void Parser::ProcAssign() {
    this->ProcBecomes();
    this->ProcExp();
}

void Parser::ProcBecomes() {
    while (!lexer.End()) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
        if (type == TokenType::BECOMES || value->name == ":") {
            break;
        }
        else if (type == TokenType::EQU) {
            this->ProcError(SYNERROR::UNEXPECTED_TOKEN, value);
            break;
        }
        else {
            this->ProcError(SYNERROR::EXPECTING_EXPR, value);
        }
    }
}

void Parser::loadFile(const char* FilePath){
    lexer.LoadFile(FilePath);
}

void Parser::analyze(){
    /*TODO*/
    while (!lexer.End()) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
        if (type == TokenType::PROGRAM) {
            ProcProg();
            break;
        }
        else this->ProcError(SYNERROR::EXPECTING_PROGRAM, value);
    }
    lexer.WriteFile();
}

Parser::Parser()
{
}

Parser::~Parser()
{
}