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
                try{
                    code[i]->emit(output);
                } catch (NotYetBoundException& e){
                    cerr << "Attempt to emit an unbound location on line "
                      << code[i]->getLineNumber()
                      << endl;
                    output << "#UNBOUND LOCATION#" << endl;
                }
            }

            for(size_t i=0; i<code.size(); i++){
                delete code[i];
            }
        }
        int nextInstructionIndex() { return instructionIndex; }
        void initGlobal(Element* glob);
        void mkFunction(SymbolTable& table, Element* func);
        void mkReturn(int valueRegister);
    };

    void emitASM(GeneratedCode& code, std::string literal){
        // strip off the ` mark from both ends
        string text = literal.substr(1, literal.size()-2);
        // Emit each block seperated by a ';' as an assembly instruction
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

    class StatementListTranslator : public Node::Traverser<Element> {
    public:
        vector<Element*> breakStack;
        GeneratedCode& code;
        SymbolTable& table;
        int freeStackSpace;
        bool allocateDecls;
        StatementListTranslator(GeneratedCode& code, SymbolTable& table, bool allocateDecls=true):
            code(code), table(table), freeStackSpace(-2), allocateDecls(allocateDecls) {}
        MemoryRef allocate(Type var);
        void computeOperation(int op, int reg, Element* e);
        void loadConst(Element* e);
        void pre(Element * e);
        void inorder(Element * e, int index);
        void post(Element * e);
        bool assignTypeOp(int op);
    };
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
                // set the location of the global and keep track of its size
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
      << Inst::jmp(&(table.lookup("main")->codeStart), "Jump to main")
      << Inst::halt("DONE");

    // Generate code for the functions
    for(size_t i=0; i<functions.size(); i++){
        code.mkFunction(table, functions[i]);
    }

    // Dump generated code to output
    code.dump(output);
}

void GeneratedCode::initGlobal(Element* glob){
    SymbolTable emptyTable;
    StatementListTranslator ag(*this, emptyTable, false);
    glob->traverse(ag);
}
void GeneratedCode::mkFunction(SymbolTable& table, Element* func){
    table.enter(func);
    emit(Inst::comment("************************************"));
    emit(Inst::comment("Start of ", func->token->text.c_str()));
    Instruction* start =
        emit(Inst::store(RETURNVAL, RETURN_ADDRESS_LOC, "store rtn addr"));
    func->codeStart.bind(start->getLocation());

    // This traverser will visit the AST nodes in this function and
    // translate them to assembly instructions
    StatementListTranslator ag(*this, table);
    func->traverse(ag);

    mkReturn(ZEROREG);
    table.exit();
}
void GeneratedCode::mkReturn(int valueRegister){
    *this << Inst::move(RETURNVAL, valueRegister, "set return value")
      << Inst::load(ACC1, RETURN_ADDRESS_LOC, "load rtn addr")
      << Inst::load(LOCALFRM, OLD_FRAME_LOC, "pop frame")
      << Inst::move(PC, ACC1, "jump");
}

MemoryRef StatementListTranslator::allocate(Type var){
    MemoryRef space = Mem::Data(freeStackSpace-var.offset(), LOCALFRM);
    freeStackSpace -= var.size();
    return space;
}
bool StatementListTranslator::assignTypeOp(int op){
    switch(op){
        case '=':
        case MULASS:
        case ADDASS:
        case SUBASS:
        case DIVASS:
        case INC:
        case DEC:
            return true;
        default:
            return false;
    }
}
/**
 * leave the value resulting from `op` in `reg`
 */
