#include "UnitTest++/UnitTest++.h"
#include "Token.hpp"
#include "ParseDriver.hpp"
#include "parser.hpp"
#include <vector>
using namespace std;

void freeAll(vector<Token*>& r){
    for(vector<Token*>::iterator itr = r.begin(); itr!= r.end(); ++itr){
        delete (*itr);
    }
}

void printAll(vector<Token*>& r){
    cout << "+TOKEN LIST" << endl;
    for(vector<Token*>::iterator itr = r.begin(); itr != r.end(); ++itr){
        cout << (*itr)->toString() << endl;
    }
    cout << "-TOKEN LIST" << endl;
}

TEST(SingleReservedToken){
    vector<Token*> r = ParseDriver::run("while");
    CHECK_EQUAL(1, r.size());
    CHECK_EQUAL(Token(WHILE,1,"WHILE"), *r.at(0));
    freeAll(r);
}

TEST(SingleInvalidToken){
    vector<Token*> r = ParseDriver::run("@");
    CHECK_EQUAL(1, r.size());
    CHECK_EQUAL(Invalid(MISPLACED,1,"@"), *r.at(0));
    freeAll(r);
}

TEST(BoolConsts){
    vector<Token*> r = ParseDriver::run("true\nfalse");
    CHECK_EQUAL(2, r.size());
    CHECK_EQUAL(BoolConst(BOOLCONST,1,"true"), *r.at(0));
    CHECK_EQUAL(BoolConst(BOOLCONST,2,"false"), *r.at(1));
    freeAll(r);
}

TEST(CharTokens){
    vector<Token*> r = ParseDriver::run("\'a\'\'\\0\'\'\\p\'");
    CHECK_EQUAL(3, r.size());
    CHECK_EQUAL(CharConst(CHARCONST,1,"\'a\'"),*r.at(0));
    CHECK_EQUAL(CharConst(CHARCONST,1,"\'\\0\'"),*r.at(1));
    CHECK_EQUAL(CharConst(CHARCONST,1,"\'\\p\'"),*r.at(2));
    freeAll(r);
}

TEST(SingleIdToken){
    vector<Token*> r = ParseDriver::run("dogs");
    CHECK_EQUAL(1, r.size());
    CHECK_EQUAL(IdToken(ID,1,"dogs"), *r.at(0));
    freeAll(r);
}

TEST(SingleNumberToken){
    vector<Token*> r = ParseDriver::run("123");
    CHECK_EQUAL(1, r.size());
    CHECK_EQUAL(NumConst(NUMCONST,1,"123"), *r.at(0));
    freeAll(r);
}

TEST(IgnoreWhitespace){
    vector<Token*> r = ParseDriver::run("123 \t  456   \n\n  789");
    CHECK_EQUAL(3, r.size());
    CHECK_EQUAL(NumConst(NUMCONST,1,"123"), *r.at(0));
    CHECK_EQUAL(NumConst(NUMCONST,1,"456"), *r.at(1));
    CHECK_EQUAL(NumConst(NUMCONST,3,"789"), *r.at(2));
    freeAll(r);
}

TEST(AssignExpression){
    vector<Token*> r = ParseDriver::run("int x = 52*3 > 4;");
    CHECK_EQUAL(9, r.size());
    CHECK_EQUAL(Token(INT,1,"INT"), *r.at(0));
    CHECK_EQUAL(IdToken(ID,1,"x"), *r.at(1));
    CHECK_EQUAL(Token('=',1,"="), *r.at(2));
    CHECK_EQUAL(NumConst(NUMCONST,1,"52"), *r.at(3));
    CHECK_EQUAL(Token('*',1,"*"), *r.at(4));
    CHECK_EQUAL(NumConst(NUMCONST,1,"3"), *r.at(5));
    CHECK_EQUAL(Token('>',1,">"), *r.at(6));
    CHECK_EQUAL(NumConst(NUMCONST,1,"4"), *r.at(7));
    CHECK_EQUAL(Token(';',1,";"), *r.at(8));
    freeAll(r);
}

