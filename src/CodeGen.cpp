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
        void mkFunction(SymbolTable& table, Element* func);
        void mkReturn(int valueRegister);
    };

    void emitASM(GeneratedCode& code, std::string literal){
        // strip off the ` mark from both end
        string text = literal.substr(1, literal.size()-2);
        while(true){
            size_t lineSplitLoc = text.find(';');
            if(lineSplitLoc == std::string::npos) {
                code << Inst::ASM(text);
                break;
            }
            code << Inst::ASM(text.substr(0, lineSplitLoc));
            text = text.substr(lineSplitLoc+1, text.size()-lineSplitLoc);
        }
    }
}

void CodeGen::generate(Node* tree, ostream& output){
    GeneratedCode code;

    // sort out global defs and function defs, log main's location
    vector<Element*> globals, functions;
    int globalSize = 0;
    SymbolTable table;
    for(int i=0; tree->getChild(i) != NULL; i++){
        Element* e = dynamic_cast<Element *>(tree->getChild(i));
        if(e == NULL) continue;
        switch(e->nodeType){
            case FUNCTIONDECL:
                functions.push_back(e);
                table.add(e->token->text, e);
                break;
            case DECLARATION:
                globals.push_back(e);
                table.add(e->token->text, e);
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
      << Inst::jmp(&(table.lookup("main")->location), "Jump to main")
      << Inst::halt("DONE");

    // Generate code for the functions
    for(size_t i=0; i<functions.size(); i++){
        code.mkFunction(table, functions[i]);
    }

    // Dump generated code to output
    code.dump(output);
}

void GeneratedCode::initGlobal(Element* glob){
}
void GeneratedCode::mkFunction(SymbolTable& table, Element* func){
    table.enter(func);
    emit(Inst::comment("************************************"));
    emit(Inst::comment("Start of ", func->token->text.c_str()));
    Instruction* start =
        emit(Inst::store(RETURNVAL, RETURN_ADDRESS_LOC, "store rtn addr"));
    func->location.bind(start->getLocation());

    class AssemblyGenerator : public Node::Traverser {
    public:
        GeneratedCode& code;
        SymbolTable& table;
        int freeStackSpace;
        AssemblyGenerator(GeneratedCode& code, SymbolTable& table):
            code(code), table(table), freeStackSpace(-2) {}
        MemoryRef allocate(Type var){
            MemoryRef space = Mem::Data(freeStackSpace-var.offset(), LOCALFRM);
            freeStackSpace -= var.size();
            return space;
        }
        void loadConst(Element* e){
            e->location.bind( allocate(e->type) );
            int value = 0;
            switch(e->token->token){
                case BOOLCONST: {
                    value = ((const BoolConst*) e->token)->value;
                } break;
                case CHARCONST: {
                    value = ((const CharConst*) e->token)->literal;
                } break;
                case NUMCONST: {
                    value = ((const NumConst*) e->token)->value;
                } break;
                default: break;
            }
            code << Inst::loadConst(ACC1, value, e->token->text);
            code << Inst::store(ACC1, e->location, "Store constant");
        }
        void pre(Node * n){}
        void post(Node * n){
            Element* e = dynamic_cast<Element *>(n);
            if(e == NULL) return; // not an element node
            switch(e->nodeType){
                /**
                 *
                 */
                case DECLARATION:
                case PARAMETER: {
                    table.add(e->token->text, e);
                    e->location.bind(allocate(e->type));
                    // if declaration, copy initialization value in
                } break;
                /**
                 *
                 */
                case VALUE: {
                    if(e->token->token == ID) break;
                    loadConst(e);
                } break;
                /**
                 * emit asm node contents literally
                 */
                case ASM: {
                    emitASM(code, e->token->text);
                } break;
                /**
                 *
                 */
                case CALL: {
                    Location* funcAddr = &(table.lookup(e->token->text)->location);
                    // ACC3 holds the ghost frame
                    code << Inst::comment("Jump to ", e->token->text)
                      << Inst::addConst(ACC3, LOCALFRM, freeStackSpace, "Make ghost frame")
                      // load parameters
                      << Inst::store(LOCALFRM, Mem::Data(0, ACC3), "Store local frame")
                      << Inst::move(LOCALFRM, ACC3, "Swap to ghost frame")
                      << Inst::addConst(RETURNVAL, PC, 1, "store return addr")
                      << Inst::jmp(funcAddr, "Jump");
                } break;
                default: break;
            }
        }
    } ag(*this, table);
    func->traverse(ag);

    mkReturn(ZEROREG);
    table.exit();
}
void GeneratedCode::mkReturn(int valueRegister){
    *this << Inst::move(RETURNVAL, valueRegister, "Load return value")
      << Inst::load(ACC1, RETURN_ADDRESS_LOC, "load rtn addr")
      << Inst::load(LOCALFRM, OLD_FRAME_LOC, "pop frame")
      << Inst::move(PC, ACC1, "jump");
}

