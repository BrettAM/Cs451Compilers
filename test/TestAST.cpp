#include "UnitTest++/UnitTest++.h"
#include "Token.hpp"
#include "AST.hpp"
#include "ParseDefines.hpp"
#include <string>
#include <sstream>

using namespace AST;

TEST(PrintVarDecl){
    auto a = IdToken(ID,3,"a");
    auto adecl = VarDecl(&a, Type::BOOL);
    CHECK_EQUAL("Var a of type bool [line: 3]",
                adecl->toString()
                );
    delete adecl;
    auto shaun = IdToken(ID,6,"shaun");
    auto sdecl = VarDecl(&shaun, Type::INT.mkArray(0));
    CHECK_EQUAL("Var shaun is array of type int [line: 6]",
                sdecl->toString()
                );
    delete sdecl;
}

TEST(PrintFuncDecl){
    auto wallace = IdToken(ID,8,"wallace");
    auto wdecl = FuncDecl(&wallace,Type::VOID, NULL, NULL);
    CHECK_EQUAL("Func wallace returns type void [line: 8]",
                wdecl->toString()
                );
    delete wdecl;
    auto penguin = IdToken(ID,55,"penguin");
    auto pdecl = FuncDecl(&penguin,Type::BOOL, NULL, NULL);
    CHECK_EQUAL("Func penguin returns type bool [line: 55]",
                pdecl->toString()
                );
    delete pdecl;
}

TEST(PrintParamDecl){
    auto feathers = IdToken(ID,55,"Feathers");
    auto fdecl = Parameter(&feathers,Type::CHAR);
    CHECK_EQUAL("Param Feathers of type char [line: 55]",
                fdecl->toString()
                );
    delete fdecl;
    auto yarn = IdToken(ID,75,"yarn");
    auto ydecl = Parameter(&yarn,Type::INT.mkArray(0));
    CHECK_EQUAL("Param yarn is array of type int [line: 75]",
                ydecl->toString()
                );
    delete ydecl;
}

TEST(PrintIdNode){
    auto id = IdToken(ID, 13, "z");
    auto node = IdNode(&id);
    CHECK_EQUAL("Id: z [line: 13]",
                node->toString());
    delete node;
}

TEST(PrintConstNode){
    auto chr = CharConst(CHARCONST, 113, "'t'");
    auto num = NumConst(NUMCONST, 15, "42");
    auto bol = BoolConst(BOOLCONST, 89, "true");
    auto chrNode = ConstNode(&chr);
    auto numNode = ConstNode(&num);
    auto bolNode = ConstNode(&bol);
    CHECK_EQUAL("Const: 't' [line: 113]",
                chrNode->toString());
    CHECK_EQUAL("Const: 42 [line: 15]",
                numNode->toString());
    CHECK_EQUAL("Const: true [line: 89]",
                bolNode->toString());
    delete chrNode;
    delete numNode;
    delete bolNode;
}

TEST(PrintRecordNode){
    auto token = IdToken(ID, 115, "Point");
    auto node = RecordNode(&token, NULL);
    CHECK_EQUAL("Record Point  [line: 115]",
                node->toString());
    delete node;
}

TEST(PrintCallNode){
    auto token = IdToken(ID, 42, "wallace");
    auto node = CallNode(&token, NULL);
    CHECK_EQUAL("Call: wallace [line: 42]",
                node->toString());
    delete node;
}

TEST(PrintOpNode){
    auto token = Token('-',42,"-");
    auto node = OpNode(&token, NULL, NULL);
    CHECK_EQUAL("Op: - [line: 42]",
                node->toString());
    delete node;
}

TEST(PrintAssignNode){
    auto token = Token('=',46,"=");
    auto node = AssignNode(&token, NULL, NULL);
    CHECK_EQUAL("Assign: = [line: 46]",
                node->toString());
    delete node;
}

TEST(PrintCompoundNode){
    auto token = Token('{',43,"{");
    auto node = Compound(&token, NULL, NULL);
    CHECK_EQUAL("Compound [line: 43]",
                node->toString());
    delete node;
}

TEST(PrintIfNode){
    auto token = Token(IF,47,"if");
    auto node = IfNode(&token, NULL, NULL, NULL);
    CHECK_EQUAL("If [line: 47]",
                node->toString());
    delete node;
}

TEST(PrintWhileNode){
    auto token = Token(WHILE,95,"while");
    auto node = WhileNode(&token, NULL, NULL);
    CHECK_EQUAL("While [line: 95]",
                node->toString());
    delete node;
}

TEST(PrintReturnNode){
    auto token = Token(RETURN,95,"return");
    auto node = ReturnNode(&token, NULL);
    CHECK_EQUAL("Return [line: 95]",
                node->toString());
    delete node;
}

