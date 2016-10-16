#include "Error.hpp"

using namespace std;

ostream& operator<<(ostream& os, const Error& e){
    os << e.toString();
    return os;
}

ostream& operator<<(ostream& os, const Error* e){
    os << e->toString();
    return os;
}

Error* Errors::cannotBeCalled(const Token* t){
    return new Error(0,"ERROR(%d): '%s' is a simple variable and cannot be called.");
}
Error* Errors::incorrectLHS(const Token* t, Type found, Type required){
    return new Error(0,"ERROR(%d): '%s' requires operands of %s but lhs is of %s.");
}
Error* Errors::incorrectRHS(const Token* t, Type found, Type required){
    return new Error(0,"ERROR(%d): '%s' requires operands of %s but rhs is of %s.");
}
Error* Errors::mismatchedLR(const Token* t, Type lhs, Type rhs){
    return new Error(0,"ERROR(%d): '%s' requires operands of the same type but lhs is %s and rhs is %s.");
}
Error* Errors::badArrayIndex(const Token* t, Type received){
    return new Error(0,"ERROR(%d): Array '%s' should be indexed by type int but got %s.");
}
Error* Errors::arrayIndexedByArray(const Token* t){
    return new Error(0,"ERROR(%d): Array index is the unindexed array '%s'.");
}
Error* Errors::cannotIndexNonarray(const Token* t){
    return new Error(0,"ERROR(%d): Cannot index nonarray '%s'.");
}
Error* Errors::cannotIndexNonarray(int lineno){
    return new Error(0,"ERROR(%d): Cannot index nonarray.");
}
Error* Errors::cannotReturnArray(const Token* t){
    return new Error(0,"ERROR(%d): Cannot return an array.");
}
Error* Errors::functionUsedAsVar(const Token* func){
    return new Error(0,"ERROR(%d): Cannot use function '%s' as a variable.");
}
Error* Errors::alreadyDefined(std::string name, int previousLine){
    return new Error(0,"ERROR(%d): Symbol '%s' is already defined at line %d.");
}
Error* Errors::notDefined(const Token* t){
    return new Error(0,"ERROR(%d): Symbol '%s' is not defined.");
}
Error* Errors::opDoesntAcceptArrays(const Token* op){
    return new Error(0,"ERROR(%d): The operation '%s' does not work with arrays.");
}
Error* Errors::opOnlyAcceptsArrays(const Token* op){
    return new Error(0,"ERROR(%d): The operation '%s' only works with arrays.");
}
Error* Errors::unaryTypeMismatch(const Token* op, Type found, Type required){
    return new Error(0,"ERROR(%d): Unary '%s' requires an operand of type %s but was given %s.");
}
Error* Errors::missingMainFunction(){
    return new Error(-1,"Procedure main is not defined.");
}

