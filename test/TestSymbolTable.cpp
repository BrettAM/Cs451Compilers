#include "UnitTest++/UnitTest++.h"
#include <string>
#include "AST.hpp"
#include "SymbolTable.hpp"
using namespace std;
using namespace AST;

namespace{
    Node* one = Leaf();
    Node* two = Leaf();
}

TEST(equality){
    CHECK(one != two);
    CHECK(one == one);
    CHECK(two == two);
}

TEST(addAndAccess){
    SymbolTable t;
    CHECK(t.lookup("one") == NULL);
    CHECK(t.add("one",one) == true);
    CHECK(one == t.lookup("one"));
}

TEST(addDuplicate){
    SymbolTable t;
    t.add("one",one);
    CHECK(t.add("one",one) == false);
}

TEST(shadowing){
    SymbolTable t;
    t.add("one",one);
    t.add("two",two);
    t.enter();
    CHECK(t.lookup("one") == one);
    CHECK(t.add("one",two) == true);
    CHECK(t.lookup("one") == two);
    CHECK(t.lookup("two") == two);
}

TEST(leaveScope){
    SymbolTable t;
    t.add("one",one);
    t.enter();
    t.add("one",two);
    t.exit();
    CHECK(t.lookup("one")==one);
}
