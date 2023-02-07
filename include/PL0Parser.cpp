#include "PL0Parser.h"
#include "Code.h"
#include <string.h>
#include <algorithm>
#include <fstream>
#include "ConsoleColor.h"
constexpr int initAllocateSize = 3;
constexpr int maxNestLevel = 4;

//<prog> → program <id>；<block>
void Parser::ProcProg() {
    int allocateSize = initAllocateSize;
    std::string name = this->ProcId(0, true, SymbolType::PROGRAM, allocateSize);                             //<id>
    while (!lexer.END) {                      //;
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) return;
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
    this->ProcBlock(name, 0, allocateSize);                          //<block>
}

//<block> → [<condecl>][<vardecl>][<proc>]<body>
void Parser::ProcBlock(const std::string& name, int level, int &offset) {
    int Cx = Code.size();
    gen(oprType::JMP, 0, 0);                                        //1
    int parameterNum = offset - initAllocateSize;
    int allocateSize = initAllocateSize;
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) break;
        switch (type)
        {
        case TokenType::CONST:
            this->ProcCondecl(level, offset);
            break;
        case TokenType::VAR:
            this->ProcVardecl(level, offset);
            break;
        case TokenType::PROCEDURE:
            if (level > maxNestLevel) {
                lexer.Err(ERROR::EXCEED_MAX_NEST_LEVEL, value);
            }
            this->ProcProc(level, allocateSize);
            break;
        case TokenType::READ:
        case TokenType::IDENTIFIER:
        case TokenType::CALL:
        case TokenType::WRITE:
        case TokenType::WHILE:
        case TokenType::IF:
        case TokenType::END:
            lexer.Keep();
            lexer.Err(ERROR::MISSING_BEGIN, value);
            [[fallthrough]];
        case TokenType::BEGIN:
            if (level != 0 && !stk.empty()) {
                auto [_, symbol] = stk.end()[-2]->search(name);
                symbol->size = offset;
                //for (int i = 0; i < parameterNum; ++i)
                //    gen(oprType::POP, 0, 0);
                Code[Cx].offset = Code.size();
                gen(oprType::INT, 0, offset);               
                /*
                cx                              jmp
                [cx+1,cx+paramnum]              pop           
                cx+paramnum+1                   int 
                cx+paramnum+2                   code.size
                */
                symbol->val = parameterNum;
                symbol->address = Code.size() - 1;
            }
            else {
                //for (int i = 0; i < parameterNum; ++i)
                //    gen(oprType::POP, 0, 0);
                Code[Cx].offset = Code.size();
                gen(oprType::INT, 0, offset);     
            }
            this->ProcBody(level);
            gen(oprType::OPR, 0, static_cast<int>(oprCode::RET));
            return;
        default:
            lexer.Err(ERROR::EXPECTING_BEGIN, value);
            break;
        }
    }

}

//<condecl> → const <const>{,<const>};
//<const> → <id>:=<integer>
void Parser::ProcCondecl(int level, int &offset) {
    this->ProcId(level, true, SymbolType::CONST, offset);
    while (!lexer.END) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::COMMA) {         //<integer>
            this->ProcCondecl(level, offset);
            return;
        }
        else if (type == TokenType::IDENTIFIER) {
            lexer.Keep();
            lexer.Err(ERROR::MISSING_COMMA, value);
            this->ProcCondecl(level, offset);
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
//<id>{,<id>};
void Parser::ProcVardecl(int level, int &offset) {
    this->ProcId(level, true, SymbolType::VAR, offset);
    while (!lexer.END) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::COMMA) {         //var
            this->ProcId(level, true, SymbolType::VAR, offset);
        }
        else if (type == TokenType::IDENTIFIER) {
            lexer.Keep();
            lexer.Err(ERROR::MISSING_COMMA, value);
            this->ProcId(level, true, SymbolType::VAR, offset);
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
void Parser::ProcProc(int level, int &offset) {
    std::string name = this->ProcId(level, true, SymbolType::PROCEDURE, offset);
    level += 1;
    int allocateSize = initAllocateSize;
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

    this->ProcProcParam(level, allocateSize);
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

    this->ProcBlock(name, level, allocateSize);                              //<block>
    stk.pop_back();
    level -= 1;
    while (!lexer.END) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::SEMICOLON) {         //{;<proc>}
            while (!lexer.END) {
                auto [type, value] = lexer.getToken();
                if (type == TokenType::PROCEDURE) {
                    this->ProcProc(level, offset);
                    return;
                }
                else {
                    lexer.Err(ERROR::EXPECTING_PROCEDURE, value);
                    lexer.Keep();
                    this->ProcProc(level, offset);
                    return;
                }
            }
            return;
        }
        else if (type == TokenType::PROCEDURE) {
            lexer.Keep();
            lexer.Err(ERROR::MISSING_SEMICOLON, value);
            this->ProcProc(level, offset);
            return;
        }
        else {
            lexer.Keep();
            return;
        }
    }
}

