//
// Created by Administrator on 2021/6/29.
//

#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "common.h"
#include "SymbolTable.h"

int priority(Element&);//calculate the priority of a operator
void genRP(vector<Element>&, vector<Element>&);//Generate Reserve Polish
void Apart(vector<Element>&, vector<Element>&, vector<Element>&);//Get Left Part and Right Part of an assignment

class Parser {
//For Parser
private:
    Token next;
    int curIndex;
public:
    Parser(vector<Token>& TokenList);
    ~Parser() { };
    void showQuaternary(const char* path);
    vector<Quaternary>& RetQtSet();
    void analyse();
//For Generating Quaternary
private:
    int tmpIndex;
    vector<Token> TokenList;
    vector<Quaternary> QtSet;
    vector<Element> TmpExp;
    stack<Element> stEmt;
    Operand MergerLVal(vector<Element>&);
    void solveRP(vector<Element>&);
//For Generating SymTables
private:
    bool isConst;
    bool isGlobal;
    //All for Generate SymbolTable
    //For ConstDef
    void solveConstDef(vector<Element>&);//get a string of decl of an const var or const arr and solve its decl
    int solveConstExp(vector<Element>&);//get the ans of ConstExp because it can be calculated in compiler phase
    void solveArrLVal(ArrayTable&, vector<Element>&);//get the left part of a const arr assignment
    void solveArrRVal(ArrayTable&, vector<Element>&);//get the right part of a const arr assignment
    //For VarDef
    void solveVarDef(vector<Element>&);
    void solveExp(vector<Element>&);
    //for Generate FuncTable
    vectorStr DeclStr;
    void solveFuncDecl();
private:
//For Support Parser
private:
    void getNextToken();
    Token getPeek();
    bool findAssign();
//For Parser
    void S();
    void CompUnit();
    void FuncDecl();
    void FuncFParams();
    void FuncFParam();
    void Block();
    void BlockItem();
    void stmt();
    void Cond();
    void LOrExp();
    void LAndExp();
    void EqExp();
    void RelExp();
    void AddExp();
    void MulExp();
    void UnaryExp();
    void PrimaryExp();
    void LVal();
    void Exp();
    void ConstExp();
    void FuncRParma();
    void ConstDecl();
    void ConstDef();
    void ConstInitVal();
    void VarOrFuncDecl();
    void VarDecl();
    void VarDef();
    void InitVal();
};

#endif //PARSER_H