TEST(PrintFunction){
    auto x = IdToken(ID, 1, "x");
    auto y = IdToken(ID, 1, "y");
    auto z = IdToken(ID, 1, "z");
    auto gr = Token('>', 1, ">");
    auto eq = Token('=', 1, "=");
    auto op = Token('{', 1, "{");
    auto two = NumConst(NUMCONST, 1, "2");
    auto ift = IdToken(IF, 1, "if");

    auto node =
        IfNode(&ift,
            OpNode(
                &gr,
                IdNode(&x),
                IdNode(&y)),
            AssignNode(
                &eq,
                IdNode(&z),
                IdNode(&x)),
            Compound(
                &op,
                VarDecl(&z, Type::INT),
                AssignNode(
                    &eq,
                    IdNode(&z),
                    ConstNode(&two))
                )
            );

    CHECK_EQUAL(
        "If [line: 1]\n"
        "Child: 0  Op: > [line: 1]\n"
        "!   Child: 0  Id: x [line: 1]\n"
        "!   Child: 1  Id: y [line: 1]\n"
        "Child: 1  Assign: = [line: 1]\n"
        "!   Child: 0  Id: z [line: 1]\n"
        "!   Child: 1  Id: x [line: 1]\n"
        "Child: 2  Compound [line: 1]\n"
        "!   Child: 0  Var z of type int [line: 1]\n"
        "!   Child: 1  Assign: = [line: 1]\n"
        "!   !   Child: 0  Id: z [line: 1]\n"
        "!   !   Child: 1  Const: 2 [line: 1]\n",
        node->formatTree()
    );

    node->deleteTree();
}


TEST(SiblingPrintFunction){
    auto x = IdToken(ID, 1, "x");
    auto y = IdToken(ID, 1, "y");
    auto z = IdToken(ID, 1, "z");
    auto gr = Token('>', 1, ">");
    auto eq = Token('=', 1, "=");
    auto op = Token('{', 1, "{");
    auto two = NumConst(NUMCONST, 1, "2");
    auto ift = IdToken(IF, 1, "if");

    auto node =
        Compound(
            &op,
            VarDecl(&z, Type::INT),
            Siblings(listof<Node*>()
                << AssignNode(
                    &eq,
                    IdNode(&z),
                    ConstNode(&two))
                << OpNode(
                    &gr,
                    IdNode(&x),
                    IdNode(&y))
                << AssignNode(
                    &eq,
                    IdNode(&z),
                    IdNode(&x))
                )
            );

    CHECK_EQUAL(
        "Compound [line: 1]\n"
        "Child: 0  Var z of type int [line: 1]\n"
        "Child: 1  Assign: = [line: 1]\n"
        "!   Child: 0  Id: z [line: 1]\n"
        "!   Child: 1  Const: 2 [line: 1]\n"
        "Sibling: 0  Op: > [line: 1]\n"
        "!   Child: 0  Id: x [line: 1]\n"
        "!   Child: 1  Id: y [line: 1]\n"
        "Sibling: 1  Assign: = [line: 1]\n"
        "!   Child: 0  Id: z [line: 1]\n"
        "!   Child: 1  Id: x [line: 1]\n",
        node->formatTree()
    );

    node->deleteTree();
}

TEST(VarDeclEquality){
    Node* A = VarDecl(new IdToken(ID,1,"x"), Type::INT);
    Node* B = VarDecl(new IdToken(ID,1,"x"), Type::INT);
    CHECK_EQUAL(*A,*B);
    A->deleteTree();
    B->deleteTree();
}

TEST(LeafEquality){
    Node* A = Leaf();
    Node* B = Leaf();
    CHECK_EQUAL(*A,*B);
    A->deleteTree();
    B->deleteTree();
}

TEST(NodeEquality){
    Node* A = Siblings(
        listof<Node*>() <<
            VarDecl(new IdToken(ID,1,"x"), Type::INT,
                OpNode(new Token('*',1,"*"),
                    ConstNode(new NumConst(NUMCONST,1,"52")),
                    ConstNode(new NumConst(NUMCONST,1,"3"))
                )
            )
    );
    Node* B = Siblings(
        listof<Node*>() <<
            VarDecl(new IdToken(ID,1,"x"), Type::INT,
                OpNode(new Token('*',1,"*"),
                    ConstNode(new NumConst(NUMCONST,1,"52")),
                    ConstNode(new NumConst(NUMCONST,1,"3"))
                )
            )
    );

    CHECK_EQUAL(*A,*B);

    A->deleteTree();
    B->deleteTree();
}

TEST(VarDeclInequality){
    Node* A = VarDecl(new IdToken(ID,1,"x"), Type::INT);
    Node* B = VarDecl(new IdToken(ID,2,"x"), Type::INT);
    Node* C = VarDecl(new IdToken(ID,1,"x"), Type::BOOL);
    CHECK(! (*A==*B));
    CHECK(! (*B==*C));
    CHECK(! (*A==*C));
    A->deleteTree();
    B->deleteTree();
    C->deleteTree();
}
