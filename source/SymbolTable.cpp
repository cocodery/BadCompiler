//
// Created by Administrator on 2021/7/21.
//

#include "../include/SymbolTable.h"

Symtable::Symtable() {
    name = "unknown";
    init = 0;
    isConst = false;
}

Symtable::Symtable(string _str, int _int, bool is) :
    name(_str), init(_int), isConst(is) {
}

ArrayTable::ArrayTable() {
    name ="unknown";
    size = vectorInt (0);
    init = vectorInt (0);
    isConst = false;
}

ArrayTable::ArrayTable(string _name, vectorInt _size, vectorInt _init, bool _ch) :
    name(_name), size(_size), init(_init), isConst(_ch) {
}

FunctionTable::FunctionTable() {
    name = "unknown";
    RetType = ret_Null;
    paramsName = vectorStr (0);
    paramsType = vector<Type> (0);
}

FunctionTable::FunctionTable(string _name, ReType _ret, vectorStr _str, vector<Type> _type) :
    name(_name), RetType(_ret), paramsName(_str), paramsType(_type) {
}

vector<Symtable> GlobalTable;
vector<Symtable> LocalTable;
vector<ArrayTable> ArrayVar;
vector<FunctionTable> FuncTable;

void showSymTable() {
    ofstream out;
    out.open("123.st");
    out << "GlobalTable :" << endl;
    int size = (int)GlobalTable.size();
    for(int i = 0; i < size; ++i) {
        out << GlobalTable[i].name << ' ' << GlobalTable[i].init << endl;
    }

    out << "LocalTable :" << endl;
    size = (int)LocalTable.size();
    for(int i = 0; i < size; ++i) {
        out << LocalTable[i].name << ' ' << LocalTable[i].init << endl;
    }

    out << "ArrTable :" << endl;
    size = (int)ArrayVar.size();
    for(int i = 0; i < size; ++i) {
        out << ArrayVar[i].name << " size ";
        int len = (int)ArrayVar[i].size.size();
        for(int j = 0; j < len; ++j) {
            out << ArrayVar[i].size[j] << ' ';
        }
        out << "init ";
        len = (int)ArrayVar[i].init.size();
        for(int j = 0; j < len; ++j) {
            out << ArrayVar[i].init[j] << ' ';
        }
        out << endl;
    }

    out << "FuncTable :" << endl;
    size = (int)FuncTable.size();
    for(int i = 0; i < size; ++i) {
        out << FuncTable[i].name << " RetType " << FuncTable[i].RetType << " ParamType ";
        int len = (int)FuncTable[i].paramsType.size();
        for(int j = 0; j < len; ++j) {
            out << FuncTable[i].paramsType[j] << ' ';
        }
        out << "ParamName ";
        len = (int)FuncTable[i].paramsName.size();
        for(int j = 0; j < len; ++j) {
            out << FuncTable[i].paramsName[j] << ' ';
        }
        out << endl;
    }
    cout << "--- SymTable Complete ---" << endl;
}

bool inGlobal(string& name) {
    int len = (int)GlobalTable.size();
    for(int i = 0; i < len; ++i) {
        if(GlobalTable[i].name == name) {
            return true;
        }
    }
    return false;
}

bool inLocal(string& name) {
    int len = (int)LocalTable.size();
    for(int i = 0; i < len; ++i) {
        if(LocalTable[i].name == name) {
            return true;
        }
    }
    return false;
}

bool inArr(string& name) {
    int len = (int)ArrayVar.size();
    for(int i = 0; i < len; ++i) {
        if(ArrayVar[i].name == name) {
            return true;
        }
    }
    return false;
}

bool inFunc(string& name) {
    int len = (int)FuncTable.size();
    for(int i = 0; i < len; ++i) {
        if(FuncTable[i].name == name) {
            return true;
        }
    }
    return false;
}

int IntGlobalTable(string& name) {
    int size = (int)GlobalTable.size();
    for(int i = 0; i < size; ++i) {
        if(name == GlobalTable[i].name) {
            return GlobalTable[i].init;
        }
    }
}

int IntLocalTable(string& name) {
    int size = (int)LocalTable.size();
    for(int i = 0; i < size; ++i) {
        if(name == LocalTable[i].name) {
            return LocalTable[i].init;
        }
    }
}

int indexOfArr(string& name) {
    int size = (int)ArrayVar.size();
    for(int i = 0; i < size; ++i) {
        if(name == ArrayVar[i].name) {
            return i;
        }
    }
}