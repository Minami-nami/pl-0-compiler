#pragma once
#include "PL0Parser.h"
#include <string>

class Compiler {
public:
    void run(const std::string& path);
    Parser parser;
};