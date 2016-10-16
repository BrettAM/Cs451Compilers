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
    return new Error(0,"ERROR(%d): '%s' is a simple variable and cannot be called.\n");
}
Error* Errors::incorrectLHS(const Token* t, Type found, Type required){
    return new Error(0,"ERROR(%d): '%s' requires operands of %s but lhs is of %s.\n");
}
Error* Errors::incorrectRHS(const Token* t, Type found, Type required){
    return new Error(0,"ERROR(%d): '%s' requires operands of %s but rhs is of %s.\n");
}
Error* Errors::mismatchedLR(const Token* t, Type lhs, Type rhs){
    return new Error(0,"ERROR(%d): '%s' requires operands of the same type but lhs is %s and rhs is %s.\n");
}
Error* Errors::badArrayIndex(const Token* t, Type received){
    return new Error(0,"ERROR(%d): Array '%s' should be indexed by type int but got %s.\n");
}
Error* Errors::arrayIndexedByArray(const Token* t){
    return new Error(0,"ERROR(%d): Array index is the unindexed array '%s'.\n");
}
Error* Errors::cannotIndexNonarray(const Token* t){
    return new Error(0,"ERROR(%d): Cannot index nonarray '%s'.\n");
}
Error* Errors::cannotIndexNonarray(int lineno){
    return new Error(0,"ERROR(%d): Cannot index nonarray.\n");
}
Error* Errors::cannotReturnArray(const Token* t){
    return new Error(0,"ERROR(%d): Cannot return an array.\n");
}
Error* Errors::functionUsedAsVar(const Token* func){
    return new Error(0,"ERROR(%d): Cannot use function '%s' as a variable.\n");
}
Error* Errors::alreadyDefined(const Token* t, int previousLine){
    return new Error(0,"ERROR(%d): Symbol '%s' is already defined at line %d.\n");
}
Error* Errors::notDefined(const Token* t){
    return new Error(0,"ERROR(%d): Symbol '%s' is not defined.\n");
}
Error* Errors::opDoesntAcceptArrays(const Token* op){
    return new Error(0,"ERROR(%d): The operation '%s' does not work with arrays.\n");
}
Error* Errors::opOnlyAcceptsArrays(const Token* op){
    return new Error(0,"ERROR(%d): The operation '%s' only works with arrays.\n");
}
Error* Errors::unaryTypeMismatch(const Token* op, Type found, Type required){
    return new Error(0,"ERROR(%d): Unary '%s' requires an operand of type %s but was given %s.\n");
}
