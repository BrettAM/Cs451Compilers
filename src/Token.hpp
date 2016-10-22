#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>
#include <sstream>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include "TextUtils.hpp"
#include "Type.hpp"

class Token {
public:
    Token(int token, int line, int index, std::string str):
        token(token), line(line), index(index), text(str) {}
    Token(int token, int line, int index, cstr str):
        token(token), line(line), index(index), text(str) {}
    virtual ~Token() {}
    const int token;
    const int line;
    const int index;
    const std::string text;
    /**
     * Text description of this token
     */
    virtual std::string toString() const {
        std::ostringstream oss;
        oss << "Line " << line << " Token: " << text;
        return oss.str();
    }
    /**
     * Label for this token to appear in the AST listing
     */
    virtual std::string nodeLabel() const {
        return text;
    }
    /**
     * Box describing what line the token occurs on for AST listings
     */
    std::string lineBox() const {
        std::ostringstream oss;
        oss << "[line: " << line << "]";
        return oss.str();
    }
    /**
     * Get the type associated with this token literal if applicable
     */
    virtual Type getType() const {
        return Type::NONE;
    }
    bool operator==(const Token& r) const {
        return this->equals(r);
    }
protected:
    virtual bool equals(const Token& t) const {
        return token==t.token && line==t.line && index==t.index && text==t.text;
    }
};
std::ostream& operator<<(std::ostream&, const Token&);
std::ostream& operator<<(std::ostream&, const Token*);

class CharConst : public Token {
public:
    const char literal;
    CharConst(int token, int line, int index, cstr str):
        Token(token, line, index, str), literal(TextUtils::unescape(str+1)) {}
    std::string toString() const {
        std::ostringstream oss;
        oss << "Line " << line << " Token: CHARCONST Value: \'" << literal
            << "\'  Input: " << text;
        return oss.str();
    }
    std::string nodeLabel() const {
        char conv[4] = {'\'', literal, '\'', '\0'};
        return std::string(conv);
    }
    Type getType() const {
        return Type::CHAR;
    }
protected:
    bool equals(const Token& r) const {
        if(CharConst const * p = dynamic_cast<CharConst const *>(&r)){
            return Token::equals(r) && literal == p->literal;
        }
        return false;
    }
};

class NumConst : public Token {
public:
    const int32_t value;
    NumConst(int token, int line, int index, cstr str):
        Token(token, line, index, str), value(atoi(str)) {}
    std::string toString() const {
        std::ostringstream oss;
        oss << "Line " << line << " Token: NUMCONST Value: " << value
            << "  Input: " << text;
        return oss.str();
    }
    Type getType() const {
        return Type::INT;
    }
protected:
    bool equals(const Token& r) const {
        if(NumConst const * p = dynamic_cast<NumConst const *>(&r)){
            return Token::equals(r) && value == p->value;
        }
        return false;
    }
};

class BoolConst : public Token {
private:
    static bool atob(cstr str){ return (str[0]=='t'); }
public:
    const bool value;
    BoolConst(int token, int line, int index, cstr str):
        Token(token, line, index, str), value(atob(str)) {}
    std::string toString() const {
        std::ostringstream oss;
        oss << "Line " << line << " Token: BOOLCONST Value: " << value
            << "  Input: " << text;
        return oss.str();
    }
    Type getType() const {
        return Type::BOOL;
    }
protected:
    bool equals(const Token& r) const {
        if(BoolConst const * p = dynamic_cast<BoolConst const *>(&r)){
            return Token::equals(r) && value == p->value;
        }
        return false;
    }
};

class IdToken : public Token {
public:
    IdToken(int token, int line, int index, cstr str):
        Token(token, line, index, str) {}
    std::string toString() const {
        std::ostringstream oss;
        oss << "Line " << line << " Token: ID Value: " << text;
        return oss.str();
    }
protected:
    bool equals(const Token& r) const {
        if(dynamic_cast<IdToken const *>(&r)){
            return Token::equals(r);
        }
        return false;
    }
};

class Invalid : public Token {
public:
    Invalid(int token, int line, int index, cstr str):
        Token(token, line, index, str) {}
    std::string toString() const {
        std::ostringstream oss;
        oss << "ERROR(" << line << "): Invalid or misplaced input character: \""
            << text << "\"";
        return oss.str();
    }
protected:
    bool equals(const Token& r) const {
        if(dynamic_cast<Invalid const *>(&r)){
            return Token::equals(r);
        }
        return false;
    }
};

#endif