// [<id>{,<id>}])
void Parser::ProcProcParam(int level, int &offset) {
    while (!lexer.END) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::RPAREN) {
            return;
        }
        else {
            lexer.Keep();
            break;
        }
    }
    this->ProcId(level, true, SymbolType::VAR, offset);
    while (!lexer.END) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::COMMA) { 
            this->ProcId(level, true, SymbolType::VAR, offset);
        }
        else if (type == TokenType::RPAREN) {
            break;
        }
        else if (type == TokenType::SEMICOLON) {
            lexer.Err(ERROR::MISSING_RPAREN, value);
            lexer.Keep();
            break;
        }
        else if (type == TokenType::CONST || type == TokenType::VAR || type == TokenType::PROCEDURE || type == TokenType::BEGIN) {
            lexer.Keep();
            break;
        }
        else {
            lexer.Err(ERROR::EXPECTING_RPAREN, value);
        }
    }
}

//<body> → begin <statement>{;<statement>}end
void Parser::ProcBody(int level) {
    this->ProcMultiStatement(level);
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
void Parser::ProcStatement(int level) {
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) break;
        switch (type)
        {
        case TokenType::IDENTIFIER:
            lexer.Keep();
            this->ProcAssign(level);
            return;
        case TokenType::IF:
            this->ProcIf(level);
            return;
        case TokenType::WHILE:
            this->ProcWhile(level);
            return;
        case TokenType::CALL:
            this->ProcCall(level);
            return;
        case TokenType::BEGIN:
            this->ProcBody(level);
            return;
        case TokenType::READ:
            this->ProcRead(level);
            return;
        case TokenType::WRITE:
            this->ProcWrite(level);
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

void Parser::ProcMultiStatement(int level) {
    this->ProcStatement(level);
    while (!lexer.END) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::SEMICOLON) {         
            this->ProcStatement(level);
        }
        else if (StatementBegin(type)) {
            lexer.Keep();
            lexer.Err(ERROR::MISSING_SEMICOLON, value);
            this->ProcStatement(level);
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
void Parser::ProcLexp(int level) {
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) break;
        if (ExpBegin(type)) {
            lexer.Keep();
            this->ProcExp(level);
            auto loptype = this->ProcLop();
            this->ProcExp(level);
            auto oprt = oprCode::EQL;
            switch (loptype)
            {
            case TokenType::EQU:
                oprt = oprCode::EQL;
                break;
            case TokenType::NEQ:
                oprt = oprCode::NEQ;
                break;
            case TokenType::LEQ:
                oprt = oprCode::LEQ;
                break;
            case TokenType::LES:
                oprt = oprCode::LES;
                break;
            case TokenType::GEQ:
                oprt = oprCode::GEQ;
                break;
            case TokenType::GTR:
                oprt = oprCode::GTR;
                break;
            default:
                break;
            }
            gen(oprType::OPR, 0, static_cast<int>(oprt));
            return;
        }
        else if (type == TokenType::ODD) {
            this->ProcExp(level);
            gen(oprType::OPR, 0, static_cast<int>(oprCode::ODD));
            return;
        }
        else if (LexpEnd(type)) {
            lexer.Keep();
            lexer.Err(ERROR::MISSING_LEXPR, value);
            return;
        }
        else {
            lexer.Keep();
            return;
        }
    }
}

//<exp> → [+|-]<term>{<aop><term>}
void Parser::ProcExp(int level) {
    auto [type, value] = lexer.getToken();
    if (!addop(type))
        lexer.Keep();
    this->ProcTerm(level);
    if (type == TokenType::MINUS)
        gen(oprType::OPR, 0, static_cast<int>(oprCode::NEG));
    while (!lexer.END) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::PLUS){
            this->ProcTerm(level);
            gen(oprType::OPR, 0, static_cast<int>(oprCode::ADD));
        }
        else if (type == TokenType::MINUS){
            this->ProcTerm(level);
            gen(oprType::OPR, 0, static_cast<int>(oprCode::SUB));
        }
        else if (TermBegin(type)){
            lexer.Keep();
            lexer.Err(ERROR::MISSING_OPERATOR, value);
            this->ProcTerm(level);
        }
        else {
            lexer.Keep();
            break;
        }
    }
}

