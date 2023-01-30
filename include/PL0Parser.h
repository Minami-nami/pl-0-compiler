#pragma once
#include "Lexer.h"
#include "Error.h"

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
    
    void ProcProg();
    void ProcBlock();
    void ProcCondecl();
    void ProcConst();
    void ProcVardecl();
    void ProcVar();
    void ProcProc();
    void ProcProcParam();
    void ProcBody();
    void ProcMultiStatement();
    void ProcStatement();
    void ProcLexp();
    void ProcExp();
    void ProcTerm();
    void ProcFactor();
    void ProcLop();
    void ProcId(bool decl, SymbolType Stype);
    void ProcInteger();
    void ProcIf();
    void ProcWhile();
    void ProcCall();
    void ProcCallParam();
    void ProcRead();
    void ProcReadParam();
    void ProcWrite();
    void ProcWriteParam();
    void ProcAssign();
    void ProcBecomes();

public:
    Parser();
    ~Parser();
    void loadFile(const char* FilePath);
    void analyze();
};
