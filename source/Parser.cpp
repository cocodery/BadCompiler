//
// Created by Administrator on 2021/6/29.
//

#include "../include/Parser.h"

Parser::Parser(vector<Token>& TokenList) {
    this->TokenList = TokenList;
    curIndex = 0;
    tmpIndex = 0;
    isConst = false;
    isGlobal = true;
}

void Parser::getNextToken() {
    next = TokenList[curIndex];
    ++curIndex;
}

Token Parser::getPeek() {
    return TokenList[curIndex];
}

bool Parser::findAssign() {
    for(int index = curIndex;
        TokenList[index].first != TK_SEMICOLON; ++index) {
        if(TokenList[index].first == TK_ASSIGN) {
            return true;
        }
    }
    return false;
}

int priority(Element& op) {
    if(op.name == "+" || op.name == "-") {
        if(!op.isSignal) return 5;
        else return 7;
    }
    else if(op.name == "*" || op.name == "/" || op.name == "%") return 6;
    else if(op.name == "!") return 7;
    else if(op.name == "=") return 0;
    else if(op.name == "<" || op.name == "<=" || op.name == ">" || op.name == ">=") return 4;
    else if(op.name == "!=" || op.name == "==") return 3;
    else if(op.name == "&&") return 2;
    else if(op.name == "||")  return 1;
    return 0;
}

void genRP(vector<Element>& arrRP, vector<Element>& Exp) {
    int len = (int)Exp.size();
    stack<Element> stOP;
    for(int i = 0; i < len; i++) {
        if(Exp[i].isVar) {
            arrRP.emplace_back(Exp[i]);
        }
        else {
            if(stOP.empty() || priority(Exp[i]) > priority(stOP.top())) {
                stOP.push(Exp[i]);
            }
            else if(Exp[i].name == "(" || Exp[i].name == "[") {
                stOP.push(Exp[i]);
            }
            else if(Exp[i].name == ")") {
                while(!stOP.empty() && stOP.top().name != "(") {
                    arrRP.emplace_back(stOP.top());
                    stOP.pop();
                }
                stOP.pop();
            }
            else if(Exp[i].name == "]") {
                while(!stOP.empty() && stOP.top().name != "[") {
                    arrRP.emplace_back(stOP.top());
                    stOP.pop();
                }
                stOP.pop();
                arrRP.emplace_back("[]", false, false, false);
            }
            else if(priority(Exp[i]) <= priority(stOP.top())) {
                while(!stOP.empty() && priority(stOP.top()) >= priority(Exp[i])) {
                    arrRP.emplace_back(stOP.top());
                    stOP.pop();
                }
                stOP.push(Exp[i]);
            }
        }
    }
    while(!stOP.empty()) {
        arrRP.emplace_back(stOP.top());
        stOP.pop();
    }
}

Operand Parser::MergerLVal(vector<Element>& LVal) {
    string name;
    int len = (int)LVal.size();
    for(int i = 0; i < len; ++i) {
        name += LVal[i].name;
    }
    Operand tmp = Operand(name, true, false, false);
    return tmp;
}

void Parser::solveRP(vector<Element>& Exp) {
    vector<Element> arrRP;
    genRP(arrRP, Exp);
    while(!stEmt.empty()) {
        stEmt.pop();
    }
    Operand value1, value2, result;
    Element tmp1, tmp2;
    string tmpName = "t";
    int len = (int)arrRP.size();
    for(int index = 0; index < len; ++index) {
        if (arrRP[index].isVar) stEmt.push(arrRP[index]);
        else {
            if(arrRP[index].isSignal) {
                if(arrRP[index].name == "=") {
                    tmp2 = stEmt.top();
                    stEmt.pop();
                    value2 = Operand(tmp2.name, false, tmp2.isTmp,IsDigit(tmp2.name[0]));
                    tmp1 = stEmt.top();
                    stEmt.pop();
                    result = Operand(tmp1.name, false, tmp1.isTmp, false);
                    QtSet.emplace_back(Quaternary(arrRP[index].name, value2, epsilon, result));
                }
                else if(arrRP[index].name == "+")  continue;
                else {
                    tmp2 = stEmt.top();
                    stEmt.pop();
                    value2 = Operand(tmp2.name, false, tmp2.isTmp, IsDigit(tmp2.name[0]));
                    result = Operand(tmpName + to_string(tmpIndex), false, true, false);
                    QtSet.emplace_back(Quaternary(arrRP[index].name, value2, epsilon, result));
                    stEmt.push(Element(tmpName + to_string(tmpIndex), true, true, false));
                    ++tmpIndex;
                }
            }
            else {
                tmp2 = stEmt.top();
                stEmt.pop();
                value2 = Operand(tmp2.name, false, tmp2.isTmp, IsDigit(tmp2.name[0]));
                tmp1 = stEmt.top();
                stEmt.pop();
                value1 = Operand(tmp1.name, false, tmp1.isTmp, IsDigit(tmp1.name[0]));
                result = Operand(tmpName + to_string(tmpIndex), false, true, false);
                QtSet.emplace_back(Quaternary(arrRP[index].name, value1, value2, result));
                stEmt.push(Element(tmpName + to_string(tmpIndex), true, true, false));
                ++tmpIndex;
            }
        }
    }
}

