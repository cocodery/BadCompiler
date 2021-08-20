//
// Created by Administrator on 2021/7/22.
//

#include "../include/Optimization.h"

Optimizer::Optimizer(vector<Quaternary>& QtSet) {
    DivideBlock(QtSet);
    for(auto& it : Block) {
        UselessCalculation(it);
        Activity(it);
    }
}

void Optimizer::UselessCalculation(BasicBlock& bb) {
    BasicBlock newBlock;
    int len = (int)bb.Blocks.size();
    for(int i = 0; i < len; ++i) {
        if(bb.Blocks[i].Operator == "+") {
            if(bb.Blocks[i].Value1.name == "0" && bb.Blocks[i].Value1.isNum) { //add 0
                if(bb.Blocks[i].Value2.name == bb.Blocks[i].Result.name) { // a = 0 + a
                    continue;
                }
                else { // a = 0 + b;
                    newBlock.Blocks.emplace_back("=", bb.Blocks[i].Value2, epsilon, bb.Blocks[i].Result);
                }
            }
            else if(bb.Blocks[i].Value2.name == "0" && bb.Blocks[i].Value2.isNum) {
                if(bb.Blocks[i].Value1.name == bb.Blocks[i].Result.name) { // a = a + 0
                    continue;
                }
                else { // a = b + 0
                    newBlock.Blocks.emplace_back("=", bb.Blocks[i].Value1, epsilon, bb.Blocks[i].Result);
                }
            }
            else {
                newBlock.Blocks.emplace_back(bb.Blocks[i]);
            }
        }
        else if(bb.Blocks[i].Operator == "-") {
            if(bb.Blocks[i].Value2.name == "0" && bb.Blocks[i].Value2.isNum) {
                if(bb.Blocks[i].Value1.name == bb.Blocks[i].Result.name) { // a = a - 0
                    continue;
                }
                else { // a = b - 0
                    newBlock.Blocks.emplace_back("=", bb.Blocks[i].Value1, epsilon, bb.Blocks[i].Result);
                }
            }
            else {
                newBlock.Blocks.emplace_back(bb.Blocks[i]);
            }
        }
        else if(bb.Blocks[i].Operator == "*") {
            if(bb.Blocks[i].Value1.name == "1" && bb.Blocks[i].Value1.isNum) { //add 0
                if(bb.Blocks[i].Value2.name == bb.Blocks[i].Result.name) { // a = a * 1
                    continue;
                }
                else { // a = 1 * b;
                    newBlock.Blocks.emplace_back("=", bb.Blocks[i].Value2, epsilon, bb.Blocks[i].Result);
                }
            }
            else if(bb.Blocks[i].Value2.name == "1" && bb.Blocks[i].Value2.isNum) {
                if(bb.Blocks[i].Value1.name == bb.Blocks[i].Result.name) { // a = 1 * a
                    continue;
                }
                else { // a = b * 1
                    newBlock.Blocks.emplace_back("=", bb.Blocks[i].Value1, epsilon, bb.Blocks[i].Result);
                }
            }
            else {
                newBlock.Blocks.emplace_back(bb.Blocks[i]);
            }
        }
        else if(bb.Blocks[i].Operator == "/") {
            if(bb.Blocks[i].Value2.name == "1" && bb.Blocks[i].Value2.isNum) {
                if(bb.Blocks[i].Value1.name == bb.Blocks[i].Result.name) { // a = a / 1
                    continue;
                }
                else { // a = b/ 1
                    newBlock.Blocks.emplace_back("=", bb.Blocks[i].Value1, epsilon, bb.Blocks[i].Result);
                }
            }
            else {
                newBlock.Blocks.emplace_back(bb.Blocks[i]);
            }
        }
        else {
            newBlock.Blocks.emplace_back(bb.Blocks[i]);
        }
    }
    bb = newBlock;
}

void Optimizer::assignAc(Map& haveAppeared, Map& activities, Operand& var, bool isResult) {
    if (haveAppeared[var.name]) {
        var.activity = (bool)activities[var.name];
    }
    else {
        var.activity = !var.isTmp;
        haveAppeared[var.name] = 1;
    }
    activities[var.name] = int(!isResult);
}

void Optimizer::Activity(BasicBlock& it) {
    Map haveAppeared;
    Map activities;
    int index = it.Blocks.size() - 1;
    for (; index >= 0; index--) {
        Quaternary &qt = it.Blocks[index];
        if(qt.Result.name != "#") assignAc(haveAppeared, activities, qt.Result, true);
        if(qt.Value2.name != "#") assignAc(haveAppeared, activities, qt.Value2, false);
        if(qt.Value1.name != "#") assignAc(haveAppeared, activities, qt.Value1, false);
    }
}

void Optimizer::showBlocks() {
    ofstream out;
    out.open("123.ir");
    int len = (int)Block.size();
    for(int i = 0; i < len; ++i) {
        int size = Block[i].Blocks.size();
        for(int j = 0; j < size; ++j) {
            out << Block[i].Blocks[j].Operator << ' '
                << Block[i].Blocks[j].Value1.name << '|' << Block[i].Blocks[j].Value1.activity << ' '
                << Block[i].Blocks[j].Value2.name << '|' << Block[i].Blocks[j].Value2.activity << ' '
                << Block[i].Blocks[j].Result.name << '|' << Block[i].Blocks[j].Result.activity << endl;
        }
        out << endl << endl;
    }
    cout << "--- Optimizer Complete ---" << endl;
}

vector<BasicBlock>& Optimizer::RetBlock() {
    return Block;
}

void Optimizer::DivideBlock(vector<Quaternary>& QtSet) {
    int size = QtSet.size();
    if(size == 0) return;

    vectorStr jump = {"do", "we", "while", "if", "else", "ie", "FuncEnd" };
    BasicBlock tmpBlock;
    for(const auto& it : QtSet) {
        string op = it.Operator;
        tmpBlock.Blocks.emplace_back(it);
        if(find(jump.begin(), jump.end(), op) != jump.end()) {
            Block.emplace_back(tmpBlock);
            tmpBlock.Blocks.clear();
        }
    }
}