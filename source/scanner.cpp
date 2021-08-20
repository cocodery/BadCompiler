//
// Created by Administrator on 2021/6/23.
//

#include "../include/scanner.h"

Scanner::Scanner(const char *path) {
    openfile(path);
}

Scanner::~Scanner() {
    if(src.is_open()) {
        src.close();
    }
}

void Scanner::openfile(const char *path) {
    src.open(path, ios::in);
    if(!src.is_open()) {
        cerr << "file " << path << " open error" << endl;
        exit(-1);
    }
}

char Scanner::getNextChar() {
    char c;
    while(src >> c) {
        if(c == ' ' || c == '\t') {
            continue;
        }
        else if(c == '\n') {
            return '\n';
        }
        break;
    }
    if(src.eof()) {
        return 0;
    }
    else return c;
}

Token Scanner::getNextToken() {
    char c = getNextChar();
    switch (c) {
        case '\n':
            return Token(TK_NEWLINE, "\n");
        case '(' :
            return Token(TK_LPAREN, "(");
        case ')':
            return Token(TK_RPAREN, ")");
        case '{':
            return Token(TK_LBRACE, "{");
        case '}':
            return Token(TK_RBRACE, "}");
        case '[':
            return Token(TK_LBRACKET, "[");
        case ']':
            return Token(TK_RBRACKET, "]");
        case '+':
            return Token(TK_ADD, "+");
        case '-':
            return Token(TK_SUB, "-");
        case '*':
            return Token(TK_MUL, "*");
        case '%':
            return Token(TK_MOD, "%");
        case ',':
            return Token(TK_COMMA, ",");
        case ';':
            return Token(TK_SEMICOLON, ";");
        case '=':
            if(src.peek() == '=') {
                src.get();
                return Token(TK_EQUAL, "==");
            }
            else return Token(TK_ASSIGN, "=");
        case '!':
            if(src.peek() == '=') {
                src.get();
                return Token(TK_UNEQUAL, "!=");
            }
            else return Token(TK_NOT, "!");
        case '<':
            if(src.peek() == '=') {
                src.get();
                return Token(TK_LESS_EQ, "<=");
            }
            else return Token(TK_LESS, "<");
        case '>':
            if(src.peek() == '=') {
                src.get();
                return Token(TK_GREAT_EQ, ">=");
            }
            else return Token(TK_GREAT, ">");
        case '&':
            if(src.peek() == '&') {
                src.get();
                return Token(TK_AND, "&&");
            }
            else return Token(TK_AND_BYTE, "&");
        case '|':
            if(src.peek() == '|') {
                src.get();
                return Token(TK_OR, "||");
            }
            else return Token(TK_OR_BYTE, "|");
        case '/':
            if(src.peek() == '/') {
                char buf [1024];
                src.getline(buf, 1024);
                return getNextToken();
            }
            else if(src.peek() == '*') {
                src.get();
                while(src >> c) {
                    if(c == '*') {
                        src >> c;
                        if(c == '/') {
                            break;
                        }
                    }
                }
                return getNextToken();
            }
            else return Token(TK_DIV, "/");
        default:
            if(IsDigit(c)) {
                string buf;
                buf.push_back(c);
                while(true) {
                    c = char(src.peek());
                    if(addDigit(c)) {
                        src >> c;
                        buf += c;
                    }
                    else break;
                }
                int base = checkNum(buf);
                if(base == 0) {

                }
                else {
                    changeBase(buf, base);
                }
                return Token(TK_NUM, buf);
            }
            else if(IsLetter(c)) {
                string buf;
                buf.push_back(c);
                while (true) {
                    c = char(src.peek());
                    if (IsNotFirstLetter(c)) {
                        src >> c;
                        buf += c;
                    }
                    else {
                        break;
                    }
                }
                if (buf == "int") return Token(TK_INT, "int");
                else if (buf == "const") return Token(TK_CONST, "const");
                else if (buf == "void") return Token(TK_VOID, "void");
                else if (buf == "break") return Token(TK_BREAK, "break");
                else if (buf == "continue") return Token(TK_CONTINUE, "continue");
                else if (buf == "else") return Token(TK_ELSE, "else");
                else if (buf == "if") return Token(TK_IF, "if");
                else if (buf == "return") return Token(TK_RETURN, "return");
                else if (buf == "while") return Token(TK_WHILE, "while");
                else return Token(TK_ID, buf);
            }
    }
    return Token(TK_EOF, "EOF");
}

int checkNum(string& str) {
    int len = (int)str.size();
    if(len == 1) {
        return 1;
    }
    else {
        if (str[0] == '0') {
            if (str[1] == 'x' || str[1] == 'X') {
                if (len == 2) {
                    return 0;
                }
                for (int index = 2; index < len; ++index) {
                    if (!Is0XDigit(str[index])) {
                        return 0;
                    }
                }
                return 3;
            }
            else {
                for (int index = 1; index < len; ++index) {
                    if (!IsOcDigit(str[index])) {
                        return 0;
                    }
                }
                return 2;
            }
        } else if (IsDigit(str[0])) {
            return 1;
        } else {
            return 0;
        }
    }
}

void changeBase(string& str, int base) {
    if(base == 1) {
        return;
    }
    int tmpNum = 0;
    int count = 0;
    int len = (int)str.size();
    if(base == 2) {
        for(int index = len - 1; index > 0; --index) {
            tmpNum += int((str[index] - '0') * pow(8, count));
            ++count;
        }
        str =  to_string(tmpNum);
    }
    else {
        for (int index = len - 1; index > 1; --index) {
            if (IsDigit(str[index])) {
                tmpNum += int((str[index] - '0') * pow(16, count));
                ++count;
            } else {
                if (str[index] == 'A' || str[index] == 'a') {
                    tmpNum += 10 * int(pow(16, count));
                } else if (str[index] == 'B' || str[index] == 'b') {
                    tmpNum += 11 * int(pow(16, count));
                } else if (str[index] == 'C' || str[index] == 'c') {
                    tmpNum += 12 * int(pow(16, count));
                } else if (str[index] == 'D' || str[index] == 'd') {
                    tmpNum += 13 * int(pow(16, count));
                } else if (str[index] == 'E' || str[index] == 'e') {
                    tmpNum += 14 * int(pow(16, count));
                } else if (str[index] == 'F' || str[index] == 'f') {
                    tmpNum += 15 * int(pow(16, count));
                }
                ++count;
            }
        }
        str = to_string(tmpNum);
    }
}

void Scanner::analyse() {
    while(true) {
        Token t = getNextToken();
        TokenList.push_back(t);
        if(t.first == TK_EOF) {
            break;
        }
    }
    cout << "--- Scanner Complete ---" << endl;
}

void Scanner::showToken(const char* path) {
    ofstream out;
    out.open(path);
    int len = (int)TokenList.size();
    for(int i = 0; i < len; ++i) {
        out << TokenList[i].first << ' ' << TokenList[i].second << endl;
    }
}

vector<Token>& Scanner::RetTokenList() {
    return TokenList;
}