void Parser::showQuaternary(const char* path) {
    ofstream out;
    out.open(path);
    int len = (int)QtSet.size();
    for(int i = 0; i < len; ++i) {
        out << QtSet[i].Operator << ' ' <<  QtSet[i].Value1.name << ' '
            << QtSet[i].Value2.name << ' ' << QtSet[i].Result.name << endl;
    }
}

vector<Quaternary>& Parser::RetQtSet() {
    return QtSet;
}

void Parser::analyse() {
    S();
    cout << "--- Parser Complete ---" << endl;
}

//S->CompUnit[CompUnit]
void Parser::S() {
    getNextToken();
    Loop0:
    CompUnit();
    if(next.first == TK_EOF) {
        return;
    }
    else {
        goto Loop0;
    }
}

//CompUnit->'int' VarOrFuncDecl|
//          'const' ConstVarDecl|
//          'void' FuncDecl
void Parser::CompUnit() {
    if(next.first == TK_INT) {
        getNextToken();
        VarOrFuncDecl();
        return;
    }
    else if(next.first == TK_CONST) {
        getNextToken();
        ConstDecl();
        if(next.first == TK_SEMICOLON) {
            getNextToken();
            return;
        }
    }
    else if(next.first == TK_VOID) {
        getNextToken();
        DeclStr.emplace_back("void");
        FuncDecl();
        return;
    }
}

//FuncDecl->Ident '('[FuncFParams]')' Block
void Parser::FuncDecl() {
    isGlobal = true;
    if(next.first == TK_ID) {
        QtSet.emplace_back("Func", Operand(next.second, false, false, false), epsilon, epsilon);
        DeclStr.emplace_back(next.second);
        getNextToken();
        if(next.first == TK_LPAREN) {
            DeclStr.emplace_back("(");
            getNextToken();
            FuncFParams();
            if(next.first == TK_RPAREN) {
                DeclStr.emplace_back(")");
                solveFuncDecl();
                DeclStr.clear();
                getNextToken();
                Block();
                QtSet.emplace_back("FuncEnd", epsilon, epsilon, epsilon);
                return;
            }
        }
    }
    isGlobal = false;
}

//FuncFParams->FuncParam{','FuncParam}
void Parser::FuncFParams() {
    Loop3:
    FuncFParam();
    if(next.first == TK_COMMA) {
        DeclStr.emplace_back(",");
        getNextToken();
        goto Loop3;
    }
    else {
        return;
    }
}

//FuncFParam->'int' Ident['['']'{'['Exp']'}]
//             Epsilon
void Parser::FuncFParam() {
    if(next.first == TK_INT) {
        DeclStr.emplace_back("int");
        getNextToken();
        if(next.first == TK_ID) {
            DeclStr.emplace_back(next.second);
            Loop4:
            getNextToken();
            if(next.first == TK_LBRACKET) {
                DeclStr.emplace_back(next.second);
                getNextToken();
                Loop5:
                if(next.first == TK_RBRACKET) {
                    DeclStr.emplace_back(next.second);
                    goto Loop4;
                }
                else {
                    Exp();
                    goto Loop5;
                }
            }
            else {
                return;
            }
        }
    }
    else {
        return;
    }
}

//Block->'{'BlockItem'}'
void Parser::Block() {
    if(next.first == TK_LBRACE) {
        getNextToken();
        Loop9:
        if(next.first == TK_RBRACE) {
            getNextToken();
            return;
        }
        else {
            BlockItem();
            goto Loop9;
        }
    }
}

