#pragma once 
#include <string>

enum class oprType {
    LIT, //LIT L, a     取常量a放入数据栈栈顶
    OPR, //OPR L, a     执行运算，a表示执行某种运算
    LOD, //LOD L, a     取变量（相对地址为a，层差为L）放到数据栈的栈顶
    STO, //STO L, a     将数据栈栈顶的内容存入变量（相对地址为a，层次差为L）
    CAL, //CAL L, a     调用过程（转子指令）（入口地址为a，层次差为L）
    INT, //INT 0, a     数据栈栈顶指针增加a
    JMP, //JMP 0, a     无条件转移到地址为a的指令
    JPC, //JPC 0, a     条件转移指令，若栈顶为0则转移到地址为a的指令
    RED, //RED L, a     读数据并存入变量（相对地址为a，层次差为L）
    WRT, //WRT 0, 0     将栈顶内容输出(--)
    POP, //POP 0, 0     数据栈栈顶内容向高地址移动3，指针--
};

enum class oprCode {//(--)
    RET = 0,    //return
    ADD = 1,    //[top-1] =  [top] +  [top-1]
    SUB = 2,    //[top-1] =  [top] -  [top-1]
    MUL = 3,    //[top-1] =  [top] *  [top-1]
    DIV = 4,    //[top-1] =  [top] /  [top-1]
    ODD = 5,    //[top]   = ([top] mod 2) == 1
    NEG = 6,    //[top]   =  -[top]
    EQL = 7,    //[top-1] = ([top] =  [top-1])
    NEQ = 8,    //[top-1] = ([top] <> [top-1])
    GTR = 9,    //[top-1] = ([top] >  [top-1])
    GEQ = 10,   //[top-1] = ([top] >= [top-1])
    LES = 11,   //[top-1] = ([top] <  [top-1])
    LEQ = 12,   //[top-1] = ([top] <= [top-1])
    LIN = 13    //output \n
};

std::string toString(oprType type);

class Ins {
public:
    oprType opr;
    int level, offset;
    Ins(oprType _opr = oprType::JMP, int _level = 0, int _offset = 0): opr(_opr), level(_level), offset(_offset) {}
    Ins(const std::string& s);
    std::string str() const;
};