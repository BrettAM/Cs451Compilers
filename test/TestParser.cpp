#include "UnitTest++/UnitTest++.h"
#include "Token.hpp"
#include "ParseDriver.hpp"
#include "ParseDefines.hpp"
#include "AST.hpp"
#include "Type.hpp"
#include <vector>
using namespace std;

using namespace AST;

TEST(intVariableWithInitialExpression){
    auto result = ParseDriver::run("int x: 52*3;");

    auto id = new IdToken(ID,1,"x");
    auto mul = new Token('*',1,"*");
    auto fiftyTwo = new NumConst(NUMCONST,1,"52");
    auto three = new NumConst(NUMCONST,1,"3");

    Node* expected = Siblings(
        listof<Node*>() <<
            VarDecl(id, Type::INT,
                OpNode(mul,
                    ConstNode(fiftyTwo),
                    ConstNode(three)
                )
            )
    );

    CHECK_EQUAL(*expected, *result.getAST());

    result.cleanup();
    expected->deleteTree();
    delete id;
    delete mul;
    delete fiftyTwo;
    delete three;
}
