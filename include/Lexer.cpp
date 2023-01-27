#include "Lexer.h"
#include <fstream>
#include <string>
#include <cstdio>
#include <algorithm>
#include <vector>

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

Lexer::Lexer(): row(1), col(0), curType(TokenType::ERROR), remain(false) {}

Lexer::~Lexer() {}

bool Lexer::LoadFile(const char* FilePath) {
    row = 1, col = 0;
    curType = TokenType::ERROR;
    remain = false;
    Path = std::string(FilePath);
    std::ifstream CodeFile;
    CodeFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        CodeFile.open(Path);
        CodeStream << CodeFile.rdbuf();
        CodeStream << '\n';
        CodeFile.close();
    }
    catch (std::ifstream::failure& e) {
        std::cout << "ERROR::LEXER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        std::cout << "From " << Path << std::endl;
        return false;
    }
    return true;
}

bool Lexer::WriteFile() {
    std::filesystem::path WritePath = Path;
    WritePath.replace_extension(".token");
    std::ofstream TokenFile;
    TokenFile.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    if (End()) {
        CodeStream.clear();
        CodeStream.str("");
    }
    try {
        TokenFile.open(WritePath);
        std::vector<Token> st;
        std::for_each(TokenTable.begin(), TokenTable.end(), [&st](const auto& v) {
            st.push_back(v);
        });
        sort(st.begin(), st.end(), [](const auto& l, const auto& r) {
            return l.row < r.row? true: (l.row == r.row? l.col < r.col: false);
        });
        std::for_each(st.begin(), st.end(), [&TokenFile](const auto& v) {
            TokenFile << v.toString() << std::endl;
        });
        TokenFile.close();
    } 
    catch (std::ofstream::failure& e) {
        std::cout << "ERROR::LEXER::FILE_NOT_SUCCESFULLY_WRITE: " << e.what() << std::endl;
        std::cout << "From " << WritePath << std::endl;
        return false;
    }
    std::cout << "TOKEN_TABLE IS WRITED IN " << WritePath << std::endl;
    return true;
}

