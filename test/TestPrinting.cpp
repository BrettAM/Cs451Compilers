#include "UnitTest++/UnitTest++.h"
#include "Token.hpp"
#include "AST.hpp"
#include "ParseDefines.hpp"
#include "ParseDriver.hpp"
#include "TextUtils.hpp"
#include <string>
#include <sstream>

using namespace AST;

cstr tests[][2] = {
{ // record declaration
    "record Point {\n"
    "bool defined;\n"
    "int x, y;\n"
    "}\n",
    "Record Point  [line: 1]\n"
    "!   Child: 0  Var defined of type bool [line: 2]\n"
    "!   Sibling: 0  Var x of type int [line: 3]\n"
    "!   Sibling: 1  Var y of type int [line: 3]\n"
},

};

TEST(TreePrintingIntegrationTests){
    for(int i=0; i<sizeof(tests)/sizeof(tests[0]); i++){
        auto result = ParseDriver::run(tests[i][0]);
        CHECK_EQUAL(tests[i][1], result.getAST()->formatTree());
        result.cleanup();
    }
}
