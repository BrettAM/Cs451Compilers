#include "Instruction.hpp"

using namespace std;

namespace{
    cstr aluOps[] = {
        [Instruction::Add] = "ADD",
        [Instruction::Sub] = "SUB",
        [Instruction::Mul] = "MUL",
        [Instruction::Div] = "DIV",
        [Instruction::And] = "AND",
        [Instruction::Or] = "OR ",
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
void Instruction::emit(std::ostream& output) const {
    int line = address.lookup().offset;
    output << line << ":\t";
    output << operation << "\t";
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
    output << "\t" << comment;
    output << endl;
}
std::string Instruction::toString() const {
    ostringstream oss;
    emit(oss);
    return oss.str();
}
Instruction Instruction::halt(cstr cmt){
    return Instruction("HALT", cmt);
}
Instruction Instruction::nop(cstr cmt){
    return Instruction("NOP", cmt);
}
Instruction Instruction::ASM(cstr data, cstr cmt){
    return Instruction(data, cmt);
}
Instruction Instruction::alu(Op operation, int r1, int r2, int r3, cstr cmt){
    return Instruction(aluOps[operation], cmt, r1, r2, r3);
}
Instruction Instruction::move(int r, int source, cstr cmt){
    return Instruction("LDA", cmt, r, MemoryRef::Data(0, source));
}
Instruction Instruction::loadConst(int r, int cnst, cstr cmt){
    return Instruction("LDC", cmt, r, MemoryRef::Data(cnst));
}
Instruction Instruction::addConst(int r, int source ,int cnst, cstr cmt){
    return Instruction("LDA", cmt, r, MemoryRef::Data(cnst, source));
}
Instruction Instruction::load(int r, Location l, cstr cmt){
    return Instruction("LD ", cmt, r, l);
}
Instruction Instruction::store(int r, Location l, cstr cmt){
    return Instruction("ST ", cmt, r, l);
}
Instruction Instruction::jmp(Location l, cstr cmt){
    return Instruction("LDC", cmt, PC, l);
}
Instruction Instruction::relJmp(Location l, cstr cmt){
    return Instruction("LDA", cmt, PC, l);
}
Instruction Instruction::jmpNotZero(int testReg, Location l, cstr cmt){
    return Instruction("JNZ", cmt, testReg, l);
}
Instruction Instruction::jmpZero(int testReg, Location l, cstr cmt){
    return Instruction("JZR", cmt, testReg, l);
}
