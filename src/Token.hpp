#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <sstream>

class Token{
public:
    Token(int token, int line, std::string str):
        token(token), line(line), text(str) {}
    Token(int token, int line, const char* str):
        token(token), line(line), text(str) {}
    const int token;
    const int line;
    const std::string text;
    virtual std::string toString(){
        std::ostringstream oss;
        oss << "Line: " << line << " Token: " << text;
        return oss.str();
    }
};

class CharConst : public Token {};

class NumConst : public Token {};

class BoolConst : public Token {};

class ID : public Token {};

class Invalid : public Token {};

#endif
