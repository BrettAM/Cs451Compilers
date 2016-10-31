#ifndef ERROR_H
#define ERROR_H

#include <iostream>
#include <sstream>
#include <stdint.h>
#include <stdlib.h>
#include <string>

#include "Token.hpp"
#include "Type.hpp"
#include "ParseDefines.hpp"

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
    Error* cannotReturnArray(const Token* t);
    Error* functionUsedAsVar(const Token* func);
    Error* alreadyDefined(const Token* t, int previousLine);
    Error* notDefined(const Token* t);
    Error* opDoesntAcceptArrays(const Token* op);
    Error* opOnlyAcceptsArrays(const Token* op);
    Error* unaryTypeMismatch(const Token* op, Type found, Type required);
    Error* missingMainFunction();
    Error* arrayUsedAsTest(const Token* t);
    Error* badTestType(const Token* t, Type found);
    Error* mismatchedArrayStatus(const Token* t);
    Error* nonconstInitializer(const Token* t);
    Error* badInitializerType(const Token* t, Type found, Type expected);
    Error* missingReturnStatement(const Token* t, Type found);
    Error* badReturnValue(const Token* t, const Token* function, Type found, Type expected);
    Error* breakOutsideLoop(const Token* t);
    Error* tooFewParameters(const Token* call, const Token* function);
    Error* badParameterType(const Token* t, Type found, Type expected, int parmIndex, const Token* function);
    Error* expectedArrayParameter(const Token* t, int parmIndex, const Token* function);
    Error* unexpectedArrayParameter(const Token* t, int parmIndex, const Token* function);
    Error* tooManyParameters(const Token* t, const Token* function);
    Error* undefinedFunction(const Token* t);
}

#endif
