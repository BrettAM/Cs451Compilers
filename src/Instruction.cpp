#include "Instruction.hpp"

using namespace std;

namespace{
    cstr aluOps[] = {
        [Instruction::Add] = "ADD",
        [Instruction::Sub] = "SUB",
        [Instruction::Mul] = "MUL",
        [Instruction::Div] = "DIV",
        [Instruction::And] = "AND",
        [Instruction::Or] = "OR",
        [Instruction::Xor] = "XOR",
        [Instruction::Not] = "NOT",
        [Instruction::Swap] = "SWP",
        [Instruction::Random] = "RND",
        [Instruction::LessThan] = "TLT",
        [Instruction::LessEqual] = "TLE",
        [Instruction::Equal] = "TEQ",
        [Instruction::NotEqual] = "TNE",
        [Instruction::GreaterEqual] = "TGE",
        [Instruction::Greater] = "TGT",
    };
}

void Instruction::setLocation(int lineno){
    address.bind(MemoryRef::Program(lineno));
}
const Location* Instruction::getLocation(){
    return &address;
}
void Instruction::emit(std::ostream& output) const {
    if(optype == Comment){
        output << cmt << endl;
        return;
    }

    ostream::pos_type startp = output.tellp();
    int line = address.lookup().offset;

    // output line number and pad to 5 places
    output << line << ":";
    while(output.tellp()-startp < 5) output<<' ';

    // output operation and pad to positon 10
    output << operation;
    while(output.tellp()-startp < 10) output<<' ';

    // output arguments
    switch(optype){
        case RegReg:{
            output << result << "," << op1 << "," << op2;
        } break;
        case RegMem:{
            MemoryRef mem = memory.lookup();
            output << result << "," << mem.offset << "(" << mem.registr << ")";
        } break;
        default: break;
    }

    //pad to position 20 and output the comment
    output << ' ';
    while(output.tellp()-startp < 20) output<<' ';
    output << cmt;

    output << endl;
}
std::string Instruction::toString() const {
    ostringstream oss;
    emit(oss);
    return oss.str();
}
Instruction* Instruction::comment(cstr label, cstr text){
    ostringstream oss;
    oss << "* " << label << text;
    return new Instruction(oss.str());
}
Instruction* Instruction::halt(cstr cmt){
    return new Instruction("HALT", cmt);
}
Instruction* Instruction::nop(cstr cmt){
    return new Instruction("NOP", cmt);
}
Instruction* Instruction::ASM(cstr data, cstr cmt){
    return new Instruction(data, cmt);
}
Instruction* Instruction::alu(Op operation, int r1, int r2, int r3, cstr cmt){
    return new Instruction(aluOps[operation], cmt, r1, r2, r3);
}
Instruction* Instruction::move(int r, int source, cstr cmt){
    return new Instruction("LDA", cmt, r, MemoryRef::Data(0, source));
}
Instruction* Instruction::loadConst(int r, int cnst, cstr cmt){
    return new Instruction("LDC", cmt, r, MemoryRef::Data(cnst));
}
Instruction* Instruction::addConst(int r, int source ,int cnst, cstr cmt){
    return new Instruction("LDA", cmt, r, MemoryRef::Data(cnst, source));
}
Instruction* Instruction::load(int r, Location l, cstr cmt){
    return new Instruction("LD", cmt, r, l);
}
Instruction* Instruction::store(int r, Location l, cstr cmt){
    return new Instruction("ST", cmt, r, l);
}
Instruction* Instruction::jmp(Location l, cstr cmt){
    return new Instruction("LDC", cmt, PC, l);
}
Instruction* Instruction::relJmp(Location l, cstr cmt){
    return new Instruction("LDA", cmt, PC, l);
}
Instruction* Instruction::jmpNotZero(int testReg, Location l, cstr cmt){
    return new Instruction("JNZ", cmt, testReg, l);
}
Instruction* Instruction::jmpZero(int testReg, Location l, cstr cmt){
    return new Instruction("JZR", cmt, testReg, l);
}
