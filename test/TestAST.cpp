#include "UnitTest++/UnitTest++.h"
#include "Token.hpp"
#include "AST.hpp"
#include "ParseDefines.hpp"
#include <string>
#include <sstream>

using namespace AST;

TEST(PrintVarDecl){
    auto a = IdToken(ID,3,"a");
    CHECK_EQUAL("Var a of type bool [line: 3]",
                Decl(VAR,&a,Type::BOOL).toString()
                );
    auto shaun = IdToken(ID,6,"shaun");
    CHECK_EQUAL("Var shaun is array of type int [line: 6]",
                Decl(VAR,&shaun,Type::INT.mkArray(0)).toString()
                );
}

TEST(PrintFuncDecl){
    auto wallace = IdToken(ID,8,"wallace");
    CHECK_EQUAL("Func wallace returns type void [line: 8]",
                Decl(FUNC,&wallace,Type::VOID).toString()
                );
    auto penguin = IdToken(ID,55,"penguin");
    CHECK_EQUAL("Func penguin returns type bool [line: 55]",
                Decl(FUNC,&penguin,Type::BOOL).toString()
                );
}

TEST(PrintParamDecl){
    auto feathers = IdToken(ID,55,"Feathers");
    CHECK_EQUAL("Param Feathers of type char [line: 55]",
                Decl(PARAM,&feathers,Type::CHAR).toString()
                );
    auto yarn = IdToken(ID,75,"yarn");
    CHECK_EQUAL("Param yarn is array of type int [line: 75]",
                Decl(PARAM,&yarn,Type::INT.mkArray(0)).toString()
                );
}

TEST(TerminalValues){
    auto id = IdToken(ID, 13, "z");
    CHECK_EQUAL("Id: z [line: 13]",
                Value(&id).toString());
    auto chr = CharConst(CHARCONST, 113, "'t'");
    CHECK_EQUAL("Const: 't' [line: 113]",
                Value(&chr).toString());
    auto num = NumConst(NUMCONST, 15, "42");
    CHECK_EQUAL("Const: 42 [line: 15]",
                Value(&num).toString());
    auto bol = BoolConst(BOOLCONST, 89, "true");
    CHECK_EQUAL("Const: true [line: 89]",
                Value(&bol).toString());
}
