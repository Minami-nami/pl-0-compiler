#include "Error.h"

constexpr int maxErrCnt = 128;

std::string toString(ERROR eType) {
    switch (eType) {
        case ERROR::ILEGAL_CHARACTER: return "Ilegal Character: ";
        case ERROR::ILEGAL_IDENTIFIER: return "Ilegal Identifier: ";
        case ERROR::MISSING_BECOMES: return "Missing Becomes(:=): ";
        case ERROR::MISSING_DO: return "Missing Do: ";
        case ERROR::MISSING_STATEMENT: return "Missing Statement: ";
        case ERROR::MISSING_ELSE: return "Missing Else: ";
        case ERROR::EXPECTING_PROGRAM: return "Expecting Keyword(program): ";
        case ERROR::EXPECTING_SEMICOLON: return "Expecting Semicolon: ";
        case ERROR::EXPECTING_FACTOR: return "Expecting Factor: ";
        case ERROR::EXPECTING_BEGIN: return "Expecting Begin: ";
        case ERROR::EXPECTING_END: return "Expecting End: ";
        case ERROR::EXPECTING_CONST: return "Expecting Const: ";
        case ERROR::EXPECTING_IDENTIFIER: return "Expecting Identifier: ";
        case ERROR::EXPECTING_INTEGER: return "Expecting Integer: ";
        case ERROR::EXPECTING_LPAREN: return "Expecting Lparen: ";
        case ERROR::EXPECTING_RPAREN: return "Expecting Rparen: ";
        case ERROR::EXPECTING_STATEMENT: return "Expecting Statement: ";
        case ERROR::EXPECTING_CMPOPERATOR: return "Expecting Cmpoperator(> < = <> >= <=): ";
        case ERROR::EXPECTING_ADDOPERATOR: return "Expecting Add Operator(+ -): ";
        case ERROR::EXPECTING_MULOPERATOR: return "Expecting Muloperator(* /): ";
        case ERROR::EXPECTING_ODD: return "Expecting Odd: ";
        case ERROR::EXPECTING_EXPR: return "Expecting Expr: ";
        case ERROR::EXPECTING_DO: return "Expecting Do: ";
        case ERROR::EXPECTING_THEN: return "Expecting Then: ";
        case ERROR::EXPECTING_PROCEDURE: return "Expecting Procedure: ";
        case ERROR::MISSING_SEMICOLON: return "Missing Semicolon(;):";
        case ERROR::MISSING_COMMA: return "Missing Comma(,): ";
        case ERROR::MISSING_IDENTIFIER: return "Missing Identifer: ";
        case ERROR::MISSING_INTEGER: return "Missing Integer: ";
        case ERROR::MISSING_CMPOPERATOR: return "Missing Cmpoperator(> < = <> >= <=): ";
        case ERROR::MISSING_ADDOPERATOR: return "Missing Addoperator(+ -): ";
        case ERROR::MISSING_MULOPERATOR: return "Missing Muloperator(* /): ";
        case ERROR::MISSING_LEXPR: return "Missing Lexpr: ";
        case ERROR::MISSING_EXPR: return "Missing Expr: ";
        case ERROR::MISSING_LPAREN: return "Missing Lparen: ";
        case ERROR::MISSING_RPAREN: return "Missing Rparen: ";
        case ERROR::MISSING_THEN: return "Missing Then: ";
        case ERROR::MISSING_COLON: return "Missing Colon: ";
        case ERROR::UNEXPECTED_TOKEN: return "Unexpected Token: ";
        case ERROR::OUT_OF_RANGE: return "Integer Out Of Range:";
        default: return "Unknown Error";
    }

}

void Error::ProcError(ERROR etype, int row, int col, const std::string& token, const std::string& line, const std::string& path) {
    if (++ErrCnt > maxErrCnt) {
        printf("Too much error(%d)!\n", ErrCnt);
        exit(0);
    }
    printf("%s:%d:%d: error: %s\n", path.c_str(), row, col, toString(etype).c_str());
    printf("%5d |     %s", row, line.c_str());
    printf("      |     ");
    int curcol = 1;
    while((int)curcol++ < col) {
        putchar(' ');
    }
    putchar('^');
    curcol = 1;
    while (curcol++ < token.size()) {
        putchar('~');
    }
    putchar('\n');
}