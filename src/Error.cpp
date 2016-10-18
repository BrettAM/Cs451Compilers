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

#define rtnErr(line, transform) \
    do { \
        std::ostringstream tmp; \
        tmp << transform; \
        return new Error(line, tmp.str()); \
    } while (false)

Error* Errors::cannotBeCalled(const Token* t){
    rtnErr(t->line,
        "\'"<<t->text<<"\' is a simple variable and cannot be called."
    );
}
Error* Errors::incorrectLHS(const Token* t, Type found, std::string required){
    rtnErr(t->line,
        "\'"<<t->text<<"\' requires operands of "<<required
            <<" but lhs is of " << found << "."
    );
}
Error* Errors::incorrectRHS(const Token* t, Type found, std::string required){
    rtnErr(t->line,
        "\'"<<t->text<<"\' requires operands of "<<required
            <<" but rhs is of " << found << "."
    );
}
Error* Errors::mismatchedLR(const Token* t, Type lhs, Type rhs){
    rtnErr(t->line,
        "\'"<<t->text<<"\' requires operands of the same type but lhs is "
            <<lhs<<" and rhs is "<<rhs<<"."
    );
}
Error* Errors::badArrayIndex(const Token* t, Type received){
    rtnErr(t->line,
        "Array \'"<<t->text<<"\' should be indexed by type int but got type "
            <<received<<"."
    );
}
Error* Errors::arrayIndexedByArray(const Token* t){
    rtnErr(t->line,
        "Array index is the unindexed array \'"<<t->text<<"\'."
    );
}
Error* Errors::cannotIndexNonarray(const Token* t){
    rtnErr(t->line,
        "Cannot index nonarray \'"<<t->text<<"\'."
    );
}
Error* Errors::cannotIndexNonarray(int lineno){
    rtnErr(lineno, "Cannot index nonarray.");
}
Error* Errors::cannotReturnArray(const Token* t){
    rtnErr(t->line, "Cannot return an array.");
}
Error* Errors::functionUsedAsVar(const Token* t){
    rtnErr(t->line,
        "Cannot use function \'"<<t->text<<"\' as a variable."
    );
}
Error* Errors::alreadyDefined(const Token* t, int line){
    rtnErr(t->line,
        "Symbol \'"<<t->text<<"\' is already defined at line "<<line<<"."
    );
}
Error* Errors::notDefined(const Token* t){
    rtnErr(t->line,
        "Symbol \'"<<t->text<<"\' is not defined."
    );
}
Error* Errors::opDoesntAcceptArrays(const Token* op){
    rtnErr(op->line,
        "The operation \'"<<op->text<<"\' does not work with arrays."
    );
}
Error* Errors::opOnlyAcceptsArrays(const Token* op){
    rtnErr(op->line,
        "The operation \'"<<op->text<<"\' only works with arrays."
    );
}
Error* Errors::unaryTypeMismatch(const Token* op, Type found, Type required){
    rtnErr(op->line,
        "Unary \'"<<op->text<<"\' requires an operand of type "<<required
            << " but was given " << found;
    );
}
Error* Errors::missingMainFunction(){
    return new Error(Error::LINKER,"Procedure main is not defined.");
}
