#include "semantic.hpp"

using namespace std;
using namespace AST;
using namespace Semantics;

std::vector<Error*> Semantics::analyze(AST::Node* root){
    class Analyzer : public Node::Traverser {
    public:
        vector<Error*> errors;
        SymbolTable table;
        bool lastEnteredFunction;
        void pre(Node * n){
            Element* e = dynamic_cast<Element *>(n);
            if(e == NULL) return; // not an element node

            //enter scopes on compound, function
            //register declarations, checking for dups

            //always enter on function
            //enter on compound if parent isn't a function

            switch(e->nodeType){
                case COMPOUND: {
                    if(lastEnteredFunction){
                        lastEnteredFunction = false;
                        break;
                    } else {
                        table.enter(n);
                    }
                } break;
                case FUNCTIONDECL: {
                    lastEnteredFunction = true;
                    table.enter(n);
                }
                case DECLARATION:{
                    bool success = table.add(e->token->text, e);
                    if(!success) {
                        int line = table.lookup(e->token->text)->token->line;
                        errors.push_back(Errors::alreadyDefined(e->token,line));
                    }
                } break;
                default:
                    break;
            }
        }
        void post(Node * n){
            if(table.getBlock() == n){
                // if we entered a block here, leave it
                lastEnteredFunction = false;
                table.exit();
            }

            Element* e = dynamic_cast<Element *>(n);
            if(e == NULL) return; // not an element node

            //always exit on function
            //exit on compound if parent isn't a function

            //exit scopes on compound, function
            //variables get checked for definitions
            //functions get checked for operands
            //calls get checked
            //type the node
            switch(e->nodeType){
                case VALUE:{
                    if(e->type == Type::NONE){
                        Node* def = table.lookup(e->token->text);
                        if(def == NULL){
                            errors.push_back(Errors::notDefined(e->token));
                        } else {
                            e->type = def->type;
                        }
                    }
                } break;
                default:
                break;
            }
        }
    } analyzer;
    root->traverse(analyzer);

    if(analyzer.table.lookup("main")==NULL){
        analyzer.errors.push_back(Errors::missingMainFunction());
    }

    return analyzer.errors;
}
ChkResult Semantics::checkCall(AST::Node* call, AST::Node* function){
    return ChkResult(Type::NONE);
}
ChkResult Semantics::checkOperands(AST::Node* op, AST::Node* lhs, AST::Node* rhs){
    return ChkResult(Type::NONE);
}
ChkResult Semantics::checkOperands(AST::Node* op, AST::Node* rhs){
    return ChkResult(Type::NONE);
}
