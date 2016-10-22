#ifndef ERROR_H
#define ERROR_H

#include <iostream>
#include <sstream>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <climits>

#include "Token.hpp"
#include "Type.hpp"
#include "ParseDefines.hpp"

class Error{
private:
    int32_t lineno;
    int32_t tokenIndex;
    std::string description;
public:
    static const int32_t SYNTAX = -2;
    static const int32_t LINKER = INT_MAX;
    Error(int32_t line, int32_t index, std::string description)
        : lineno(line), tokenIndex(index), description(description){}
    Error(const Token* t, std::string description)
        : lineno(t->line), tokenIndex(t->index), description(description){}
    std::string toString() const {
        std::ostringstream oss;
        oss << "ERROR(";
        switch(lineno){
            case LINKER: oss << "LINKER"; break;
            case SYNTAX: oss << "SYNTAX"; break;
            default:     oss << lineno; break;
        }
        oss << "," << tokenIndex << "): " << description;
        return oss.str();
    }
    /**
     * returns a value less than zero if the callee is less than the argument,
     *   a value equal to 0 if they are equal,
     *   a value greater than 0 if the callee is greater that the argument
     */
    int ordering(const Error& rhs) const {
        int lineDiff = lineno - rhs.lineno;
        if(lineDiff != 0) return lineDiff;

        int tokenDiff = tokenIndex - rhs.tokenIndex;
        if(tokenDiff != 0) return tokenDiff;

        return description.compare(rhs.description);
    }
    bool operator< (const Error &b) const { return ordering(b) <  0; }
    bool operator<=(const Error &b) const { return ordering(b) >= 0; }
    bool operator> (const Error &b) const { return ordering(b) >  0; }
    bool operator>=(const Error &b) const { return ordering(b) >= 0; }
    bool operator==(const Error &b) const { return ordering(b) == 0; }
    bool operator!=(const Error &b) const { return ordering(b) != 0; }
};
std::ostream& operator<<(std::ostream&, const Error&);
std::ostream& operator<<(std::ostream&, const Error*);

namespace Errors{
    bool compare(const Error* const lhs, const Error* const rhs);
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
}

#endif
