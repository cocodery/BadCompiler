#include "include/Parser.h"
#include "include/Optimization.h"

int main(int argc, char* argv[]) {
    Scanner Lex(argv[4]);
    Lex.analyse();
    Lex.showToken("123.tk");

    Parser Par(Lex.RetTokenList());
    Par.analyse();
    Par.showQuaternary("123.qt");

    showSymTable();

    Optimizer Ir(Par.RetQtSet());
    Ir.showBlocks();

    cout << "--- Compiler Complete ---";
    return 0;
}
