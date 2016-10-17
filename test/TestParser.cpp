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

    Node* expected = Siblings(
        listof<Node*>() <<
            VarDecl(new IdToken(ID,1,"x"), Type::INT,
                OpNode(new Token('*',1,"*"),
                    ConstNode(new NumConst(NUMCONST,1,"52")),
                    ConstNode(new NumConst(NUMCONST,1,"3"))
                )
            )
    );

    CHECK_EQUAL(*expected, *result.getAST());

    result.cleanup();
    expected->deleteTree();
}
