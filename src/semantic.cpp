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

                    resultType = checkCall(e, func, errors);
                } break;
                /**
                 * Check that an operation is performed on subtrees of valid
                 *   types and find its resulting type.
                 */
                case OPERATION:{
                    resultType = checkOperands(e, errors);
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
Type Semantics::checkCall(Node* call, Node* f, vector<Error*>& errors){
    Element* function = dynamic_cast<Element *>(f);

    if(function->nodeType != FUNCTIONDECL){
        errors.push_back(Errors::cannotBeCalled(f->token));
    }

    return function->type.returnType();
}
Type Semantics::checkOperands(Node* opNode, vector<Error*>& errors){
    Element* e = dynamic_cast<Element *>(opNode);
    Element* lhNode = dynamic_cast<Element *>(e->getChild(0));
    Element* rhNode = dynamic_cast<Element *>(e->getChild(1));
    Type lhs = (lhNode != NULL)? lhNode->type.runtime() : Type::NONE;
    Type rhs = (rhNode != NULL)? rhNode->type.runtime() : Type::NONE;
    int op = opNode->token->token;

    // we don't emit multiple errors, so if lhs is wrong just ignore this op
    //    if(lhs == Type::NONE) return ChkResult(Type::NONE);
    // also if its not unary and the other side is none, don't issue errors
    //   but do return the correct type for further checking

/*
    '[' -> indexing
    NOTEQ, LESSEQ, EQ, GRTEQ, '<', '>' -> comparison
    MULASS, ADDASS, SUBASS, DIVASS, '+', '-', '*', '/', '%' -> intops
    AND, OR -> boolean
    '=' -> assignment
    INC, DEC, NOT, '*', '?' -> unary
    RETURN
*/
    //"ERROR(%d): Cannot index nonarray '%s'.\n"
    //"ERROR(%d): Cannot index nonarray.\n"
    //"ERROR(%d): Array index is the unindexed array '%s'.\n"
    //"ERROR(%d): Array '%s' should be indexed by type int but got %s.\n"
    if(op == '[') {
        if(!lhs.isArray()) {
            errors.push_back((lhNode->nodeType == VALUE)
                ? Errors::cannotIndexNonarray(lhNode->token)
                : Errors::cannotIndexNonarray(lhNode->token->line)
                );
        }

        if(rhs.isArray()){
            errors.push_back(Errors::arrayIndexedByArray(rhNode->token));
        }

        if(rhs != Type::INT){
            errors.push_back(Errors::badArrayIndex(lhNode->token, rhs));
        }

        return lhs.returnType();
    }

    //"ERROR(%d): Cannot return an array.\n"
    if(op == RETURN){
        if(lhs.isArray()){
            errors.push_back(Errors::cannotReturnArray(lhNode->token));
        }

        return Type::VOID;
    }


/*
    //"ERROR(%d): Unary '%s' requires an operand of type %s but was given %s.\n"
    //"ERROR(%d): The operation '%s' does not work with arrays.\n"
    //"ERROR(%d): The operation '%s' only works with arrays.\n"
    typedef struct UnarySpecs{
        int op;
        bool arrays;
        Type requiredLhs;
        Type returnType;
    } UnarySpecs;
    UnarySpecs unaries[] = {
        {INC, false, Type::INT,  Type::INT},
        {DEC, false, Type::INT,  Type::INT},
        {NOT, false, Type::BOOL, Type::BOOL},
        {'*',  true, Type::NONE, Type::INT},
        {'?',  true, Type::NONE, Type::INT},
    };
    for(size_t i=0; i<sizeof(unaries)/sizeof(unaries[0]); i++){
        UnarySpecs spec = unaries[i];
        if(op != spec.op) continue;

        //ignore lhs undefined

        if(spec.arrays && !lhs.isArray()){
            return ChkResult(Errors::opOnlyAcceptsArrays(e->token));
        }
        if(!spec.arrays && lhs.isArray()){
            return ChkResult(Errors::opDoesntAcceptArrays(e->token));
        }
        if(spec.requiredLhs != Type::NONE && spec.requiredLhs != lhs){
            return ChkResult(Errors::unaryTypeMismatch(
                e->token, lhs, spec.requiredLhs)
            );
        }
        return ChkResult(spec.returnType);
    }

    // not work with arrays
    // only works with arrays
    // requires lhs x
    // requires rhs x
    // unary requires lhs x
    // return type


    // comparisons
    typedef struct ComparisonSpec{
        int op;
        bool onlyIntegralTypes;
    } ComparisonSpec;
    ComparisonSpec comparisons[] = {
        {NOTEQ, false},
        {   EQ, false},
        {LESSEQ, true},
        { GRTEQ, true},
        {   '>', true},
        {   '<', true},
    };
    for(size_t i=0; i<sizeof(comparisons)/sizeof(comparisons[0]); i++){
        ComparisonSpec spec = comparisons[i];
        if(op != spec.op) continue;

        //ignore lhs undefined
        // these should also set the type but ChkResult makes them none?
        if(spec.onlyIntegralTypes && (lhs.isArray() || rhs.isArray())){
            return ChkResult(Errors::opDoesntAcceptArrays(e->token));
        }
        if(spec.onlyIntegralTypes && !(lhs == Type::INT || lhs == Type::CHAR)){
            return ChkResult(Errors::incorrectLHS(e->token, lhs, "type char or type int"));
        }
        if(spec.onlyIntegralTypes && !(rhs == Type::INT || rhs == Type::CHAR)){
            return ChkResult(Errors::incorrectRHS(e->token, rhs, "type char or type int"));
        }
        if(lhs != rhs){
            return ChkResult(Errors::mismatchedLR(e->token, lhs, rhs));
        }
        return ChkResult(Type::BOOL);
    }

    // binary ops
    typedef struct BinarySpec{
        int op;
        Type type;
    } BinarySpec;
    BinarySpec binaries[] = {
        {MULASS, Type::INT},
        {ADDASS, Type::INT},
        {SUBASS, Type::INT},
        {DIVASS, Type::INT},
        {'+', Type::INT},
        {'-', Type::INT},
        {'*', Type::INT},
        {'/', Type::INT},
        {'%', Type::INT},
        {AND, Type::BOOL},
        {OR, Type::BOOL},
    };
    for(size_t i=0; i<sizeof(binaries)/sizeof(binaries[0]); i++){
        BinarySpec spec = binaries[i];
        if(op != spec.op) continue;

        // these should also set the type but ChkResult makes them none?
        if(lhs.isArray() || rhs.isArray()){
            return ChkResult(Errors::opDoesntAcceptArrays(e->token));
        }
        if(lhs != spec.type){
            return ChkResult(Errors::incorrectLHS(e->token, lhs, spec.type.toString()));
        }
        if(rhs != spec.type){
            return ChkResult(Errors::incorrectRHS(e->token, rhs, spec.type.toString()));
        }
        if(rhs != rhs){
            return ChkResult(Errors::mismatchedLR(e->token, lhs, rhs));
        }
        return ChkResult(spec.type);
    }

    //assignment
    if(lhs.isArray() || rhs.isArray()){
        return ChkResult(Errors::opDoesntAcceptArrays(e->token));
    }
    if(lhs != rhs){
        return ChkResult(Errors::mismatchedLR(e->token, lhs, rhs));
    }

    return ChkResult(lhs);*/

/*
    '[' -> indexing
    NOTEQ, LESSEQ, EQ, GRTEQ, '<', '>' -> comparison
    MULASS, ADDASS, SUBASS, DIVASS, '+', '-', '*', '/', '%' -> intops
    AND, OR -> boolean
    '=' -> assignment
    INC, DEC, NOT, '*', '?' -> unary
    RETURN
*/


    typedef struct ResultType{
        int op;
        Type result;
    } ResultType;
    ResultType results[] = {
        {NOTEQ, Type::BOOL},
        {LESSEQ, Type::BOOL},
        {EQ, Type::BOOL},
        {GRTEQ, Type::BOOL},
        {'<', Type::BOOL},
        {'>', Type::BOOL},
        {AND, Type::BOOL},
        {OR, Type::BOOL},
        {NOT, Type::BOOL},
        {'=', Type::NONE}
    };
    for(size_t i=0; i<sizeof(results)/sizeof(results[0]); i++){
        ResultType spec = results[i];
        if(op != spec.op) continue;

        return (spec.result == Type::NONE)
                ? lhs
                : spec.result;
    }

    // all other operations return INT
    return Type::INT;
}
