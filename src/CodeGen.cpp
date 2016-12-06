#include "CodeGen.hpp"

using namespace std;
using namespace AST;

typedef Instruction Inst;
typedef MemoryRef Mem;

namespace{
    const MemoryRef OLD_FRAME_LOC      = Mem::Data(0, LOCALFRM);
    const MemoryRef RETURN_ADDRESS_LOC = Mem::Data(-1, LOCALFRM);

    class GeneratedCode {
    public:
        GeneratedCode(): instructionIndex(0) {}
        vector<Instruction*> code;
        int instructionIndex;
        Instruction* emit(Instruction* istr){
            if(!istr->isComment()){
                istr->setLocation(instructionIndex);
                instructionIndex++;
            }
            code.push_back(istr);
            return istr;
        }
        GeneratedCode& operator<<(Instruction* istr){
            emit(istr);
            return *this;
        }
        void dump(ostream& output){
            for(size_t i=0; i<code.size(); i++){
                code[i]->emit(output);
                delete code[i];
            }
        }
        void initGlobal(Element* glob);
        void mkFunction(Element* func);
        void mkReturn(int valueRegister);
    };
}

void CodeGen::generate(Node* tree, ostream& output){
    GeneratedCode code;

    // sort out global defs and function defs, log main's location
    vector<Element*> globals, functions;
    int globalSize = 0;
    Location mainFunc;
    for(int i=0; tree->getChild(i) != NULL; i++){
        Element* e = dynamic_cast<Element *>(tree->getChild(i));
        if(e == NULL) continue;
        switch(e->nodeType){
            case FUNCTIONDECL:
                functions.push_back(e);
                if(e->token->text == "main") mainFunc.bind(&(e->location));
                break;
            case DECLARATION:
                globals.push_back(e);
                e->location.bind(
                    Mem::Data(e->type.offset()-globalSize, GLOBALFRM)
                    );
                globalSize += e->type.size();
                break;
            default:
                break;
        }
    }

    // standard header
    code << Inst::loadConst(ZEROREG, 0, "Zero Register")
      << Inst::load(GLOBALFRM, Mem::Data(0), "global pointer")
      << Inst::addConst(LOCALFRM, GLOBALFRM, -globalSize, "Frame after globs")
      << Inst::store(LOCALFRM, Mem::Data(0,LOCALFRM), "old fp at self")
      << Inst::comment("Init Globals");

    // global setters
    for(size_t i=0; i<globals.size(); i++){
        code.initGlobal(globals[i]);
    }

    // jump to main
    code << Inst::addConst(RETURNVAL, PC, 1, "store return addr")
         << Inst::jmp(mainFunc, "Jump to main")
         << Inst::halt("DONE");

    // Generate code for the functions
    for(size_t i=0; i<functions.size(); i++){
        code.mkFunction(functions[i]);
    }

    // Dump generated code to output
    code.dump(output);
}

void GeneratedCode::initGlobal(Element* glob){
}
void GeneratedCode::mkFunction(Element* func){
    emit(Inst::comment("Start of ", func->token->text.c_str()));
    Instruction* start =
        emit(Inst::store(RETURNVAL, RETURN_ADDRESS_LOC, "store rtn addr"));
    func->location.bind(start->getLocation());

    mkReturn(ZEROREG);
}
void GeneratedCode::mkReturn(int valueRegister){
    *this << Inst::move(RETURNVAL, valueRegister, "Load return value")
      << Inst::load(ACC1, RETURN_ADDRESS_LOC, "load rtn addr")
      << Inst::load(LOCALFRM, OLD_FRAME_LOC, "pop frame")
      << Inst::move(PC, ACC1, "jump");
}
