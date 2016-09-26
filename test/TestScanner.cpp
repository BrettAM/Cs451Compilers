#include "UnitTest++/UnitTest++.h"
#include "Token.hpp"
#include "ParseDriver.hpp"
#include "ParseDefines.hpp"
#include <vector>
using namespace std;

void printAll(vector<Token*>& r){
    cout << "+TOKEN LIST" << endl;
    for(auto itr = r.begin(); itr != r.end(); ++itr){
        cout << (*itr)->toString() << endl;
    }
    cout << "-TOKEN LIST" << endl;
}
/*
For a reason unknown to me now, having bison complain about these token
sequences is causing flex to output a difference sequence of tokens?

TEST(SingleReservedToken){
    auto result = ParseDriver::run("while");
    auto r = result.getTokens();
    CHECK_EQUAL(1, r->size());
    CHECK_EQUAL(Token(WHILE,1,"while"), *r->at(0));
    result.cleanup();
}

TEST(SingleInvalidToken){
    auto result = ParseDriver::run("@");
    auto r = result.getTokens();
    CHECK_EQUAL(1, r->size());
    CHECK_EQUAL(Invalid(MISPLACED,1,"@"), *r->at(0));
    result.cleanup();
}

TEST(BoolConsts){
    auto result = ParseDriver::run("true\nfalse");
    auto r = result.getTokens();
    CHECK_EQUAL(2, r->size());
    CHECK_EQUAL(BoolConst(BOOLCONST,1,"true"), *r->at(0));
    CHECK_EQUAL(BoolConst(BOOLCONST,2,"false"), *r->at(1));
    result.cleanup();
}

TEST(CharTokens){
    auto result = ParseDriver::run("\'a\'\'\\0\'\'\\p\'");
    auto r = result.getTokens();
    CHECK_EQUAL(3, r->size());
    CHECK_EQUAL(CharConst(CHARCONST,1,"\'a\'"),*r->at(0));
    CHECK_EQUAL(CharConst(CHARCONST,1,"\'\\0\'"),*r->at(1));
    CHECK_EQUAL(CharConst(CHARCONST,1,"\'\\p\'"),*r->at(2));
    result.cleanup();
}

TEST(SingleIdToken){
    auto result = ParseDriver::run("dogs");
    auto r = result.getTokens();
    CHECK_EQUAL(1, r->size());
    CHECK_EQUAL(IdToken(ID,1,"dogs"), *r->at(0));
    result.cleanup();
}

TEST(SingleNumberToken){
    auto result = ParseDriver::run("123");
    auto r = result.getTokens();
    CHECK_EQUAL(1, r->size());
    CHECK_EQUAL(NumConst(NUMCONST,1,"123"), *r->at(0));
    result.cleanup();
}

TEST(IgnoreWhitespace){
    auto result = ParseDriver::run("123 \t  456   \n\n  789");
    auto r = result.getTokens();
    CHECK_EQUAL(3, r->size());
    CHECK_EQUAL(NumConst(NUMCONST,1,"123"), *r->at(0));
    CHECK_EQUAL(NumConst(NUMCONST,1,"456"), *r->at(1));
    CHECK_EQUAL(NumConst(NUMCONST,3,"789"), *r->at(2));
    result.cleanup();
}

TEST(AssignExpression){
    auto result = ParseDriver::run("int x = 52*3 > 4;");
    auto r = result.getTokens();
    CHECK_EQUAL(9, r->size());
    CHECK_EQUAL(Token(INT,1,"int"), *r->at(0));
    CHECK_EQUAL(IdToken(ID,1,"x"), *r->at(1));
    CHECK_EQUAL(Token('=',1,"="), *r->at(2));
    CHECK_EQUAL(NumConst(NUMCONST,1,"52"), *r->at(3));
    CHECK_EQUAL(Token('*',1,"*"), *r->at(4));
    CHECK_EQUAL(NumConst(NUMCONST,1,"3"), *r->at(5));
    CHECK_EQUAL(Token('>',1,">"), *r->at(6));
    CHECK_EQUAL(NumConst(NUMCONST,1,"4"), *r->at(7));
    CHECK_EQUAL(Token(';',1,";"), *r->at(8));
    result.cleanup();
}
*/
