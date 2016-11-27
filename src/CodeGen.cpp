#include "CodeGen.hpp"

using namespace std;
using namespace AST;

namespace{
    Location badLocation(Location::NONE, 1, 0);
}

int CodeGen::calculateLocations(AST::Node* tree){
    class MemoryAllocator : public Node::Traverser {
    public:
        SymbolTable table;
        int globalPtr;
        int localPtr;
        Element* containingFunc;

        MemoryAllocator(): globalPtr(0), containingFunc(NULL) {}

        Location allocate(Type data){
            int* memorySpace;
            Location::RefClass ref;
            if(data.isStatic()){
                memorySpace = &globalPtr;
                ref = Location::STATIC;
            } else if (containingFunc == NULL){
                memorySpace = &globalPtr;
                ref = Location::GLOBAL;
            } else {
                memorySpace = &localPtr;
                ref = Location::LOCAL;
            }

            int memStart = *memorySpace;
            *memorySpace -= data.size();

            return Location(ref, data.size(), memStart-data.offset());
        }

        Location allocateParameter(Type data){
            int memStart = localPtr;
            localPtr--;
            return Location(Location::PARAMETER, 1, memStart);
        }

        void pre(Node * n){
            Element* e = dynamic_cast<Element *>(n);
            if(e == NULL) return; // not an element node

            switch(e->nodeType){
                case COMPOUND: {
                    table.enter(n);
                } break;
                case DECLARATION:{
                    bool goodDecl = table.add(e->token->text, e);
                    e->location = (goodDecl)? allocate(e->type) : badLocation;
                } break;
                case PARAMETER:{
                    bool goodDecl = table.add(e->token->text, e);
                    e->location = (goodDecl)
                        ? allocateParameter(e->type)
                        : Location(Location::PARAMETER, 1, 0);
                } break;
                case FUNCTIONDECL:{
                    table.add(e->token->text, e);
                    table.enter(n);
                    containingFunc = e;
                    localPtr = -2;
                } break;
                case CALL:{
                    Node* def = table.lookup(e->token->text);
                    e->location = (def != NULL)
                        ? Location(Location::NONE, def->location.getSize(), 0)
                        : badLocation;
                } break;
                case VALUE:{
                    if(e->token->token != ID) break;
                    Node* def = table.lookup(e->token->text);
                    e->location = (def != NULL)? def->location : badLocation;
                } break;
                default: break;
            }
        }
        void post(Node * n){
            Element* e = dynamic_cast<Element *>(n);
            if(e == NULL) return; // not an element node

            switch(e->nodeType){
                case COMPOUND: {
                    table.exit();
                } break;
                case FUNCTIONDECL:{
                    e->location = Location(Location::GLOBAL, localPtr, 0);
                    table.exit();
                    containingFunc = NULL;
                } break;
                default: break;
            }
        }
    } allocator;
    tree->traverse(allocator);
    return allocator.globalPtr;
}