//<term> -> <factor>{<mop><factor>}
void Parser::ProcTerm(int level) {
    this->ProcFactor(level);
    while (!lexer.END) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::TIMES) {
            this->ProcFactor(level);
            gen(oprType::OPR, 0, static_cast<int>(oprCode::MUL));
        }
        else if (type == TokenType::SLASH) {
            this->ProcFactor(level);
            gen(oprType::OPR, 0, static_cast<int>(oprCode::DIV));
        }
        else if (TermBegin(type)) {
            lexer.Keep();
            lexer.Err(ERROR::MISSING_OPERATOR, value);
            this->ProcFactor(level);
            break;
        }
        else if (TermEnd(type)) {
            lexer.Keep();
            break;
        }
        else if (type == TokenType::ERROR) {
            lexer.Err(ERROR::EXPECTING_OPERATOR, value);
            break;    
        }
        else {
            lexer.Keep();
            break;
        }
    }
}

//<factor>→<id>|<integer>|(<exp>)
void Parser::ProcFactor(int level) {
    int _ = 1;
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) break;
        if (type == TokenType::IDENTIFIER) {
            lexer.Keep();
            this->ProcId(level, false, SymbolType::VAR, _);
            if (last == nullptr) break;
            switch (last->type)
            {
            case SymbolType::PROGRAM:
            case SymbolType::PROCEDURE:
                lexer.Err(ERROR::EXPECTING_VAR_CONST_INTEGER, value);
                break;
            case SymbolType::VAR:
                gen(oprType::LOD, level - last->level, last->address);
                break;
            case SymbolType::CONST:
                gen(oprType::LIT, 0, last->val);
                break;
            default:
                break;
            }
            break;
        }
        else if (type == TokenType::NUMBER) {
            lexer.Keep();
            int val = this->ProcInteger();
            gen(oprType::LIT, 0, val);
            break;
        }
        else if (type == TokenType::LPAREN) {
            this->ProcExp(level);
            while (!lexer.END) {
                auto [type, value] = lexer.getToken();
                if (type == TokenType::RPAREN) {
                    break;
                }
                else if (type == TokenType::ERROR){
                    lexer.Err(ERROR::EXPECTING_RPAREN, value);
                }
                else {
                    lexer.Err(ERROR::MISSING_RPAREN, value);
                    lexer.Keep();
                    break;
                }
            }
            break;
        }
        else if (StatementEnd(type) || StatementBegin(type)) {
            lexer.Keep();
            break;
        }
        else if (FactorEnd(type)) {
            lexer.Keep();
            lexer.Err(ERROR::MISSING_FACTOR, value);
            break;
        }
        else{
            lexer.Err(ERROR::EXPECTING_FACTOR, value);
        }
    }
}

//<lop> → =|<>|<|<=|>|>=
TokenType Parser::ProcLop() {
    while (!lexer.END) {                      
        auto [type, value] = lexer.getToken();
        switch (type) {
        case TokenType::EQU:
        case TokenType::NEQ:
        case TokenType::LES:
        case TokenType::LEQ:
        case TokenType::GTR:
        case TokenType::GEQ:
            return type;
        case TokenType::LPAREN:
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::IDENTIFIER:
        case TokenType::NUMBER:
            lexer.Keep();
            return type;
        default:
            lexer.Keep();
            lexer.Err(ERROR::EXPECTING_OPERATOR, value);
            return type;
        }
    }
    return TokenType::EQU;
}

