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

bool Errors::compare(const Error* const lhs, const Error* const rhs){
    return (*lhs < *rhs);
}

#define rtnErr(token, transform) \
    do { \
        std::ostringstream tmp; \
        tmp << transform; \
        return new Error(token, tmp.str()); \
    } while (false)

Error* Errors::cannotBeCalled(const Token* t){
    rtnErr(t,
        "\'"<<t->text<<"\' is a simple variable and cannot be called."
    );
}
Error* Errors::incorrectLHS(const Token* t, Type found, std::string required){
    rtnErr(t,
        "\'"<<t->text<<"\' requires operands of "<<required
            <<" but lhs is of type " << found.rawString() << "."
    );
}
Error* Errors::incorrectLHS(const Token* t, Type found, Type required){
    rtnErr(t,
        "\'"<<t->text<<"\' requires operands of type "<<required
            <<" but lhs is of type " << found.rawString() << "."
    );
}
Error* Errors::incorrectRHS(const Token* t, Type found, std::string required){
    rtnErr(t,
        "\'"<<t->text<<"\' requires operands of "<<required
            <<" but rhs is of type " << found.rawString() << "."
    );
}
Error* Errors::incorrectRHS(const Token* t, Type found, Type required){
    rtnErr(t,
        "\'"<<t->text<<"\' requires operands of type "<<required
            <<" but rhs is of type " << found.rawString() << "."
    );
}
Error* Errors::mismatchedLR(const Token* t, Type lhs, Type rhs){
    rtnErr(t,
        "\'"<<t->text<<"\' requires operands of the same type but lhs is type "
            <<lhs.rawString()<<" and rhs is type "<<rhs.rawString()<<"."
    );
}
Error* Errors::badArrayIndex(const Token* t, Type received){
    rtnErr(t,
        "Array \'"<<t->text<<"\' should be indexed by type int but got type "
            <<received.rawString()<<"."
    );
}
Error* Errors::arrayIndexedByArray(const Token* t){
    rtnErr(t,
        "Array index is the unindexed array \'"<<t->text<<"\'."
    );
}
Error* Errors::cannotIndexNonarray(const Token* t){
    if(t->token == ID){
        rtnErr(t, "Cannot index nonarray \'"<<t->text<<"\'.");
    } else {
        rtnErr(t, "Cannot index nonarray.");
    }
}
Error* Errors::cannotReturnArray(const Token* t){
    rtnErr(t, "Cannot return an array.");
}
Error* Errors::functionUsedAsVar(const Token* t){
    rtnErr(t,
        "Cannot use function \'"<<t->text<<"\' as a variable."
    );
}
Error* Errors::alreadyDefined(const Token* t, int line){
    rtnErr(t,
        "Symbol \'"<<t->text<<"\' is already defined at line "<<line<<"."
    );
}
Error* Errors::notDefined(const Token* t){
    rtnErr(t,
        "Symbol \'"<<t->text<<"\' is not defined."
    );
}
Error* Errors::opDoesntAcceptArrays(const Token* op){
    rtnErr(op,
        "The operation \'"<<op->text<<"\' does not work with arrays."
    );
}
Error* Errors::opOnlyAcceptsArrays(const Token* op){
    rtnErr(op,
        "The operation \'"<<op->text<<"\' only works with arrays."
    );
}
Error* Errors::unaryTypeMismatch(const Token* op, Type found, Type required){
    rtnErr(op,
        "Unary \'"<<op->text<<"\' requires an operand of type type "<<required.rawString()
            << " but was given type " << found.rawString() << ".";
    );
}
Error* Errors::missingMainFunction(){
    return new Error(Error::LINKER,0,"Procedure main is not defined.");
}