//BlockItem->'const' ConstDecl
//           'int' VarDef
//            stmt
void Parser::BlockItem() {
    if(next.first == TK_CONST) {
        getNextToken();
        ConstDecl();
        return;
    }
    else if(next.first == TK_INT) {
        getNextToken();
        VarDecl();
        return;
    }
    else {
        stmt();
        return;
    }
}

//stmt->Block
//      'if''('Cond')'stmt['else' stmt]
//      'while''('Cond')'stmt
//      'break'';'
//      'continue'';'
//      'return'';'
//      LVal '=' Exp ';'
//      [Exp] ';'
void Parser::stmt() {
    if(next.first == TK_LBRACE) {
        Block();
        return;
    }
    else if(next.first == TK_IF) {
        getNextToken();
        if(next.first == TK_LPAREN) {
            getNextToken();
            Cond();
            QtSet.emplace_back("if", Operand(stEmt.top().name, stEmt.top().isTmp, false, false), epsilon, epsilon);
            if(next.first == TK_RPAREN) {
                getNextToken();
                stmt();
                if(next.first == TK_ELSE) {
                    getNextToken();
                    QtSet.emplace_back("else", epsilon, epsilon, epsilon);
                    stmt();
                    QtSet.emplace_back(Quaternary("ie", epsilon, epsilon, epsilon));
                    return;
                }
                else {
                    QtSet.emplace_back(Quaternary("ie", epsilon, epsilon, epsilon));
                    return;
                }
            }
        }
    }
    else if(next.first == TK_WHILE) {
        getNextToken();
        QtSet.emplace_back(Quaternary("while", epsilon, epsilon, epsilon));
        if(next.first == TK_LPAREN) {
            getNextToken();
            Cond();
            QtSet.emplace_back(Quaternary("do", Operand(stEmt.top().name, stEmt.top().isTmp, false, false), epsilon, epsilon));
            if(next.first == TK_RPAREN) {
                getNextToken();
                stmt();
                QtSet.emplace_back(Quaternary("we", epsilon, epsilon, epsilon));
                return;
            }
        }
    }
    else if(next.first == TK_BREAK) {
        getNextToken();
        QtSet.emplace_back(Quaternary("break", epsilon, epsilon, epsilon));
        if(next.first == TK_SEMICOLON) {
            return;
        }
    }
    else if(next.first == TK_CONTINUE) {
        getNextToken();
        QtSet.emplace_back(Quaternary("continue", epsilon, epsilon, epsilon));
        if(next.first == TK_SEMICOLON) {
            getNextToken();
            return;
        }
    }
    else if(next.first == TK_RETURN) {
        getNextToken();
        if(next.first == TK_SEMICOLON) {
            getNextToken();
            QtSet.emplace_back("Ret", epsilon, epsilon, epsilon);
            return;
        }
        else {
            TmpExp.clear();
            Exp();
            solveRP(TmpExp);
            TmpExp.clear();
            if(next.first == TK_SEMICOLON) {
                QtSet.emplace_back("Ret", Operand(stEmt.top().name, false, false, false), epsilon, epsilon);
                getNextToken();
                return;
            }
        }
    }
    else if(next.first == TK_ID) {
        if(findAssign()) {
            TmpExp.clear();
            LVal();
            if(next.first == TK_ASSIGN) {
                getNextToken();
                TmpExp.emplace_back(Element("=", false, true, false));
                Exp();
                vector<Element> LVal, RVal;
                Apart(LVal, RVal, TmpExp);
                solveRP(RVal);
                QtSet.emplace_back("=", Operand(stEmt.top().name, false, false, false), epsilon, MergerLVal(LVal));
                TmpExp.clear();
                if(next.first == TK_SEMICOLON) {
                    getNextToken();
                    return;
                }
            }
        }
        else {
            Exp();
            solveRP(TmpExp);
            TmpExp.clear();
            if(next.first == TK_SEMICOLON) {
                getNextToken();
                return;
            }
        }
    }
    else  {
        if(next.first == TK_SEMICOLON) {
            getNextToken();
            return;
        }
        Exp();
        solveRP(TmpExp);
        TmpExp.clear();
        if(next.first == TK_SEMICOLON) {
            getNextToken();
            return;
        }
        return;
    }
}

