#pragma once
#include "Lexer.h"
#include "Code.h"
#include <vector>
#include <array>
constexpr int maxCodeSize = 1024;
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
               |call <id> ([<exp>{,<exp>}])
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

class Parser
{
private:
    Lexer lexer;
    SymbolTable base;
    std::vector<SymbolTable*> stk;
    Symbol* last;
    void ProcProg();
    void ProcBlock(const std::string& name, int level, int &offset);
    void ProcCondecl(int level, int &offset);
    void ProcVardecl(int level, int &offset);
    void ProcProc(int level, int &offset);
    void ProcProcParam(int level, int &offset);
    void ProcBody(int level);
    void ProcMultiStatement(int level);
    void ProcStatement(int level);
    void ProcLexp(int level);
    void ProcExp(int level);
    void ProcTerm(int level);
    void ProcFactor(int level);
    TokenType ProcLop();
    std::string ProcId(int level, bool decl, SymbolType Stype, int &offset);
    int  ProcInteger();
    void ProcIf(int level);
    void ProcWhile(int level);
    void ProcCall(int level);
    int  ProcCallParam(int level);
    void ProcRead(int level);
    void ProcReadParam(int level);
    void ProcWrite(int level);
    void ProcWriteParam(int level);
    void ProcAssign(int level);
    void ProcBecomes();
public:
    std::vector<Ins> Code;
    Parser(): base("main"){}
    ~Parser() {}
    void loadFile(const std::string& path);
    void analyze();
    void write();
    void gen(oprType type, int l, int a);
};
