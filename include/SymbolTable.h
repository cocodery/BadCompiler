//
// Created by Administrator on 2021/7/12.
//

#ifndef SymbolTable_H
#define SymbolTable_H

#include <iostream>
#include <vector>
#include <string>
#include "common.h"
using namespace std;

enum Type {
    type_Int, type_Arr
};

enum ReType {
    ret_Int, ret_Void, ret_Null
};

class Symtable {
public:
    string name;
    int init;
    bool isConst;
public:
    Symtable();
    Symtable(string, int, bool);
    ~Symtable() { };
};

class ArrayTable {
public:
    string name;
    vectorInt size;
    vectorInt init;
    bool isConst;
public:
    ArrayTable();
    ArrayTable(string, vectorInt, vectorInt,bool);
    ~ArrayTable() { };
};

class FunctionTable {
public:
    string name;
    ReType RetType;
    vectorStr paramsName;
    vector<Type> paramsType;
public:
    FunctionTable();
    FunctionTable(string, ReType, vectorStr, vector<Type>);
    ~FunctionTable() { };
};

extern vector<Symtable> GlobalTable;
extern vector<Symtable> LocalTable;
extern vector<ArrayTable> ArrayVar;
extern vector<FunctionTable> FuncTable;

void showSymTable();
void addTmpVar();

bool inGlobal(string&);
bool inLocal(string&);
bool inArr(string&);
bool inFunc(string&);
int IntGlobalTable(string&);
int IntLocalTable(string&);
int indexOfArr(string&);

#endif //SymbolTable_H
