#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <sstream>
#include "TextUtils.hpp"

class Token{
public:
    Token(int token, int line, std::string str):
        token(token), line(line), text(str) {}
    Token(int token, int line, cstr str):
        token(token), line(line), text(str) {}
    const int token;
    const int line;
    const std::string text;
    virtual std::string toString(){
        std::ostringstream oss;
        oss << "Line " << line << " Token: " << text;
        return oss.str();
    }
};

class CharConst : public Token {
public:
    const char literal;
    CharConst(int token, int line, cstr str):
        Token(token, line, str), literal(TextUtils::unescape(str)) {}
    std::string toString(){
        std::ostringstream oss;
        oss << "Line " << line << " Token: CHARCONST Value: \'" << literal
            << "\'  Input: \'" << text << "\'";
        return oss.str();
    }
};

class NumConst : public Token {
public:
    const int32_t value;
    NumConst(int token, int line, cstr str):
        Token(token, line, str), value(atoi(str)) {}
    std::string toString(){
        std::ostringstream oss;
        oss << "Line " << line << " Token: NUMCONST Value: " << value
            << "  Input: " << text;
        return oss.str();
    }
};

class BoolConst : public Token {
private:
    static bool atob(cstr str){ return (str[0]=='t'); }
public:
    const bool value;
    BoolConst(int token, int line, cstr str):
        Token(token, line, str), value(atob(str)) {}
    std::string toString(){
        std::ostringstream oss;
        oss << "Line " << line << " Token: BOOLCONST Value: " << value
            << "  Input: " << text;
        return oss.str();
    }
};

class ID : public Token {
public:
    ID(int token, int line, cstr str):
        Token(token, line, str) {}
    std::string toString(){
        std::ostringstream oss;
        oss << "Line " << line << " Token: ID Value: " << text;
        return oss.str();
    }
};

class Invalid : public Token {
public:
    Invalid(int token, int line, cstr str):
        Token(token, line, str) {}
    std::string toString(){
        std::ostringstream oss;
        oss << "ERROR(" << line << "): Invalid or misplaced input character: \""
            << text << "\"";
        return oss.str();
    }
};

#endif
