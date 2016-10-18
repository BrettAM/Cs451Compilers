#ifndef ERROR_H
#define ERROR_H

#include <iostream>
#include <sstream>
#include <stdint.h>
#include <stdlib.h>
#include <string>

#include "Token.hpp"
#include "Type.hpp"

class Error{
private:
    int32_t lineno;
    std::string description;
public:
    static const int32_t SYNTAX = -2;
    static const int32_t LINKER = -1;
    Error(int32_t line, std::string description)
        : lineno(line), description(description){}
    std::string toString() const {
        std::ostringstream oss;
        oss << "ERROR(";
        switch(lineno){
            case LINKER: oss << "LINKER"; break;
            case SYNTAX: oss << "SYNTAX"; break;
            default:     oss << lineno; break;
        }
        oss << "): " << description;
        return oss.str();
    }
};
std::ostream& operator<<(std::ostream&, const Error&);
std::ostream& operator<<(std::ostream&, const Error*);

namespace Errors{
    Error* cannotBeCalled(const Token* t);
    Error* incorrectLHS(const Token* t, Type found, Type required);
    Error* incorrectLHS(const Token* t, Type found, std::string required);
    Error* incorrectRHS(const Token* t, Type found, Type required);
    Error* incorrectRHS(const Token* t, Type found, std::string required);
    Error* mismatchedLR(const Token* t, Type lhs, Type rhs);
    Error* badArrayIndex(const Token* t, Type received);
    Error* arrayIndexedByArray(const Token* t);
    Error* cannotIndexNonarray(const Token* t);
    Error* cannotIndexNonarray(int lineno);
    Error* cannotReturnArray(const Token* t);
    Error* functionUsedAsVar(const Token* func);
    Error* alreadyDefined(const Token* t, int previousLine);
    Error* notDefined(const Token* t);
    Error* opDoesntAcceptArrays(const Token* op);
    Error* opOnlyAcceptsArrays(const Token* op);
    Error* unaryTypeMismatch(const Token* op, Type found, Type required);
    Error* missingMainFunction();
}

#endif
