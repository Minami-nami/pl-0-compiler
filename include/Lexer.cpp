#include "Lexer.h"
#include <fstream>
#include <string>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include "ConsoleColor.h"

/*
<prog> → program <id>；<block>
<block> → [<condecl>][<vardecl>][<proc>]<body>
<condecl> → const <const>{,<const>};
<const> → <id>:=<integer>
<vardecl> → var <id>{,<id>};
<proc> → procedure <id>（[<id>{,<id>}]）;<block>{;<proc>}
<body> → begin <statement>{;<statement>}end
<statement> → <id> := <exp>
               |if <lexp> then <statement>[else <statement>]
               |while <lexp> do <statement>
               |call <id>（[<exp>{,<exp>}]）
               |<body>
               |read (<id>{，<id>})
               |write (<exp>{,<exp>})
<lexp> → <exp> <lop> <exp>|odd <exp>
<exp> → [+|-]<term>{<aop><term>}
<term> → <factor>{<mop><factor>}
<factor>→<id>|<integer>|(<exp>)
<lop> → =|<>|<|<=|>|>=
<aop> → +|-
<mop> → *|/
<id> → l{l|d}   （注：l表示字母）
<integer> → d{d}
注释：
<prog>：程序 ；<block>：块、程序体 ；<condecl>：常量说明 ；<const>：常量；
<vardecl>：变量说明 ；<proc>：分程序 ； <body>：复合语句 ；<statement>：语句；
<exp>：表达式 ；<lexp>：条件 ；<term>：项 ； <factor>：因子 ；<aop>：加法运算符；
<mop>：乘法运算符； <lop>：关系运算符
odd：判断表达式的奇偶性。
*/
enum class state {
    BEGIN,
    WORD,
    NUMBER,
    COLON,          //  :
    LES,            //  <
    GTR,            //  >
    NUMWORD,
    ILEGALCH
};

const std::unordered_map<char, TokenType> LegalCh = {
    {'=', TokenType::EQU},      {'+', TokenType::PLUS},     {'-', TokenType::MINUS},
    {'*', TokenType::TIMES},    {'/', TokenType::SLASH},    {'(', TokenType::LPAREN},
    {')', TokenType::RPAREN},   {',', TokenType::COMMA},    {';', TokenType::SEMICOLON}
};

Lexer::Lexer(): row(1), col(0), curType(TokenType::ERROR), remain(false), END(false) {}

Lexer::~Lexer() {}

bool Lexer::LoadFile(const std::string& FilePath) {
    row = 1, col = 0;
    curType = TokenType::ERROR;
    remain = false;
    END = false;
    Path = std::string(FilePath);
    std::filesystem::path codePath = std::filesystem::path(Path).replace_extension(".pcode");
    std::filesystem::remove(codePath);
    std::ifstream CodeFile;
    CodeFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        CodeFile.open(Path);
        CodeStream << CodeFile.rdbuf();
        CodeStream << '\n';
        CodeFile.close();
    }
    catch (std::ifstream::failure& e) {
        std::cout << red << "Load Error: " << white << e.what() << std::endl;
        std::cout << "From " << blue << Path << white << std::endl;
        return false;
    }
    std::getline(CodeStream, linebuf);
    linebuf += '\n';
    lineptr = linebuf.begin();
    return true;
}

void Lexer::ClearBuf() {
    CodeStream.clear();
    CodeStream.str("");
}

bool Lexer::write(const std::filesystem::path& WritePath) {
    /* _______________________________________________
     *      name     |    type    |   row   |   col   
     * ———————————————————————————————————————————————
     *     lololol        inde         2         1     
    */
    std::ofstream TokenFile;
    TokenFile.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    ClearBuf();
    try {
        TokenFile.open(WritePath);
        std::vector<Token> st;
        std::for_each(TokenTable.begin(), TokenTable.end(), [&st](const auto& v) {
            st.push_back(v);
        });
        sort(st.begin(), st.end(), [](const auto& l, const auto& r) {
            return l.row < r.row? true: (l.row == r.row? l.col < r.col: false);
        });
        TokenFile << "_______________________________________________\n"
                     "     name     |    type    |   row   |   col   \n" 
                     "———————————————————————————————————————————————\n";
        std::for_each(st.begin(), st.end(), [&TokenFile](const auto& v) {
            TokenFile << v.toString() << std::endl;
        });
        TokenFile.close();
    } 
    catch (std::ofstream::failure& e) {
        std::cout << red << "Write Error: " << white << e.what() << std::endl;
        std::cout << "From " << blue << WritePath << white << std::endl;
        return false;
    }
    std::cout << "TOKEN_TABLE IS WRITED IN " << blue << WritePath << white << std::endl;
    return true;
}

