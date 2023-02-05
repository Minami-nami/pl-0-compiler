#pragma once
#include "Compiler.h"

constexpr int maxStackSize = 2048;

class Interpreter {
private:
    std::array<Ins, maxCodeSize> Code;
    std::array<int, maxStackSize> Stack;
    int stackTopReg, baseAddrReg, ProgAddrReg;
    int CodeLen = 0;
    Ins InsReg;
    Compiler compiler;
    std::string FilePath;
    int base(int level, int base) {
        while (level-- > 0) {
            base = Stack[base + 1];
        }
        return base;
    }
    void init();
public:
    Interpreter(int argc, char* argv[]);
    void run();
    void printPcode();
    void printSymbolTable();
    void printTokens();
};