//Cond -> LOrExp
void Parser::Cond() {
    TmpExp.clear();
    LOrExp();
    solveRP(TmpExp);
    TmpExp.clear();
    tmpIndex = 0;
}

//LOrExp -> LAndExp ['||' LOrExp ]
void Parser::LOrExp() {
    LAndExp();
    if(next.first == TK_OR) {
        TmpExp.emplace_back(Element(next.second, false, false, false));
        getNextToken();
        LOrExp();
        return;
    }
    else {
        return;
    }
}

//LAndExp -> LAndExp ['&&' LAndExp ]
void Parser::LAndExp() {
    EqExp();
    if(next.first == TK_AND) {
        TmpExp.emplace_back(Element(next.second, false, false, false));
        getNextToken();
        LAndExp();
        return;
    }
    else {
        return;
    }
}

//EqExp -> RelExp [ '!=' | '==' EqExp ]
void Parser::EqExp() {
    RelExp();
    if(next.first == TK_EQUAL || next.first == TK_UNEQUAL) {
        TmpExp.emplace_back(Element(next.second, false, false, false));
        getNextToken();
        EqExp();
        return;
    }
    else {
        return;
    }
}

//RelExp -> AddExp [(<, >, <= ,>=) RelExp]
void Parser::RelExp() {
    AddExp();
    if(next.first == TK_LESS || next.first == TK_LESS_EQ
        || next.first == TK_GREAT || next.first == TK_GREAT_EQ) {
        TmpExp.emplace_back(Element(next.second, false, false, false));
        getNextToken();
        RelExp();
        return;
    }
    else {
        return;
    }
}

//AddExp -> MulExp [(+, -) AddExp]
void Parser::AddExp() {
    MulExp();
    if(next.first == TK_ADD || next.first == TK_SUB) {
        TmpExp.emplace_back(Element(next.second, false, false, false));
        getNextToken();
        AddExp();
        return;
    }
    else {
        return;
    }
}

//MulExp -> UnaryExp [ (*, /, %) MulExp ]
void Parser::MulExp() {
    UnaryExp();
    if(next.first == TK_MUL || next.first == TK_DIV
        || next.first == TK_MOD) {
        TmpExp.emplace_back(Element(next.second, false, false, false));
        getNextToken();
        MulExp();
        return;
    }
    else {
        return;
    }
}

//UnaryExp -> (+, -, !) UnaryExp
//          Ident '(' [FuncRParams] ')'
//          PrimaryExp
void Parser::UnaryExp() {
    if(next.first == TK_ADD || next.first == TK_SUB
        || next.first == TK_NOT) {
        if(next.first != TK_ADD) TmpExp.emplace_back(Element(next.second, false, true, false));
        getNextToken();
        UnaryExp();
        return;
    }
    else if(next.first == TK_ID) {
        Token peek = getPeek();
        //function
        if(peek.first == TK_LPAREN) {
            TmpExp.emplace_back(Element(next.second, true, false, false));
            getNextToken();
            getNextToken();
            if(next.first == TK_RPAREN) {
                getNextToken();
                return;
            }
            else {
                FuncRParma();
                if(next.first == TK_RPAREN) {
                    getNextToken();
                    return;
                }
            }
        }
        //var
        else {
            LVal();
            return;
        }
    }
    else {
        PrimaryExp();
        return;
    }
}

//PrimaryExp -> '(' Exp ')'
//              LVal
//              Number
void Parser::PrimaryExp() {
    if(next.first == TK_LPAREN) {
        TmpExp.emplace_back(Element(next.second, false, false, false));
        getNextToken();
        Exp();
        if(next.first == TK_RPAREN) {
            TmpExp.emplace_back(Element(next.second, false, false, false));
            getNextToken();
            return;
        }
    }
    else if(next.first == TK_NUM) {
        TmpExp.emplace_back(Element(next.second, true, false, false));
        getNextToken();
        return;
    }
    else {
        LVal();
        return;
    }
}

//LVal -> Ident {'[' Exp ']'}
void Parser::LVal() {
    if(next.first == TK_ID) {
        TmpExp.emplace_back(Element(next.second, true, false, false));
        Loop12:
        getNextToken();
        if(next.first == TK_LBRACKET) {
            TmpExp.emplace_back(Element(next.second, false, false, false));
            getNextToken();
            Exp();
            if(next.first == TK_RBRACKET) {
                TmpExp.emplace_back(Element(next.second, false, false, false));
                goto Loop12;
            }
        }
        return;
    }
}