void StatementListTranslator::computeOperation(int op, int result, Element* e){
    code << Inst::load(ACC1, &(e->getChild(0)->location), "Load operand");
    if(dynamic_cast<LeafNode*>(e->getChild(1)) == NULL){
        code << Inst::load(ACC2, &(e->getChild(1)->location), "Load operand");

        // Binary operators
        switch(op){
            case NOTEQ: case NOT: {
                code << Inst::alu(Inst::Not, result, ACC1, ACC2);
            } break;
            case MULASS: case '*': {
                code << Inst::alu(Inst::Mul, result, ACC1, ACC2);
            } break;
            case ADDASS: case '+': {
                code << Inst::alu(Inst::Add, result, ACC1, ACC2);
            } break;
            case SUBASS: case '-': {
                code << Inst::alu(Inst::Sub, result, ACC1, ACC2);
            } break;
            case DIVASS: case '/': {
                code << Inst::alu(Inst::Div, result, ACC1, ACC2);
            } break;
            case AND: {
                code << Inst::alu(Inst::And, result, ACC1, ACC2);
            } break;
            case OR: {
                code << Inst::alu(Inst::Or, result, ACC1, ACC2);
            } break;
            case EQ: {
                code << Inst::alu(Inst::Equal, result, ACC1, ACC2);
            } break;
            case GRTEQ: {
                code << Inst::alu(Inst::GreaterEqual, result, ACC1, ACC2);
            } break;
            case LESSEQ: {
                code << Inst::alu(Inst::LessEqual, result, ACC1, ACC2);
            } break;
            case '<': {
                code << Inst::alu(Inst::LessThan, result, ACC1, ACC2);
            } break;
            case '>': {
                code << Inst::alu(Inst::Greater, result, ACC1, ACC2);
            } break;
            case '=': {
                code << Inst::move(result, ACC2, "Assignment");
            } break;
            case '%': {
                code << Inst::alu(Inst::Div, ACC3, ACC1, ACC2, "Mod op start")
                  << Inst::alu(Inst::Mul, ACC3, ACC2, ACC3, "Mod continued")
                  << Inst::alu(Inst::Sub, result, ACC1, ACC3, "Mod finish");
            } break;
            default:
                cerr << "Unhandled binary op " << e->token->text << endl;
        }
    } else {
        // Unary operators
        switch(op){
            case INC: {
                code << Inst::addConst(result, ACC1, 1);
            } break;
            case DEC: {
                code << Inst::addConst(result, ACC1, -1);
            } break;
            case '-': {
                code << Inst::alu(Inst::Sub, result, ZEROREG, ACC1);
            } break;
            case '?': {
                code << Inst::alu(Inst::Random, result, ACC1, ZEROREG);
            } break;
            default:
                cerr << "Unhandled unary op " << e->token->text << endl;
        }
    }
}
void StatementListTranslator::loadConst(Element* e){
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
void StatementListTranslator::pre(Element * e){
    // bind the code start address for the node
    if(e->nodeType != FUNCTIONDECL) {
        e->codeStart.bind(MemoryRef::Program(code.nextInstructionIndex()));
    }

    if(e->token->token == WHILE) {
        breakStack.push_back(e);
    }
}
void StatementListTranslator::inorder(Element * e, int index){
    if(e->token->token == IF){
        switch(index){
            case 0: { // after test
                Location* passoverLocation =
                    (dynamic_cast<LeafNode*>(e->getChild(2)) != NULL)
                        ? &(e->codeEnd)// no ELSE
                        : &(e->getChild(2)->codeStart); // ELSE exists
                code << Inst::load(ACC1, &(e->getChild(0)->location), "Load condition")
                  << Inst::jmpZero(ACC1, passoverLocation, "Jump over block");
            } break;
            case 1: { // after true statement
                code << Inst::jmp(&(e->codeEnd), "Jump to end of IF");
            } break;
            case 2: { // after else
            } break;
        }
    }
    if(e->token->token == WHILE){
        switch(index){
            case 0: { // after test
                code << Inst::load(ACC1, &(e->getChild(0)->location), "Load condition")
                  << Inst::jmpZero(ACC1, &(e->codeEnd), "Jump over block");
            } break;
            case 1: { // after block
                code << Inst::jmp(&(e->codeStart), "Jump to end of WHILE");
            } break;
        }
    }
}
void StatementListTranslator::post(Element * e){
    switch(e->nodeType){
        /**
         *
         */
        // handle array references '['
        case OPERATION: {
            int op = e->token->token;
            if(assignTypeOp(op)){
                e->location.bind( &(e->getChild(0)->location) );
            } else {
                e->location.bind( allocate(e->type) );
            }

            computeOperation(op, ACC1, e);

            code << Inst::store(ACC1, e->location, "Store computed value");
        } break;
        /**
         *
         */
        case DECLARATION: {
            // if the init argument exists
            if(dynamic_cast<Element*>(e->getChild(0)) != NULL){
                code << Inst::load(ACC1, &(e->getChild(0)->location), "load initial value")
                  << Inst::store(ACC1, &(e->location), "store initial value");
            }
            // init array size
        } /* FALL THROUGH */
        case PARAMETER: {
            table.add(e->token->text, e);
            if(this->allocateDecls){
                e->location.bind(allocate(e->type));
            }
            // if declaration, copy initialization value in
        } break;
        /**
         *
         */
        case VALUE: {
            if(e->token->token == ID) {
                Node * variableDec = table.lookup(e->token->text);
                Location* varLocation = &(variableDec->location);
                e->location.bind(varLocation);
            } else {
                loadConst(e);
            }
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
            Location* funcAddr = &(table.lookup(e->token->text)->codeStart);
            MemoryRef returnTemp = allocate(e->type);
            e->location.bind(returnTemp);
            // setup a phost frame in ACC3
            code << Inst::comment("Jump to ", e->token->text)
              << Inst::addConst(ACC3, LOCALFRM, freeStackSpace, "Make ghost frame");
            // load parameters (present if e->getChild(0) is a sibling list)
            Node* parameterList = e->getChild(0);
            if(dynamic_cast<SiblingList*>(parameterList) != NULL){
                for(int i=0; parameterList->getChild(i) != NULL; i++){
                    Node * param = parameterList->getChild(i);
                    code << Inst::load(ACC1, &(param->location), "load param")
                      << Inst::store(ACC1, Mem::Data(-(i+2), ACC3), "set param");
                }
            }
            // Execute call and store return value
            code << Inst::store(LOCALFRM, Mem::Data(0, ACC3), "Store local frame")
              << Inst::move(LOCALFRM, ACC3, "Swap to ghost frame")
              << Inst::addConst(RETURNVAL, PC, 1, "store return addr")
              << Inst::jmp(funcAddr, "Jump")
              << Inst::store(RETURNVAL, returnTemp, "Store return value");
        } break;
        /**
         *
         */
        case AST::BREAK: {
            code << Inst::jmp(&(breakStack.back()->codeEnd), "break");
        } break;
        /**
         *
         */
        case CONTROL: {
            if(e->token->token == WHILE){
                breakStack.pop_back();
            }
        } break;
        /**
         *
         */
        case RETURNSTMT:{
            Node * rtval = e->getChild(0);
            int returnReg = ZEROREG;
            if(dynamic_cast<Element*>(rtval) != NULL){
                code << Inst::load(ACC1, rtval->location, "Load return value");
                returnReg = ACC1;
            }
            code.mkReturn(returnReg);
        } break;
        default: break;
    }

    // bind the code end address for the node
    e->codeEnd.bind(MemoryRef::Program(code.nextInstructionIndex()));
}
