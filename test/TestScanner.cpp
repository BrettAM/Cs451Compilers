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

/*TEST(NoTokens){
    vector<Token*> result = ParseDriver::run("");
    CHECK(result.size() == 0);
}
*/
TEST(SingleReservedToken){
    vector<Token*> r = ParseDriver::run("while");
    CHECK(r.size() == 1);
    CHECK(*r.at(0) == Token(WHILE,1,"WHILE"));
    freeAll(r);
}

TEST(SingleInvalidToken){
    vector<Token*> r = ParseDriver::run("@");
    CHECK(r.size() == 1);
    CHECK(*r.at(0) == Invalid(MISPLACED,1,"@"));
    freeAll(r);
}

TEST(SingleBoolToken){
    vector<Token*> r = ParseDriver::run("true");
    CHECK(r.size() == 1);
    CHECK(*r.at(0) == BoolConst(BOOLCONST,1,"true"));
    freeAll(r);
}

TEST(SingleCharToken){
    vector<Token*> r = ParseDriver::run("\'a\'");
    CHECK(r.size() == 1);
    CHECK(*r.at(0) == CharConst(CHARCONST,1,"\'a\'"));
    freeAll(r);
}

TEST(SingleIdToken){
    vector<Token*> r = ParseDriver::run("dogs");
    CHECK(r.size() == 1);
    CHECK(*r.at(0) == IdToken(ID,1,"dogs"));
    freeAll(r);
}

TEST(SingleNumberToken){
    vector<Token*> r = ParseDriver::run("123");
    CHECK(r.size() == 1);
    CHECK(*r.at(0) == NumConst(NUMCONST,1,"123"));
    freeAll(r);
}