std::pair<TokenType, std::unordered_set<Token, KeyHash_Token, Equal_Token>::iterator> Lexer::getToken() {
    if (remain) {
        remain = false;
        return std::make_pair(curType, it);
    }
    std::string token;
    state curstate = state::BEGIN;
    while (lineptr == linebuf.end()) {//取新行
        std::getline(CodeStream, linebuf);
        linebuf += '\n';
        lineptr = linebuf.begin();
        ++row; col = 0;
        if (CodeStream.eof()) {//文件结尾
            END = true;
            ClearBuf();
            return std::make_pair(TokenType::ENDOFFILE, it);
        }
    }
    while (lineptr != linebuf.end()) {
        char ch = *(lineptr++);
        ++col;
        std::pair<std::unordered_set<Token, KeyHash_Token>::iterator, bool> rt;
        switch (curstate) {
        case state::BEGIN:{
            token += ch;
            if (isalpha(ch)) curstate = state::WORD;
            else if (isdigit(ch)) curstate = state::NUMBER;
            else { 
                switch (ch)
                {
                case '<':
                    curstate = state::LES;
                    break;
                case '>':
                    curstate = state::GTR;
                    break;
                case ':':
                    curstate = state::COLON;
                    break;
                case '=':
                case '+':
                case '-':
                case '*':
                case '/':
                case '(':
                case ')':
                case ',':
                case ';':
                    curType = LegalCh.find(ch)->second;
                    rt = TokenTable.insert(Token(token, curType, this->row, this->col + 1 - static_cast<int>(token.size())));
                    it = rt.first;
                    return std::make_pair(curType, it);
                case '\t':
                    token.pop_back();
                    col += 3;
                    break;
                case ' ':
                case '\n':
                case '\r':
                case '\v':
                case '\f':
                    token.pop_back();
                    break;
                default:
                    curstate = state::ILEGALCH;
                    break;
                }
            }
            break;
        }
        case state::COLON:{
            if (ch == '=') {
                curType = TokenType::BECOMES;
                token += ch;
                rt = TokenTable.insert(Token(token, curType, this->row, this->col + 1 - static_cast<int>(token.size())));
                it = rt.first;
                return std::make_pair(curType, it);
            }
            else {
                --lineptr; --col;
                error.ProcError(ERROR::MISSING_BECOMES, row, this->col + 1 - static_cast<int>(token.size()), token, linebuf, Path.string());
                curType = TokenType::ERROR;
                rt = TokenTable.insert(Token(token, curType, this->row, this->col + 1 - static_cast<int>(token.size())));
                it = rt.first;
                return std::make_pair(curType, it);
                break;
            }
        }
        case state::GTR:{
            if (ch == '=') {token += ch; curType = TokenType::GEQ;}
            else {--lineptr; --col; curType = TokenType::GTR;}
            rt = TokenTable.insert(Token(token, curType, this->row, this->col + 1 - static_cast<int>(token.size())));
            it = rt.first;
            return std::make_pair(curType, it);
        }
        case state::LES:{
            if (ch == '=') {token += ch; curType = TokenType::LEQ;}
            else if (ch == '>') {token += ch; curType = TokenType::NEQ;}
            else {--lineptr; --col; curType = TokenType::LES;}
            rt = TokenTable.insert(Token(token, curType, this->row, this->col + 1 - static_cast<int>(token.size())));
            it = rt.first;
            return std::make_pair(curType, it);
        }
        case state::NUMBER:{
            token += ch;
            if (isalpha(ch)) {
                curstate = state::NUMWORD;
            }
            else if (!isdigit(ch)) {
                curType = TokenType::NUMBER;
                token.pop_back();
                --lineptr;
                --col;
                rt = TokenTable.insert(Token(token, curType, this->row, this->col + 1 - static_cast<int>(token.size())));
                it = rt.first;
                return std::make_pair(curType, it);
            }
            break;
        }
        case state::WORD:{
            if (!isalnum(ch)) {
                --lineptr;
                --col;
                curType = isKeyword(token);
                if (curType == TokenType::ERROR) {
                    curType = TokenType::IDENTIFIER;
                    rt = TokenTable.insert(Token(token, curType, this->row, this->col + 1 - static_cast<int>(token.size())));
                    it = rt.first;
                    return std::make_pair(curType, it);
                }
                else {
                    rt = TokenTable.insert(Token(token, curType, this->row, this->col + 1 - static_cast<int>(token.size())));
                    it = rt.first;
                    return std::make_pair(curType, it);
                }
            }
            else token += ch;
            break;
        }
        case state::NUMWORD:{
            if (!isalnum(ch)) {
                --lineptr;
                --col;
                curType = TokenType::ERROR;
                error.ProcError(ERROR::ILEGAL_IDENTIFIER, row, this->col + 1 - static_cast<int>(token.size()), token, linebuf, Path.string());
                rt = TokenTable.insert(Token(token, curType, this->row, this->col + 1 - static_cast<int>(token.size())));
                it = rt.first;
                return std::make_pair(curType, it);
            }
            else token += ch;
            break;
        }
        case state::ILEGALCH:{
            if (isLegalch(ch)) {
                --lineptr;
                --col;
                curType = TokenType::ERROR;
                error.ProcError(ERROR::ILEGAL_CHARACTER, row, this->col + 1 - static_cast<int>(token.size()), token, linebuf, Path.string());
                rt = TokenTable.insert(Token(token, curType, this->row, this->col + 1 - static_cast<int>(token.size())));
                it = rt.first;
                return std::make_pair(curType, it);
            }
            else token += ch;
            break;
        }
        }

        while (lineptr == linebuf.end()) {//取新行
            std::getline(CodeStream, linebuf);
            linebuf += '\n';
            lineptr = linebuf.begin();
            ++row; col = 0;
            if (CodeStream.eof()) {//文件结尾
                END = true;
                ClearBuf();
                return std::make_pair(TokenType::ENDOFFILE, it);
            }
        }
    }
    return std::make_pair(TokenType::ENDOFFILE, it);
}

bool Lexer::isLegalch(char ch) {
    return isalnum(ch) || isspace(ch) || ch == '=' || ch == '<' || ch == '>' || ch == ':' || ch == ';'
                       || ch == ','   || ch == '(' || ch == ')' || ch == '+' || ch == '-' || ch == '*'
                       || ch == '/'   || ch == 0;
}

void Lexer::Keep() {
    remain = true;
}

void Lexer::Err(ERROR etype, std::unordered_set<Token, KeyHash_Token, Equal_Token>::iterator it) {
    error.ProcError(etype, row, this->col + 1 - static_cast<int>(it->name.size()), it->name, linebuf, Path.string());
}

void Lexer::Err(ERROR etype, int real, int expected) {
    error.ProcError(etype, real, expected);
}