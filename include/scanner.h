//
// Created by Administrator on 2021/6/23.
//

#ifndef SCANNER_H
#define SCANNER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "common.h"
using namespace std;

int checkNum(string&);
void changeBase(string&, int);

class Scanner {
private:
    ifstream src;
    vector<Token> TokenList;
private:
    void openfile(const char* path);
    char getNextChar();
    Token getNextToken();
public:
    Scanner(const char* path);
    ~Scanner();
    void analyse();
    void showToken(const char* path);
    vector<Token>& RetTokenList();
};

#endif //SCANNER_H