//Exp -> AddExp
void Parser::Exp() {
    AddExp();
}

//ConstExp -> AddExp
void Parser::ConstExp() {
    AddExp();
}

//FuncRParams -> Exp { ',' Exp }
void Parser::FuncRParma() {
    Exp();
    while(next.first == TK_COMMA) {
        getNextToken();
        Exp();
    }
}

//ConstDecl->'int' ConstDef{','ConstDef)';'
void Parser::ConstDecl() {
    isConst = true;
    if(next.first == TK_INT) {
        Loop6:
        getNextToken();
        ConstDef();
        solveConstDef(TmpExp);
        TmpExp.clear();
        if(next.first == TK_COMMA) {
            goto Loop6;
        }
        else if(next.first == TK_SEMICOLON) {
            isConst = false;
            return;
        }
    }
}

//ConstDef->Ident{'['ConstExp']'} '=' ConstInitVal
void Parser::ConstDef() {
    if (next.first == TK_ID) {
        TmpExp.emplace_back(next.second, true, false, false);
        getNextToken();
        if (next.first == TK_LBRACKET) {
            while(next.first == TK_LBRACKET) {
                TmpExp.emplace_back(next.second, false, false, false);
                getNextToken();
                ConstExp();
                if(next.first == TK_RBRACKET) {
                    TmpExp.emplace_back(next.second, false, false, false);
                    getNextToken();
                }
            }
        }
        if (next.first == TK_ASSIGN) {
            TmpExp.emplace_back(next.second, false, false, false);
            getNextToken();
            ConstInitVal();
        }
    }
}

//VarOrFuncDecl->FuncDef|VarDef
void Parser::VarOrFuncDecl() {
    if(next.first == TK_ID) {
        Token peek = getPeek();
        if (peek.first == TK_LPAREN) {
            DeclStr.emplace_back("int");
            FuncDecl();
            return;
        }
        else {
            VarDecl();
            return;
        }
    }
}

//VarDecl->VarDef{','VarDef}';'
void Parser::VarDecl() {
    isConst = false;
    Loop9:
    VarDef();
    solveVarDef(TmpExp);
    TmpExp.clear();
    if(next.first == TK_COMMA) {
        getNextToken();
        goto Loop9;
    }
    else if(next.first == TK_SEMICOLON) {
        getNextToken();
        isConst = false;
    }
}

//VarDef->Ident{'['ConstExp']'}['='InitVal]
void Parser::VarDef() {
    if (next.first == TK_ID) {
        TmpExp.emplace_back(next.second, false, false, false);
        getNextToken();
        if (next.first == TK_LBRACKET) {
            while(next.first == TK_LBRACKET) {
                TmpExp.emplace_back(next.second, false, false, false);
                getNextToken();
                ConstExp();
                if(next.first == TK_RBRACKET) {
                    TmpExp.emplace_back(next.second, false, false, false);
                    getNextToken();
                }
            }
        }
        if (next.first == TK_ASSIGN) {
            TmpExp.emplace_back(next.second, false, false, false);
            getNextToken();
            InitVal();
            return;
        }
        else {
            return;
        }
    }
}

//InitVal -> Exp
//          '{' [ InitVal { ',' InitVal } ] '}'
void Parser::InitVal() {
    if(next.first == TK_LBRACE) {
        TmpExp.emplace_back(next.second, false, false, false);
        getNextToken();
        if(next.first == TK_RBRACE) {
            TmpExp.emplace_back(next.second, false, false, false);
            getNextToken();
            return;
        }
        else {
            InitVal();
            while(next.first == TK_COMMA) {
                TmpExp.emplace_back(next.second, false, false, false);
                getNextToken();
                InitVal();
            }
            if(next.first == TK_RBRACE) {
                TmpExp.emplace_back(next.second, false, false, false);
                getNextToken();
                return;
            }
        }
    }
    else {
        Exp();
        return;
    }
}

