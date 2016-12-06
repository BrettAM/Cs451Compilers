#include "UnitTest++/UnitTest++.h"
#include "Location.hpp"
#include "Instruction.hpp"
#include <string>
#include <sstream>
#include <exception>
using namespace std;

TEST(printNOP){
    auto ins = Instruction::nop();
    ins->setLocation(0);
    CHECK_EQUAL("0:   NOP            \n", ins->toString());
    delete ins;
}

TEST(printNOPcomment){
    auto ins = Instruction::nop("Comment");
    ins->setLocation(10);
    CHECK_EQUAL("10:  NOP            Comment\n", ins->toString());
    delete ins;
}

TEST(printRR){
    auto ins = Instruction::alu(Instruction::Add, 1, 2, 3);
    ins->setLocation(0);
    CHECK_EQUAL("0:   ADD  1,2,3     \n", ins->toString());
    delete ins;
}

TEST(printRRcomment){
    auto ins = Instruction::alu(Instruction::Div, 4, 5, 6, "Comment");
    ins->setLocation(10);
    CHECK_EQUAL("10:  DIV  4,5,6     Comment\n", ins->toString());
    delete ins;
}

TEST(printRM){
    Location mem(MemoryRef::Data(15,7));
    auto ins = Instruction::load(3, mem);
    ins->setLocation(0);
    CHECK_EQUAL("0:   LD   3,15(7)   \n", ins->toString());
    delete ins;
}

TEST(printRMcomment){
    Location mem(MemoryRef::Data(-30,6));
    auto ins = Instruction::store(3, mem, "Comment");
    ins->setLocation(10);
    CHECK_EQUAL("10:  ST   3,-30(6)  Comment\n", ins->toString());
    delete ins;
}
