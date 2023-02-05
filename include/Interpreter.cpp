#include "Interpreter.h"
#include <windows.h>
#include <string>
#include <filesystem>

Interpreter::Interpreter(int argc, char* argv[]) {
    SetConsoleOutputCP(65001);
    if (argc <= 1) {
		std::cout << ">>请输入文件路径: \n>>";
		std::cin >> FilePath;
	}
	else {
		FilePath = std::string(argv[1]);
	}
    if (!std::filesystem::exists(FilePath)) {
        std::cout << "文件不存在！";
        exit(0);
    }
    auto dir = std::filesystem::path(FilePath).parent_path();
    FilePath = std::filesystem::path(FilePath).filename().string();
    std::filesystem::current_path(dir);
    std::cout << "cwd: " << dir << std::endl;
	compiler.run(FilePath);
    init();
}

void Interpreter::init() {
    stackTopReg = ProgAddrReg = baseAddrReg = 0;
    if (compiler.parser.Code.empty()) {
        if (CodeLen == 0) {
            std::ifstream PCodeFile;
            std::string Path = std::filesystem::path(FilePath).replace_extension(".pcode").string();
            PCodeFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            try {
                PCodeFile.open(Path);
                std::string opr;
                int l, a;
                std::string strLine;
                while(std::getline(PCodeFile,strLine))
                {
                    if(strLine.empty())
                        continue;
                    if (CodeLen >= maxCodeSize) {
                        std::cout << "error: Exceed MaxCodeSize!\n";
                        exit(0);
                    }
                    Code[CodeLen++] = Ins(strLine);
                }
                PCodeFile.close();
            }
            catch (std::ifstream::failure& e) {
                return;
            }
        }
    }
    else if (CodeLen == 0) {
        std::copy(compiler.parser.Code.begin(), compiler.parser.Code.end(), Code.begin());
        CodeLen = compiler.parser.Code.size();
    }
}

void Interpreter::run() {
    stackTopReg = ProgAddrReg = baseAddrReg = 0;
    /*
     *  baseAddrReg         DL
     *  baseAddrReg+1       SL
     *  baseAddrReg+2       RA
     */
    do {
        [[unlikely]] if (stackTopReg >= maxStackSize) {
            std::cout << "error: Stack Overflow!\n";
            exit(0);
        }
        InsReg = Code[ProgAddrReg++];
        switch (InsReg.opr) {
        case oprType::OPR:
            switch (InsReg.offset) {
            case static_cast<int>(oprCode::RET):
                stackTopReg = baseAddrReg;
                ProgAddrReg = Stack[baseAddrReg + 2];
                baseAddrReg = Stack[baseAddrReg];
                break;
            case static_cast<int>(oprCode::ADD):
                Stack[stackTopReg - 2] = Stack[stackTopReg - 2] + Stack[stackTopReg - 1];
                --stackTopReg;
                break;
            case static_cast<int>(oprCode::SUB):
                Stack[stackTopReg - 2] = Stack[stackTopReg - 2] - Stack[stackTopReg - 1];
                --stackTopReg;
                break;
            case static_cast<int>(oprCode::MUL):
                Stack[stackTopReg - 2] = Stack[stackTopReg - 2] * Stack[stackTopReg - 1];
                --stackTopReg;
                break;
            case static_cast<int>(oprCode::DIV):
                Stack[stackTopReg - 2] = Stack[stackTopReg - 2] / Stack[stackTopReg - 1];
                --stackTopReg;
                break;
            case static_cast<int>(oprCode::ODD):
                Stack[stackTopReg - 1] &= 1; 
                break;
            case static_cast<int>(oprCode::NEG):
                Stack[stackTopReg - 1] *= -1;
                break;
            case static_cast<int>(oprCode::EQL):
                Stack[stackTopReg - 2] = (Stack[stackTopReg - 2] == Stack[stackTopReg - 1]);
                --stackTopReg;
                break;
            case static_cast<int>(oprCode::NEQ):
                Stack[stackTopReg - 2] = (Stack[stackTopReg - 2] != Stack[stackTopReg - 1]);
                --stackTopReg;
                break;
            case static_cast<int>(oprCode::GTR):
                Stack[stackTopReg - 2] = (Stack[stackTopReg - 2] > Stack[stackTopReg - 1]);
                --stackTopReg;
                break;
            case static_cast<int>(oprCode::GEQ):
                Stack[stackTopReg - 2] = (Stack[stackTopReg - 2] >= Stack[stackTopReg - 1]);
                --stackTopReg;
                break;
            case static_cast<int>(oprCode::LES):
                Stack[stackTopReg - 2] = (Stack[stackTopReg - 2] < Stack[stackTopReg - 1]);
                --stackTopReg;
                break;
            case static_cast<int>(oprCode::LEQ):
                Stack[stackTopReg - 2] = (Stack[stackTopReg - 2] <= Stack[stackTopReg - 1]);
                --stackTopReg;
                break;
            case static_cast<int>(oprCode::LIN):
                printf("\n");
                break;
            }
            break;
        case oprType::LIT:
            Stack[stackTopReg++] = InsReg.offset;
            break;
        case oprType::LOD:
            Stack[stackTopReg++] = Stack[InsReg.offset + base(InsReg.level, baseAddrReg)];
            break;
        case oprType::STO:
            Stack[InsReg.offset + base(InsReg.level, baseAddrReg)] = Stack[--stackTopReg];
            break;
        case oprType::CAL:
            Stack[stackTopReg]     = baseAddrReg;
            Stack[stackTopReg + 1] = base(InsReg.level, baseAddrReg);
            Stack[stackTopReg + 2] = ProgAddrReg;
            baseAddrReg = stackTopReg;
            ProgAddrReg = InsReg.offset;
            break;
        case oprType::INT:
            stackTopReg += InsReg.offset;
            break;
        case oprType::JMP:
            ProgAddrReg = InsReg.offset;
            break;
        case oprType::JPC:
            if (Stack[--stackTopReg] == 0)
                ProgAddrReg = InsReg.offset;
            break;
        case oprType::RED:
            scanf("%d", &Stack[InsReg.offset + base(InsReg.level, baseAddrReg)]);
            break;
        case oprType::WRT:
            printf("%d ", Stack[--stackTopReg]);
            break;
        case oprType::POP:
            Stack[stackTopReg + 2] = Stack[stackTopReg - 1];
            --stackTopReg;
            break;
        }
    } while (ProgAddrReg);
}

void Interpreter::printPcode() {
    std::ifstream File(std::filesystem::path(FilePath).replace_extension(".pcode"));
    std::cout << File.rdbuf();
    File.close();
}
void Interpreter::printSymbolTable() {
    std::ifstream File(std::filesystem::path(FilePath).replace_extension(".symbol"));
    std::cout << File.rdbuf();
    File.close();
}

void Interpreter::printTokens() {
    std::ifstream File(std::filesystem::path(FilePath).replace_extension(".token"));
    std::cout << File.rdbuf();
    File.close();
}