//ConstInitVal -> ConstExp
//              '{' [ ConstInitVal { ',' ConstInitVal } ] '}'
void Parser::ConstInitVal() {
    if(next.first == TK_LBRACE) {
        TmpExp.emplace_back(next.second, false, false, false);
        getNextToken();
        if(next.first == TK_RBRACE) {
            TmpExp.emplace_back(next.second, false, false, false);
            getNextToken();
            return;
        }
        else {
            ConstInitVal();
            while(next.first == TK_COMMA) {
                TmpExp.emplace_back(next.second, false, false, false);
                getNextToken();
                ConstInitVal();
            }
            if(next.first == TK_RBRACE) {
                TmpExp.emplace_back(next.second, false, false, false);
                getNextToken();
                return;
            }
        }
    }
    else {
        ConstExp();
        return;
    }
}

void Apart(vector<Element>& LVal, vector<Element>& RVal, vector<Element>& tmpInit) {
    bool isLVal = true;
    int size = (int)tmpInit.size();
    for(int i = 0; i < size; ++i) {
        if(tmpInit[i].name == "=") {
            isLVal = false;
            continue;
        }
        if(isLVal) LVal.emplace_back(tmpInit[i]);
        else RVal.emplace_back(tmpInit[i]);
    }
}

void Parser::solveConstDef(vector<Element>& tmpInit) {
    //Get left part and right part of an assignment exp
    vector<Element> LVal;
    vector<Element> RVal;
    Apart(LVal, RVal, tmpInit);
    //const var init
    if(LVal.size() == 1) {
        //type const can get init
        int init = solveConstExp(RVal);
        if(isGlobal) GlobalTable.emplace_back(LVal[0].name, init, isConst);
        else LocalTable.emplace_back(LVal[0].name, init, isConst);
    }
    //Arr init
    else {
        ArrayTable tmpTable;
        tmpTable.name = LVal[0].name;
        solveArrLVal(tmpTable, LVal);
        solveArrRVal(tmpTable, RVal);
        tmpTable.isConst = isConst;
        ArrayVar.emplace_back(tmpTable);
    }
}

int Parser::solveConstExp(vector<Element>& RVal) {
    if (RVal.size() == 1) {
        if (IsDigit(RVal[0].name[0])) {
            return atoi(RVal[0].name.c_str());
        }
    }
    vector<Element> arrRP;
    genRP(arrRP, RVal);
    cout << endl;
    stack<int> stAns;
    int tmp1, tmp2;
    int len = (int) arrRP.size();
    for (int index = 0; index < len; ++index) {
        if (arrRP[index].isVar) {
            if(IsDigit(arrRP[index].name[0])) {
                stAns.push(atoi(arrRP[index].name.c_str()));
            }
            else {
                if(inGlobal(arrRP[index].name)) {
                    stAns.push(IntGlobalTable((arrRP[index].name)));
                }
                else if(inLocal((arrRP[index].name))) {
                    stAns.push(IntLocalTable(arrRP[index].name));
                }
                else if(inArr(arrRP[index].name)) {

                }
                else if(inFunc(arrRP[index].name)) {

                }
            }
        }
        else {
            if(arrRP[index].isSignal) {
                tmp1 = stAns.top();
                stAns.pop();
                if(arrRP[index].name == "-") stAns.push(-tmp1);
                else {
                    if(tmp1) stAns.push(0);
                    else stAns.push(1);
                }
            }
            else {
                tmp2 = stAns.top();
                stAns.pop();
                tmp1 = stAns.top();
                stAns.pop();
                if(arrRP[index].name == "+") {
                    stAns.push(tmp1 + tmp2);
                }
                else if(arrRP[index].name == "-") {
                    stAns.push(tmp1 - tmp2);
                }
                else if(arrRP[index].name == "*") {
                    stAns.push(tmp1 * tmp2);
                }
                else if(arrRP[index].name == "/") {
                    stAns.push(tmp1 / tmp2);
                }
                else if(arrRP[index].name == "%") {
                    stAns.push(tmp1 % tmp2);
                }
            }
        }
    }
    return stAns.top();
}

void Parser::solveArrLVal(ArrayTable& tmpTable, vector<Element>& LVal) {
    int len = (int)LVal.size();
    int inBracket = 0;
    vector<Element> tmpSize;
    for(int i = 1; i < len; ++i) {
        if(LVal[i].name == "[") {
            ++inBracket;
            if(inBracket == 1) continue;
        }
        else if(LVal[i].name == "]") {
            --inBracket;
        }
        if(inBracket > 0) {
            tmpSize.emplace_back(LVal[i]);
        }
        else if(inBracket == 0) {
            int size = solveConstExp(tmpSize);
            tmpTable.size.emplace_back(size);
            tmpSize.clear();
        }
    }
}

