#include "CodeGen.hpp"

using namespace std;
using namespace AST;

typedef Instruction Inst;

namespace{
    class GeneratedCode {
    public:
        vector<Instruction> code;
        void operator<<(Instruction istr){
            istr.setLocation(code.size());
            code.push_back(istr);
        }
        void dump(ostream& output){
            for(size_t i=0; i<code.size(); i++){
                code[i].emit(output);
            }
        }
    };

    void initGlobal(GeneratedCode& code, Element* glob){

    }

    void mkFunction(GeneratedCode& code, Element* func){

    }
}

void CodeGen::generate(Node* tree, ostream& output){
    GeneratedCode code;

    // sort out global defs and function defs, log main's location
    vector<Element*> globals, functions;
    int globalSize = 0;
    Location mainFunc(MemoryRef::Program(0));
    for(int i=0; tree->getChild(i) != NULL; i++){
        Element* e = dynamic_cast<Element *>(tree->getChild(i));
        if(e == NULL) continue;
        switch(e->nodeType){
            case FUNCTIONDECL:
                functions.push_back(e);
                //if(e->token->text == "main") mainFunc.bind(&(e->location));
                break;
            case DECLARATION:
                globals.push_back(e);
                e->location.bind(
                    MemoryRef::Data(e->type.offset()-globalSize, GLOBALFRM)
                    );
                globalSize += e->type.size();
                break;
            default:
                break;
        }
    }

    // standard header
    code << Inst::loadConst(ZEROREG, 0, "Zero Register");
    code << Inst::load(GLOBALFRM, MemoryRef::Data(0), "global pointer");
    code << Inst::addConst(LOCALFRM, GLOBALFRM, -globalSize, "Frame after globs");
    code << Inst::store(LOCALFRM, MemoryRef::Data(0,LOCALFRM), "old fp at self");
    code << Inst::nop("Init Globals");

    // global setters
    for(size_t i=0; i<globals.size(); i++){
        initGlobal(code, globals[i]);
    }

    // jump to main
    code << Inst::addConst(RETURNVAL, PC, 1, "store return addr");
    code << Inst::jmp(mainFunc, "Jump to main");
    code << Inst::halt("DONE");

    // Generate code for the functions
    for(size_t i=0; i<globals.size(); i++){
        mkFunction(code, functions[i]);
    }

    // Dump generated code to output
    code.dump(output);
}