std::pair<TokenType, std::unordered_set<Token, KeyHash, Equal>::iterator> Lexer::getToken() {
    if (remain) {
        remain = false;
        return std::make_pair(curType, it);
    }
    std::string token;
    state curstate = state::BEGIN;
    char ch = CodeStream.get();
    while (!CodeStream.eof()) {
        ++col;
        std::pair<std::unordered_set<Token, KeyHash>::iterator, bool> rt;
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
                    curType = TokenType::EQU;
                    rt = TokenTable.insert(Token(token, curType, this->row, this->col + 1 - static_cast<int>(token.size())));
                    it = rt.first;
                    return std::make_pair(curType, it);
                case '+':
                    curType = TokenType::PLUS;
                    rt = TokenTable.insert(Token(token, curType, this->row, this->col + 1 - static_cast<int>(token.size())));
                    it = rt.first;
                    return std::make_pair(curType, it);
                case '-':
                    curType = TokenType::MINUS;
                    rt = TokenTable.insert(Token(token, curType, this->row, this->col + 1 - static_cast<int>(token.size())));
                    it = rt.first;
                    return std::make_pair(curType, it);
                case '*':
                    curType = TokenType::TIMES;
                    rt = TokenTable.insert(Token(token, curType, this->row, this->col + 1 - static_cast<int>(token.size())));
                    it = rt.first;
                    return std::make_pair(curType, it);
                case '/':
                    curType = TokenType::SLASH;
                    rt = TokenTable.insert(Token(token, curType, this->row, this->col + 1 - static_cast<int>(token.size())));
                    it = rt.first;
                    return std::make_pair(curType, it);
                case '(':
                    curType = TokenType::LPAREN;
                    rt = TokenTable.insert(Token(token, curType, this->row, this->col + 1 - static_cast<int>(token.size())));
                    it = rt.first;
                    return std::make_pair(curType, it);
                case ')':
                    curType = TokenType::RPAREN;
                    rt = TokenTable.insert(Token(token, curType, this->row, this->col + 1 - static_cast<int>(token.size())));
                    it = rt.first;
                    return std::make_pair(curType, it);
                case ',':
                    curType = TokenType::COMMA;
                    rt = TokenTable.insert(Token(token, curType, this->row, this->col + 1 - static_cast<int>(token.size())));
                    it = rt.first;
                    return std::make_pair(curType, it);
                case ';':
                    curType = TokenType::SEMICOLON;
                    rt = TokenTable.insert(Token(token, curType, this->row, this->col + 1 - static_cast<int>(token.size())));
                    it = rt.first;
                    return std::make_pair(curType, it);
                case '\n':
                    token.pop_back();
                    ++row;
                    col = 0;
                    break;
                case '\t':
                    token.pop_back();
                    col += 3;
                    break;
                case ' ':
                case '\r':
                case '\v':
                case '\f':
                case -1:
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
                CodeStream.seekg(-1, std::ios::cur);
                --col;
                error(LEXERROR::MISSING_EQUAL_AFTER_COLON, token);
                curType = TokenType::ERROR;
                rt = TokenTable.insert(Token(token, curType, this->row, this->col + 1 - static_cast<int>(token.size())));
                it = rt.first;
                return std::make_pair(curType, it);
                break;
            }
        }
        case state::GTR:{
            if (ch == '=') {token += ch; curType = TokenType::GEQ;}
            else {CodeStream.seekg(-1, std::ios::cur); --col; curType = TokenType::GTR;}
            rt = TokenTable.insert(Token(token, curType, this->row, this->col + 1 - static_cast<int>(token.size())));
            it = rt.first;
            return std::make_pair(curType, it);
        }
        case state::LES:{
            if (ch == '=') {token += ch; curType = TokenType::LEQ;}
            else if (ch == '>') {token += ch; curType = TokenType::NEQ;}
            else {CodeStream.seekg(-1, std::ios::cur); --col; curType = TokenType::LES;}
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
                CodeStream.seekg(-1, std::ios::cur);
                --col;
                rt = TokenTable.insert(Token(token, curType, this->row, this->col + 1 - static_cast<int>(token.size())));
                it = rt.first;
                return std::make_pair(curType, it);
            }
            break;
        }
        case state::WORD:{
            if (!isalnum(ch)) {
                CodeStream.seekg(-1, std::ios::cur);
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
                CodeStream.seekg(-1, std::ios::cur);
                --col;
                curType = TokenType::ERROR;
                error(LEXERROR::ILEGAL_IDENTIFIER, token);
                rt = TokenTable.insert(Token(token, curType, this->row, this->col + 1 - static_cast<int>(token.size())));
                it = rt.first;
                return std::make_pair(curType, it);
            }
            else token += ch;
            break;
        }
        case state::ILEGALCH:{
            if (isLegalch(ch)) {
                CodeStream.seekg(-1, std::ios::cur);
                --col;
                curType = TokenType::ERROR;
                error(LEXERROR::ILEGAL_CHARACTER, token);
                rt = TokenTable.insert(Token(token, curType, this->row, this->col + 1 - static_cast<int>(token.size())));
                it = rt.first;
                return std::make_pair(curType, it);
            }
            else token += ch;
            break;
        }
        }
        ch = CodeStream.get();
    }
    return std::make_pair(TokenType::ENDOFFILE, it);
}

bool Lexer::isLegalch(char ch) {
    return isalnum(ch) || isspace(ch) || ch == '=' || ch == '<' || ch == '>' || ch == ':' || ch == ';'
                       || ch == ',' || ch == '(' || ch == ')' || ch == '+' || ch == '-' || ch == '*'
                       || ch == '/' || ch == 0;
}

void Lexer::error(LEXERROR type, const std::string& token) {
    switch (type)
    {
    case LEXERROR::MISSING_EQUAL_AFTER_COLON:
        printf("ERROR::LEXER::MISSING_EQUAL_AFTER_COLON: ");
        break;
    case LEXERROR::ILEGAL_IDENTIFIER:
        printf("ERROR::LEXER::ILEGAL_IDENTIFIER:         ");
        break;
    case LEXERROR::ILEGAL_CHARACTER:
        printf("ERROR::LEXER::ILEGAL_CHARACTER:          ");
        break;
    }
    printf("%-16s\t[行 %-3d, 列 %-3d]\n", token.c_str(), row, col + 1 - static_cast<int>(token.size()));
}

bool Lexer::End() {
    return CodeStream.eof();
}

void Lexer::Keep() {
    remain = true;
}