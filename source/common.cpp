//
// Created by Administrator on 2021/7/5.
//

#include "../include/common.h"

Element::Element() {
    name = "unknown";
    isVar = false;
    isSignal = false;
    isTmp = false;
}

Element::Element(string _name, bool _var, bool _signal, bool _tmp = false) :
    isVar(_var), isSignal(_signal), isTmp(_tmp) {
    name = _name;
}

Operand::Operand() : name("#"), activity(false),
    isTmp(false), isNum(false) {
}

Operand::Operand(string _name, bool _ac, bool _Tmp, bool _Num) :
    activity(_ac), isTmp(_Tmp), isNum(_Num) {
    name = _name;
}

Quaternary::Quaternary() {
    Operator = "#";
    Value1 = epsilon;
    Value2 = epsilon;
    Result = epsilon;
}

Quaternary::Quaternary(string op, Operand v1, Operand v2, Operand r) :
    Value1(v1), Value2(v2), Result(r) {
    Operator = op;
}
