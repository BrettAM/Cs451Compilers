#include "UnitTest++/UnitTest++.h"
#include "Token.hpp"
#include "ParseDriver.hpp"
#include "ParseDefines.hpp"
#include "semantic.hpp"
#include "AST.hpp"
#include "Type.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
using namespace std;
using namespace AST;

string compileAndCheck(std::string s){
    ostringstream oss;
    oss <<
            "int i;\n"
            "int ia[3];\n"
            "char c;\n"
            "char ca[3];\n"
            "bool b;\n"
            "bool ba[3];\n"
            "v(){}\n"
            "main(){\n"
        << s << "\n}";
    auto code = oss.str();
    auto result = ParseDriver::run(code.c_str());
    auto errors = Semantics::analyze(result.getAST());
    ostringstream errstring;
    errstring << "\n";
    for(int i=0; i<errors.size(); i++){
        errstring << errors[i] << "\n";
        delete errors[i];
    }
    result.cleanup();
    return errstring.str();
}
TEST(andingInts){
    CHECK_EQUAL( "\n"
        "ERROR(9): 'and' requires operands of type bool but lhs is of type int.\n"
        "ERROR(9): 'and' requires operands of type bool but rhs is of type int.\n",
        compileAndCheck("i and i;")
        );
}
TEST(multiplyInts){
    CHECK_EQUAL( "\n",
        compileAndCheck("i*i;")
        );
}
TEST(intNotEqualIntArray){
    CHECK_EQUAL( "\n"
        "ERROR(9): ‘!=‘ requires that either both or neither operands be arrays.\n"
        ,compileAndCheck("i!=ia;")
        );
}
TEST(intLessThanEqualIntArray){
    CHECK_EQUAL( "\n"
        "ERROR(9): The operation '<=' does not work with arrays.\n"
        ,compileAndCheck("i<=ia;")
        );
}
TEST(intEqualToIntArray){
    CHECK_EQUAL( "\n"
        "ERROR(9): ‘==‘ requires that either both or neither operands be arrays.\n"
        ,compileAndCheck("i==ia;")
        );
}
TEST(intMultAssignIntArray){
    CHECK_EQUAL( "\n"
        "ERROR(9): The operation '*=' does not work with arrays.\n"
        ,compileAndCheck("i*=ia;")
        );
}
TEST(intAssignIntArray){
    CHECK_EQUAL( "\n"
        "ERROR(9): ‘=‘ requires that either both or neither operands be arrays.\n"
        ,compileAndCheck("i=ia;")
        );
}
TEST(intNotEqualChar){
    CHECK_EQUAL( "\n"
        "ERROR(9): '!=' requires operands of the same type but lhs is type int and rhs is type char.\n"
        ,compileAndCheck("i!=c;")
        );
}
TEST(intLessEqChar){
    CHECK_EQUAL( "\n"
        "ERROR(9): '<=' requires operands of the same type but lhs is type int and rhs is type char.\n"
        ,compileAndCheck("i<=c;")
        );
}
TEST(intLessEqCharArray){
    CHECK_EQUAL( "\n"
        "ERROR(9): '<=' requires operands of the same type but lhs is type int and rhs is type char.\n"
        "ERROR(9): The operation '<=' does not work with arrays.\n"
        ,compileAndCheck("i<=ca;")
        );
}
TEST(intTimesEqualsChar){
    CHECK_EQUAL( "\n"
        "ERROR(9): '*=' requires operands of type int but rhs is of type char.\n"
        ,compileAndCheck("i*=c;")
        );
}
TEST(intLessEqualBool){
    CHECK_EQUAL( "\n"
        "ERROR(9): '<=' requires operands of type char or type int but rhs is of type bool.\n"
        ,compileAndCheck("i<=b;")
        );
}
TEST(intNotEqualVoid){
    CHECK_EQUAL( "\n"
        "ERROR(9): '!=' requires operands of NONVOID but rhs is of type void.\n"
        ,compileAndCheck("i!=v();")
        );
}
TEST(notInt){
    CHECK_EQUAL( "\n"
        "ERROR(9): Unary 'not' requires an operand of type bool but was given type int.\n"
        ,compileAndCheck("not i;")
        );
}
TEST(notIntArray){
    CHECK_EQUAL( "\n"
        "ERROR(9): Unary 'not' requires an operand of type bool but was given type int.\n"
        "ERROR(9): The operation 'not' does not work with arrays.\n"
        ,compileAndCheck("not ia;")
        );
}
TEST(undefinedThings){
    CHECK_EQUAL( "\n"
        "ERROR(9): Function 'dog' is not defined.\n"
        "ERROR(9): Symbol 'dog' is not defined.\n"
        ,compileAndCheck("dog(); i = dog;")
        );
}
TEST(constInitializer){
    CHECK_EQUAL( "\n"
        ,compileAndCheck("int f: 37*4+12/2;")
        );
}
TEST(initializerReferncesVariable){
    CHECK_EQUAL( "\n"
        "ERROR(9): Initializer for variable 's' is not a constant expression.\n"
        ,compileAndCheck("int s: i;")
        );
}
TEST(initializerMakesCall){
    CHECK_EQUAL( "\n"
        "ERROR(9): Initializer for variable 'f' is not a constant expression.\n"
        "ERROR(9): Variable 'f' is of type int but is being initialized with an expression of type void.\n"
        ,compileAndCheck("int f: main();")
        );
}
/*
TEST(equalsBothSidesMustBeArrays){
    CHECK_EQUAL( "\n"

        ,compileAndCheck("")
        );
}
*/