//<id> → l{l|d}
std::string Parser::ProcId(int level, bool decl, SymbolType Stype, int &offset) {
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) break;
        if (type == TokenType::IDENTIFIER) {
            if (decl) {
                if (!stk.empty() && stk.back()->exist(value->name)) {
                    if (Stype == SymbolType::PROCEDURE)
                        lexer.Err(ERROR::REDEFINITION_OF_PROCEDURE, value);
                    else 
                        lexer.Err(ERROR::REDEFINITION_OF_IDENTIFIER, value);
                }
                switch (Stype) {
                case SymbolType::CONST: {
                    this->ProcBecomes();
                    int val = this->ProcInteger();
                    if (val != -1)
                        stk.back()->insert(value->name, Symbol(SymbolType::CONST, val, offset, level, 0));
                    break;
                }
                case SymbolType::PROCEDURE: {
                    stk.back()->insert(value->name, Symbol(SymbolType::PROCEDURE, -1, -1, level, initAllocateSize));
                    auto [it, success] = stk.back()->childs.insert({value->name, SymbolTable(value->name, level, stk.back())});
                    stk.push_back(&(it->second));
                    break;
                }
                case SymbolType::VAR: {
                    stk.back()->insert(value->name, Symbol(SymbolType::VAR, -1, offset++, level));
                    break;
                }
                case SymbolType::PROGRAM: {
                    this->base = SymbolTable(value->name);
                    stk.push_back(&base);
                    break;
                }
                }
            }
            else {
                if (stk.empty()) return nullptr;
                auto [name, symbol] = stk.back()->search(value->name);
                if (symbol == nullptr) {
                    lexer.Err(ERROR::UNDECLARED_IDENTIFIER, value);
                }
                if (Stype == SymbolType::PROCEDURE && symbol->type != SymbolType::PROCEDURE) {
                    lexer.Err(ERROR::EXPECTING_PROCEDURE, value);
                }
                last = symbol;
            }
            return value->name;
        }
        else if (type == TokenType::COMMA || type == TokenType::SEMICOLON || type == TokenType::LPAREN) {
            lexer.Err(ERROR::MISSING_IDENTIFIER, value);
            lexer.Keep();
            return "";
        }
        else if (type == TokenType::ERROR) {
            if (Stype == SymbolType::PROCEDURE) {
                std::string name = stk.back()->name + std::to_string(std::hash<std::string>{}(stk.back()->name));
                stk.back()->insert(name, Symbol(SymbolType::PROCEDURE, -1, -1, level, initAllocateSize));
                auto [it, success] = stk.back()->childs.insert({name, SymbolTable(name, level, stk.back())});
                stk.push_back(&(it->second));
            }
            else if (Stype == SymbolType::PROGRAM) {
                this->base = SymbolTable("main");
                stk.push_back(&base);
            }
            return "";
        }
        else if (isKeyword(value->name) != TokenType::ERROR) {
            lexer.Err(ERROR::EXPECTING_IDENTIFIER, value);
            lexer.Keep();
            return "";
        }
        else {
            lexer.Err(ERROR::EXPECTING_IDENTIFIER, value);
        }
    }
    return "";
}

//<integer> → d{d}
int Parser::ProcInteger() {
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) break;
        if (type == TokenType::NUMBER) {
            int val = -1;
            try {
                val = std::stoi(value->name);
            }
            catch (std::out_of_range const& ex) {
                lexer.Err(ERROR::OUT_OF_RANGE, value);
                val = -1;
            }
            return val;
        }
        else if (type == TokenType::COMMA || type == TokenType::SEMICOLON) {
            lexer.Err(ERROR::MISSING_INTEGER, value);
            lexer.Keep();
            return -1;
        }
        else if (isKeyword(value->name) != TokenType::ERROR) {
            lexer.Keep();
            return -1;
        }
        else {
            lexer.Err(ERROR::EXPECTING_INTEGER, value);
        }
    }
    return -1;
}

