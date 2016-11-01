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
        return Error::Err(line, tmp.str()); \
    } while (false)

#define rtnWarn(line, transform) \
    do { \
        std::ostringstream tmp; \
        tmp << transform; \
        return Error::Warn(line, tmp.str()); \
    } while (false)

Error* Errors::cannotBeCalled(const Token* t){
    rtnErr(t->line,
        "\'"<<t->text<<"\' is a simple variable and cannot be called."
    );
}
Error* Errors::incorrectLHS(const Token* t, Type found, std::string required){
    rtnErr(t->line,
        "\'"<<t->text<<"\' requires operands of "<<required
            <<" but lhs is of type " << found.rawString() << "."
    );
}
Error* Errors::incorrectLHS(const Token* t, Type found, Type required){
    rtnErr(t->line,
        "\'"<<t->text<<"\' requires operands of type "<<required
            <<" but lhs is of type " << found.rawString() << "."
    );
}
Error* Errors::incorrectRHS(const Token* t, Type found, std::string required){
    rtnErr(t->line,
        "\'"<<t->text<<"\' requires operands of "<<required
            <<" but rhs is of type " << found.rawString() << "."
    );
}
Error* Errors::incorrectRHS(const Token* t, Type found, Type required){
    rtnErr(t->line,
        "\'"<<t->text<<"\' requires operands of type "<<required
            <<" but rhs is of type " << found.rawString() << "."
    );
}
Error* Errors::mismatchedLR(const Token* t, Type lhs, Type rhs){
    rtnErr(t->line,
        "\'"<<t->text<<"\' requires operands of the same type but lhs is type "
            <<lhs.rawString()<<" and rhs is type "<<rhs.rawString()<<"."
    );
}
Error* Errors::badArrayIndex(const Token* t, Type received){
    rtnErr(t->line,
        "Array \'"<<t->text<<"\' should be indexed by type int but got type "
            <<received.rawString()<<"."
    );
}
Error* Errors::arrayIndexedByArray(const Token* t){
    if(t->token == ID){
        rtnErr(t->line, "Array index is the unindexed array \'"<<t->text<<"\'.");
    } else {
        rtnErr(t->line, "Array index is an unindexed array.");
    }
}
Error* Errors::cannotIndexNonarray(const Token* t){
    if(t->token == ID){
        rtnErr(t->line, "Cannot index nonarray \'"<<t->text<<"\'.");
    } else {
        rtnErr(t->line, "Cannot index nonarray.");
    }
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
        "Unary \'"<<op->text<<"\' requires an operand of type "<<required.rawString()
            << " but was given type " << found.rawString() << ".";
    );
}
Error* Errors::missingMainFunction(){
    return Error::Err(Error::LINKER,"Procedure main is not defined.");
}
Error* Errors::arrayUsedAsTest(const Token* t){
    rtnErr(t->line,
        "Cannot use array as test condition in " << t->text << " statement.";
    );
}
Error* Errors::badTestType(const Token* t, Type found){
    rtnErr(t->line,
        "Expecting Boolean test condition in " << t->text << " statement but got type " << found.rawString() << ".";
    );
}
Error* Errors::mismatchedArrayStatus(const Token* t){
    rtnErr(t->line,
        "'" << t->text << "' requires that either both or neither operands be arrays.";
    );
}
Error* Errors::nonconstInitializer(const Token* t){
    rtnErr(t->line,
        "Initializer for variable '"<< t->text <<"' is not a constant expression.";
    );
}
Error* Errors::badInitializerType(const Token* t, Type found, Type expected){
    rtnErr(t->line,
        "Variable '"<<t->text<<"' is of type "<<expected.rawString()<<
        " but is being initialized with an expression of type "<<
        found.rawString()<< ".";
    );
}
Error* Errors::missingReturnStatement(const Token* t, Type found){
    rtnWarn(t->line,
        "Expecting to return type "<<found.rawString()<<" but function '"<<
        t->text<<"' has no return statement.";
    );
}
Error* Errors::badReturnValue(const Token* t, const Token* function, Type found, Type expected){
    if(expected == Type::VOID){
        rtnErr(t->line,
            "Function '"<<
            function->text<<"' at line "<<
            function->line<<" is expecting no return value, but return has return value.";
        );
    } else if (found == Type::VOID){
        rtnErr(t->line,
            "Function '"<<
            function->text<<"' at line "<<
            function->line<<" is expecting to return type "<<
            expected.rawString()<<" but return has no return value.";
        );
    }
    rtnErr(t->line,
        "Function '"<<
        function->text<<"' at line "<<
        function->line<<" is expecting to return type "<<
        expected.rawString()<<" but instead returns type "<<
        found.rawString()<<".";
    );
}
Error* Errors::breakOutsideLoop(const Token* t){
    rtnErr(t->line,
        "Cannot have a break statement outside of loop.";
    );
}
Error* Errors::tooFewParameters(const Token* call, const Token* function){
    rtnErr(call->line,
        "Too few parameters passed for function '"<<function->text<<
        "' defined on line "<< function->line <<".";
    );
}
Error* Errors::badParameterType(const Token* t, Type found, Type expected, int parmIndex, const Token* function){
    rtnErr(t->line,
        "Expecting "<< expected.rawString()<<
        " in parameter "<< parmIndex <<
        " of call to '"<< function->text <<
        "' defined on line "<< function->line <<" but got "<<
        found.rawString() <<".";
    );
}
Error* Errors::expectedArrayParameter(const Token* t, int parmIndex, const Token* function){
    rtnErr(t->line,
        "Expecting array in parameter "<<parmIndex<<
        " of call to '"<< function->text <<
        "' defined on line "<< function->line <<".";
    );
}
Error* Errors::unexpectedArrayParameter(const Token* t, int parmIndex, const Token* function){
    rtnErr(t->line,
        "Not expecting array in parameter "<<
        parmIndex <<" of call to '"<<
        function->text <<"' defined on line "<<
        function->line <<".";
    );
}
Error* Errors::tooManyParameters(const Token* t, const Token* function){
    rtnErr(t->line,
        "Too many parameters passed for function '"<< function->text <<
        "' defined on line "<< function->line <<".";
    );
}
Error* Errors::undefinedFunction(const Token* t){
    rtnErr(t->line,
        "Function '"<<t->text<<"' is not defined.";
    );
}