void Parser::solveArrRVal(ArrayTable& tmpTable, vector<Element>& RVal) {
    int len = (int)RVal.size();
    int size = (int)tmpTable.size.size();
    int total[1005];
    for(int i = size - 1; i >= 0; --i) {
        if(i == size - 1) total[i] = tmpTable.size[i];
        else total[i] = tmpTable.size[i] * total[i + 1];
    }
    if(len == 0) {
        for(int i = 0; i < total[0]; ++i) {
            tmpTable.init.emplace_back(0);
        }
    }
    int inWhichLevel = -1;
    int curNum[1005];
    memset(curNum, 0, sizeof(curNum));
    vector<Element> tmpUnit;//init for each signal byte
    for(int i = 0; i < len; ++i) {
        if(RVal[i].name == "{") {
            ++inWhichLevel;
            continue;
        }
        if(RVal[i].name == "," || RVal[i].name == "}") {
            if(!tmpUnit.empty()) {
                int unit = solveConstExp(tmpUnit);//calculate current unit
                tmpTable.init.emplace_back(unit);
                tmpUnit.clear();
                for (int j = 0; j <= inWhichLevel; ++j) {
                    ++curNum[j];
                }
            }
            if(RVal[i].name == "}") {
                int diff = total[inWhichLevel] - curNum[inWhichLevel];
                for(int k = 0; k < diff; ++k) {
                    tmpTable.init.emplace_back(0);
                    for(int j = 0; j <= inWhichLevel; ++j) {
                        ++curNum[j];
                    }
                }
                curNum[inWhichLevel] = 0;
                --inWhichLevel;
            }
        }
        else {
            tmpUnit.emplace_back(RVal[i]);
        }
    }
}

void Parser::solveVarDef(vector<Element>& tmpInit) {
    //Get left part and right part of an assignment exp
    vector<Element> LVal;
    vector<Element> RVal;
    Apart(LVal, RVal, tmpInit);
    //const var init
    if(LVal.size() == 1) {
        //Global var can get init
        if(isGlobal) {
            if(!RVal.empty()) {//assign right var
                int init = solveConstExp(RVal);
                GlobalTable.emplace_back(LVal[0].name, init, isConst);
            }
            else {//without assigning right var
                GlobalTable.emplace_back(LVal[0].name, unInit, isConst);
            }
        }
        //Local var's initvar cant be solved
        else LocalTable.emplace_back(LVal[0].name, unInit, isConst);
        //needed to assign
    }
    //Arr init
    else {
        ArrayTable tmpTable;
        tmpTable.name = LVal[0].name;
        solveArrLVal(tmpTable, LVal);
        int len = tmpTable.size.size();
        int total = 1;
        for(int i = 0; i < len; ++i) {
            total *= tmpTable.size[i];
        }
        if(isGlobal) {
            if(!RVal.empty()) solveArrRVal(tmpTable, RVal);
        }
        tmpTable.isConst = isConst;
        ArrayVar.emplace_back(tmpTable);
    }
}

void Parser::solveFuncDecl() {
    int len = (int)DeclStr.size();
    FunctionTable tmpFuncTable;
    bool isArr;
    tmpFuncTable.name = DeclStr[1];
    tmpFuncTable.RetType = DeclStr[0] == "void" ? ret_Void : ret_Int;
    for(int i = 3; i < len; ++i) {
        if(DeclStr[i] == "," || DeclStr[i] == ")") {
            if(isArr) {
                tmpFuncTable.paramsType.emplace_back(type_Arr);
                isArr = false;
            }
            else {
                tmpFuncTable.paramsType.emplace_back(type_Int);
                isArr = false;
            }
            continue;

        }
        if(DeclStr[i] == "[" || DeclStr[i] == "]") {
            isArr = true;
            continue;
        }
        else if(DeclStr[i] == "int") {
            continue;
        }
        else {
            tmpFuncTable.paramsName.emplace_back(DeclStr[i]);
        }
    }
    FuncTable.emplace_back(tmpFuncTable);
}