//if <lexp> then <statement>[else <statement>]
void Parser::ProcIf(int level) {
    this->ProcLexp(level);
    int cx1, cx2;
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) break;
        if (type == TokenType::THEN) {
            cx1 = Code.size();
            gen(oprType::JPC, 0, 0);
            this->ProcStatement(level);
            cx2 = Code.size();
            gen(oprType::JMP, 0, 0);
            Code[cx1].offset = Code.size();
            Code[cx2].offset = Code.size();
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
            this->ProcStatement(level);
            break;
        }
        else {
            lexer.Err(ERROR::EXPECTING_THEN, value);
        }
    }
    /*
             lexp       if
    cx1      jpc        
             ....       then_block
    cx2      jmp        
             ....       else_block
    */
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) break;
        if (type == TokenType::ELSE) {
            this->ProcStatement(level);
            Code[cx2].offset = Code.size();
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
void Parser::ProcWhile(int level) {
    int cx1 = Code.size();
    this->ProcLexp(level);
    int cx2 = Code.size();
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) break;
        if (type == TokenType::DO) {
            gen(oprType::JPC, 0, 0);
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

    this->ProcStatement(level);
    gen(oprType::JMP, 0, cx1);
    Code[cx2].offset = Code.size();
}

//call <id>（[<exp>{,<exp>}]）
void Parser::ProcCall(int level) {
    int ofst = 1;
    this->ProcId(level, false, SymbolType::PROCEDURE, ofst);
    Symbol* procedure = last;
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) break;
        if (type == TokenType::LPAREN) {
            break;
        }
        else if (StatementEnd(type)) {
            lexer.Err(ERROR::MISSING_LPAREN, value);
            lexer.Keep();
            break;
        }
        else if (StatementBegin(type)) {
            lexer.Err(ERROR::MISSING_STATEMENT, value);
            lexer.Keep();
            break;
        }
        else {
            lexer.Err(ERROR::EXPECTING_LPAREN, value);
        }
    }

    int cnt = this->ProcCallParam(level);
    if (procedure != nullptr) {
        if (cnt != procedure->val) {
            lexer.Err(ERROR::INCORRECT_PARAMETERS, cnt, procedure->val);
        }
        for (int i = 0; i < procedure->val; ++i)
            gen(oprType::POP, 0, 0);
        gen(oprType::CAL, level - procedure->level, procedure->address);
    }
}

//[<exp>{,<exp>}])
int Parser::ProcCallParam(int level) {
    int cnt = 0;
    if (!lexer.END) {                      
        auto [type, value] = lexer.getToken();
        if (type == TokenType::RPAREN) { 
            return cnt;
        }
        else 
            lexer.Keep();
    }
    this->ProcExp(level);
    ++cnt;
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) break;
        if (type == TokenType::COMMA) {
            this->ProcExp(level);
            ++cnt;
        }
        else if (type == TokenType::RPAREN) {
            break;
        }
        else if (ExpBegin(type)) {
            lexer.Err(ERROR::MISSING_EXPR, value);
            this->ProcExp(level);
            ++cnt;
        }
        else if (StatementEnd(type)) {
            lexer.Err(ERROR::MISSING_RPAREN, value);
            lexer.Keep();
            break;
        }
        else if (StatementBegin(type)) {
            lexer.Keep();
            break;
        }
        else {
            lexer.Err(ERROR::EXPECTING_RPAREN, value);
        }
    }
    return cnt;
}

void Parser::ProcRead(int level) {
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) break;
        if (type == TokenType::LPAREN) {
            break;
        }
        else if (StatementEnd(type)) {
            lexer.Err(ERROR::MISSING_LPAREN, value);
            lexer.Keep();
            break;
        }
        else if (StatementBegin(type)) {
            lexer.Err(ERROR::MISSING_STATEMENT, value);
            lexer.Keep();
            break;
        }
        else {
            lexer.Err(ERROR::EXPECTING_LPAREN, value);
        }
    }
    this->ProcReadParam(level);
}

//<id>{，<id>})
void Parser::ProcReadParam(int level) {
    int ofst = 1;
    this->ProcId(level, false, SymbolType::VAR, ofst);
    if (last != nullptr)
        gen(oprType::RED, level - last->level, last->address);
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) break;
        if (type == TokenType::COMMA) {
            this->ProcId(level, false, SymbolType::VAR, ofst);
            if (last != nullptr)
                gen(oprType::RED, level - last->level, last->address);
        }
        else if (type == TokenType::RPAREN) {
            break;
        }
        else if (StatementEnd(type)) {
            lexer.Err(ERROR::MISSING_RPAREN, value);
            lexer.Keep();
            break;
        }
        else if (StatementBegin(type)) {
            lexer.Keep();
            break;
        }
        else {
            lexer.Err(ERROR::EXPECTING_RPAREN, value);
        }
    }
}

