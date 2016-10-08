#include "UnitTest++/UnitTest++.h"
#include "Token.hpp"
#include "ParseDriver.hpp"
#include "ParseDefines.hpp"
#include <vector>
using namespace std;

void printAll(const vector<const Token*>& r){
    cout << "+TOKEN LIST" << endl;
    for(auto itr = r.begin(); itr != r.end(); ++itr){
        cout << (*itr)->toString() << endl;
    }
    cout << "-TOKEN LIST" << endl;
}

/*
Because there is no way to seperate flex and bison, all the test data must
be syntactically valid c- code or else bison will stop the parsing process
with an error
*/

TEST(intExpression){
    auto result = ParseDriver::run("int x: 52*3;");
    auto r = result.getTokens();

    Token expected[] = {
        Token(INT,1,"int"),
        IdToken(ID,1,"x"),
        Token(':',1,":"),
        NumConst(NUMCONST,1,"52"),
        Token('*',1,"*"),
        NumConst(NUMCONST,1,"3"),
        Token(';',1,";"),
    };

    CHECK_EQUAL(sizeof(expected) / sizeof(expected[0]), r->size());
    for(int i=0; i<r->size(); i++){
        CHECK_EQUAL(expected[i], *r->at(i));
    }

    result.cleanup();
}

TEST(charExpression){
    auto result = ParseDriver::run("char abc123: '\\0';");
    auto r = result.getTokens();

    Token expected[] = {
        Token(CHAR,1,"char"),
        IdToken(ID,1,"abc123"),
        Token(':',1,":"),
        CharConst(CHARCONST,1,"'\\0'"),
        Token(';',1,";"),
    };

    CHECK_EQUAL(sizeof(expected) / sizeof(expected[0]), r->size());
    for(int i=0; i<r->size(); i++){
        CHECK_EQUAL(expected[i], *r->at(i));
    }

    result.cleanup();
}

TEST(boolExpression){
    auto result = ParseDriver::run("bool B1: true or false;");
    auto r = result.getTokens();

    Token expected[] = {
        Token(BOOL,1,"bool"),
        IdToken(ID,1,"B1"),
        Token(':',1,":"),
        BoolConst(BOOLCONST,1,"true"),
        Token(OR,1,"or"),
        BoolConst(BOOLCONST,1,"false"),
        Token(';',1,";"),
    };

    CHECK_EQUAL(sizeof(expected) / sizeof(expected[0]), r->size());
    for(int i=0; i<r->size(); i++){
        CHECK_EQUAL(expected[i], *r->at(i));
    }

    result.cleanup();
}

TEST(ignoreWhitespace){
    auto result = ParseDriver::run("bool \n\nB1:\ttrue    or \nfalse\n;\n");
    auto r = result.getTokens();

    Token expected[] = {
        Token(BOOL,1,"bool"),
        IdToken(ID,3,"B1"),
        Token(':',3,":"),
        BoolConst(BOOLCONST,3,"true"),
        Token(OR,3,"or"),
        BoolConst(BOOLCONST,4,"false"),
        Token(';',5,";"),
    };

    CHECK_EQUAL(sizeof(expected) / sizeof(expected[0]), r->size());
    for(int i=0; i<r->size(); i++){
        CHECK_EQUAL(expected[i], *r->at(i));
    }

    result.cleanup();
}
