#include "semantic.hpp"

using namespace std;
using namespace AST;
using namespace Semantics;

std::vector<Error*> Semantics::analyze(AST::Node* root){
    class Analyzer : public Node::Traverser {
    public:
        vector<Error*> errors;
        SymbolTable table;
        bool prevEnterWasFunc;
        void pre(Node * n){
            Element* e = dynamic_cast<Element *>(n);
            if(e == NULL) return; // not an element node

            switch(e->nodeType){
                /**
                 * Enter a new scope unless immediatly following a function call
                 */
                case COMPOUND: {
                    if(prevEnterWasFunc){
                        prevEnterWasFunc = false;
                        break;
                    } else {
                        table.enter(n);
                    }
                } break;
                /**
                 * Enter declarations into the symbol table, logging errors for
                 *   duplicate declarations as we go.
                 * If its a function declaration, Make a new scope and flag that
                 *   a compound immediatly following should not enter a new
                 *   scope.
                 */
                case DECLARATION:
                case FUNCTIONDECL:{
                    string id = e->token->text;
                    bool success = table.add(id, e);
                    if(!success) {
                        int line = table.lookup(id)->token->line;
                        errors.push_back(Errors::alreadyDefined(e->token,line));
                    }

                    if(e->nodeType == FUNCTIONDECL){
                        prevEnterWasFunc = true;
                        table.enter(n);
                    }
                } break;
                default:
                    break;
            }
        }
        void post(Node * n){
            Element* e = dynamic_cast<Element *>(n);
            if(e == NULL) return; // not an element node

            // if we entered a block here, leave it
            if(table.getBlock() == n){
                prevEnterWasFunc = false;
                table.exit();
            }

            Type resultType = Type::NONE;
            switch(e->nodeType){
                /**
                 * Check the symbol table for a value's current type.
                 * Constants are already tagged with a type so ignore them.
                 */
                case VALUE:{
                    if(e->type != Type::NONE) break;
                    Node* def = table.lookup(e->token->text);
                    if(def == NULL){
                        errors.push_back(Errors::notDefined(e->token));
                    } else if(def->type.isFunction()) {
                        errors.push_back(Errors::functionUsedAsVar(e->token));
                    } else {
                        resultType = def->type;
                    }
                } break;
                /**
                 * Check that a function is defined and that the call is valid,
                 *   find the call's return type.
                 */
                case CALL:{
                    Node* func = table.lookup(e->token->text);
                    if(func == NULL){
                        errors.push_back(Errors::notDefined(e->token));
                        break;
                    }

                    ChkResult res = checkCall(e, func);
                    if(!res.passed){
                        errors.push_back(res.error);
                        break;
                    }
                    resultType = res.result;
                } break;
                /**
                 * Check that an operation is performed on subtrees of valid
                 *   types and find its resulting type.
                 */
                case OPERATION:{
                    ChkResult res = checkOperands(e);
                    if(!res.passed){
                        errors.push_back(res.error);
                        break;
                    }
                    resultType = res.result;
                } break;
                default:
                break;
            }
            if(resultType != Type::NONE){
                e->type = resultType;
            }
        }
    } analyzer;
    root->traverse(analyzer);

    if(analyzer.table.lookup("main")==NULL){
        analyzer.errors.push_back(Errors::missingMainFunction());
    }

    return analyzer.errors;
}
ChkResult Semantics::checkCall(AST::Node* call, AST::Node* f){
    Element* function = dynamic_cast<Element *>(f);

    if(function->nodeType != FUNCTIONDECL){
        return ChkResult(Errors::cannotBeCalled(f->token));
    }

    return ChkResult(function->type.returnType());
}
ChkResult Semantics::checkOperands(AST::Node* opNode){
    Element* e = dynamic_cast<Element *>(opNode);
    Element* lhNode = dynamic_cast<Element *>(e->getChild(0));
    Element* rhNode = dynamic_cast<Element *>(e->getChild(1));
    Type lhs = (lhNode != NULL)? lhNode->type.runtime() : Type::NONE;
    Type rhs = (rhNode != NULL)? rhNode->type.runtime() : Type::NONE;
    int op = opNode->token->token;
/*
    cout << "Operation " << opNode->token->text << " Called on "
         << lhs << " and " << rhs << endl;
*/
    // we don't emit multiple errors, so if lhs is wrong just ignore this op
    //if(lhs == Type::NONE) return ChkResult(Type::NONE);
    // also if its not unary and the other side is none, don't issue errors
    // but do return the correct type for further checking

/*
    '[' -> indexing
    NOTEQ, LESSEQ, EQ, GRTEQ, '<', '>' -> comparison
    MULASS, ADDASS, SUBASS, DIVASS, '=' -> assignment
    '+', '-', '*', '/', '%' -> intops
    INC, DEC, NOT, '*', '?' -> unary
    AND, OR -> boolean
    RETURN
*/

    //"ERROR(%d): Cannot index nonarray '%s'.\n"
    //"ERROR(%d): Cannot index nonarray.\n"
    //"ERROR(%d): Array index is the unindexed array '%s'.\n"
    //"ERROR(%d): Array '%s' should be indexed by type int but got %s.\n"
    if(op == '[') {
        if(!lhs.isArray()) {
            return ChkResult((lhNode->nodeType == VALUE)
                ? Errors::cannotIndexNonarray(lhNode->token)
                : Errors::cannotIndexNonarray(lhNode->token->line)
                );
        }

        if(rhs.isArray()){
            return ChkResult(Errors::arrayIndexedByArray(rhNode->token));
        }

        if(rhs != Type::INT){
            return ChkResult(Errors::badArrayIndex(lhNode->token, rhs));
        }

        return ChkResult(lhs.returnType());
    }

    //"ERROR(%d): Cannot return an array.\n"
    if(op == RETURN){
        if(lhs.isArray()){
            return ChkResult(Errors::cannotReturnArray(lhNode->token));
        }

        return ChkResult(Type::VOID);
    }

    //"ERROR(%d): The operation '%s' does not work with arrays.\n"
    //"ERROR(%d): The operation '%s' only works with arrays.\n"
    //"ERROR(%d): Unary '%s' requires an operand of type %s but was given %s.\n"
    //"ERROR(%d): '%s' requires operands of %s but lhs is of %s.\n"
    //"ERROR(%d): '%s' requires operands of %s but rhs is of %s.\n"
    //"ERROR(%d): '%s' requires operands of the same type but lhs is %s and rhs is %s.\n"

    return ChkResult(Type::NONE);
}