//write (<exp>{,<exp>})
void Parser::ProcWrite(int level) {
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) break;
        if (type == TokenType::LPAREN) {
            break;
        }
        else if (StatementEnd(type)) {
            lexer.Err(ERROR::MISSING_LPAREN, value);
            lexer.Keep();
            break;
        }
        else if (StatementBegin(type)) {
            lexer.Err(ERROR::MISSING_STATEMENT, value);
            lexer.Keep();
            break;
        }
        else {
            lexer.Err(ERROR::EXPECTING_LPAREN, value);
        }
    }
    this->ProcWriteParam(level);
    gen(oprType::OPR, 0, static_cast<int>(oprCode::LIN));
}

//<exp>{,<exp>})
void Parser::ProcWriteParam(int level) {
    this->ProcExp(level);
    gen(oprType::WRT, 0, 0);
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) break;
        if (type == TokenType::COMMA) {
            this->ProcExp(level);
            gen(oprType::WRT, 0, 0);
        }
        else if (type == TokenType::RPAREN) {
            break;
        }
        else if (StatementEnd(type)) {
            lexer.Err(ERROR::MISSING_RPAREN, value);
            lexer.Keep();
            break;
        }
        else if (StatementBegin(type)) {
            lexer.Keep();
            break;
        }
        else {
            lexer.Err(ERROR::EXPECTING_RPAREN, value);
        }
    }
}

void Parser::ProcAssign(int level) {
    int ofst = 1;
    this->ProcId(level, false, SymbolType::VAR, ofst);
    Symbol* assign = last;
    this->ProcBecomes();
    this->ProcExp(level);
    if (assign != nullptr) {
        gen(oprType::STO, level - assign->level, assign->address);
    }
}

void Parser::ProcBecomes() {
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) break;
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

void Parser::loadFile(const std::string& path){
    lexer.LoadFile(path);
    base = SymbolTable("main");
    Code.clear();
    Code.reserve(maxCodeSize);
    stk.clear();
}

void Parser::analyze(){
    while (!lexer.END) {
        auto [type, value] = lexer.getToken();
        if (type == TokenType::ENDOFFILE) break;
        if (type == TokenType::PROGRAM) {
            ProcProg();
            break;
        }
        else lexer.Err(ERROR::EXPECTING_PROGRAM, value);
    }
    if (lexer.error.ErrCnt != 0) {
        std::cout << red << "Build Error : " << white << lexer.error.ErrCnt << std::endl;
        exit(0);
    }
    else {
        std::cout << green << "Build Success." << white << std::endl;
    }
}

void Parser::write() {
    std::filesystem::path WritePath = lexer.Path.replace_extension(".token");
    lexer.write(WritePath);
    WritePath = lexer.Path.replace_extension(".symbol");
    std::ofstream OutputFile;
    OutputFile.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    try {
        OutputFile.open(WritePath);
        base.write(OutputFile);
        OutputFile.close();
        std::cout << "SYMBOL_TABLE IS WRITED IN " << blue << WritePath << white << std::endl;
    } 
    catch (std::ofstream::failure& e) {
        std::cout << red << "Write Error: " << white << e.what() << std::endl;
        std::cout << "From " << blue << WritePath << white << std::endl;
        return;
    }
    if (lexer.error.ErrCnt != 0) return;
    WritePath = WritePath.replace_extension(".pcode");
    try {
        OutputFile.open(WritePath);
        std::for_each(Code.begin(), Code.end(), [&OutputFile](const Ins& ins) {
            OutputFile << ins.str() << '\n';
        });
        OutputFile.close();
        std::cout << "PCODE IS WRITED IN " << blue << WritePath << white << std::endl;
    } 
    catch (std::ofstream::failure& e) {
        std::cout << red << "Write Error: " << white << e.what() << std::endl;
        std::cout << "From " << blue << WritePath << white << std::endl;
        return;
    }
}

void Parser::gen(oprType type, int l, int a) {
    if (lexer.error.ErrCnt != 0) return;
    Code.push_back(Ins(type, l, a));
}