#include "semantic.hpp"

using namespace std;
using namespace AST;
using namespace Semantics;

std::vector<Error*> Semantics::analyze(AST::Node* root){
    class Analyzer : public Node::Traverser {
    public:
        vector<Error*> errors;
        SymbolTable table;
        Element* containingFunc;
        bool prevEnterWasFunc;
        bool returnedFromYet;
        int loopDepth;
        Analyzer(): prevEnterWasFunc(false), loopDepth(0) {}
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
                 * For function declarations, add to the symbol table,
                 *   then make a new scope and flag that
                 *   a compound immediatly following should not enter a new
                 *   scope.
                 */
                case FUNCTIONDECL:{
                    string id = e->token->text;
                    bool success = table.add(id, e);
                    if(!success) {
                        int line = table.lookup(id)->token->line;
                        errors.push_back(Errors::alreadyDefined(e->token,line));
                    }

                    prevEnterWasFunc = true;
                    containingFunc = e;
                    returnedFromYet = false;
                    table.enter(n);
                } break;
                /**
                 * Check for not defined values in the order they appear
                 */
                case VALUE:
                case CALL:{
                    if(e->type != Type::NONE) break;
                    Node* def = table.lookup(e->token->text);
                    if(def == NULL){
                        if(e->nodeType == CALL){
                            errors.push_back(Errors::undefinedFunction(e->token));
                        } else {
                            errors.push_back(Errors::notDefined(e->token));
                        }
                    }
                }break;
                /**
                 * Increment the loop depth count when entering a while loop
                 */
                case CONTROL:{
                    if(e->token->token == WHILE) loopDepth++;
                }
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
                 * check the initializer list, add decls and params to the
                 *   symbol table
                 */
                case DECLARATION:{
                    Node* init = e->getChild(0);
                    if(init != NULL){
                        bool constExpr = checkConstness(init);
                        if(!constExpr){
                            errors.push_back(Errors::nonconstInitializer(e->token));
                        }
                        if(e->type.returnType() != init->type.returnType()){
                            if(init->type != Type::NONE){
                                errors.push_back(
                                    Errors::badInitializerType(
                                        e->token,
                                        init->type,
                                        e->type)
                                );
                            }
                        }
                    }
                } /* FALL THROUGH */
                case PARAMETER:{
                    string id = e->token->text;
                    bool success = table.add(id, e);
                    if(!success) {
                        int line = table.lookup(id)->token->line;
                        errors.push_back(Errors::alreadyDefined(e->token,line));
                    }
                } break;
                /**
                 * Check the symbol table for a value's current type.
                 * Constants are already tagged with a type so ignore them.
                 */
                case VALUE:{
                    if(e->type != Type::NONE) break;
                    Node* def = table.lookup(e->token->text);
                    if(def == NULL){
                        // undefined errors are caught in preorder traversal
                        break;
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
                        // undefined errors are caught in preorder traversal
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
                /**
                 * Check that conditional statements have the right kind of
                 *   conditional checks
                 * Decrement loop depth if leaving a while loop
                 */
                case CONTROL:{
                    Node* cond = e->getChild(0);
                    if(cond->type.returnType() != Type::BOOL){
                        if(cond->type != Type::NONE)
                            errors.push_back(Errors::badTestType(e->token, cond->type));
                    }
                    if(cond->type.isArray()){
                        errors.push_back(Errors::arrayUsedAsTest(e->token));
                    }

                    if(e->token->token == WHILE) loopDepth--;
                } break;
                /**
                 * Check that break statements only occur inside loops
                 */
                case AST::BREAK:{
                    if(loopDepth <= 0){
                        errors.push_back(Errors::breakOutsideLoop(e->token));
                    }
                } break;
                /**
                 * Check return statement types
                 */
                case RETURNSTMT:{
                    Element* lhNode = dynamic_cast<Element *>(e->getChild(0));
                    Type lhs = (lhNode != NULL)? lhNode->type.runtime() : Type::VOID;

                    if (lhs.returnType() != containingFunc->type) {
                        errors.push_back(Errors::badReturnValue(
                            e->token,
                            containingFunc->token,
                            lhs,
                            containingFunc->type)
                        );
                    }
                    if(lhs.isArray()){
                        errors.push_back(Errors::cannotReturnArray(e->token));
                    }

                    returnedFromYet = true;
                    resultType = Type::VOID;
                } break;
                /**
                 * Leave a function, check for no return warnings
                 */
                case FUNCTIONDECL:{
                    if(!returnedFromYet && containingFunc->type != Type::VOID){
                        errors.push_back(
                            Errors::missingReturnStatement(
                                e->token,
                                containingFunc->type)
                        );
                    }
                    containingFunc = NULL;
                    returnedFromYet = false;
                } break;
                default: break;
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
        errors.push_back(Errors::cannotBeCalled(call->token));
    }

    std::vector<Node*> empty;
    const std::vector<Node*>* required;
    const std::vector<Node*>* found;

    if(function->getChild(0) == NULL){
        required = &empty;
    } else {
        required = function->getChild(0)->viewChildren();
    }

    if(call->getChild(0) == NULL){
        found = &empty;
    } else {
        found = call->getChild(0)->viewChildren();
    }

    if(required->size() > found->size()) {
        errors.push_back(Errors::tooFewParameters(call->token, f->token));
    } else if (required->size() < found->size()) {
        errors.push_back(Errors::tooManyParameters(call->token, f->token));
    }

    for(size_t i=0; i<min(required->size(), found->size()); i++){
        Type r = required->at(i)->type;
        Type f = found->at(i)->type;
        if(r.isArray() && !f.isArray()){
            errors.push_back(Errors::expectedArrayParameter(
                call->token, i+1, function->token));
        } else if (f.isArray() && !r.isArray()){
            errors.push_back(Errors::unexpectedArrayParameter(
                call->token, i+1, function->token));
        }

        if(r.returnType() != f.returnType() && f!=Type::NONE){
            errors.push_back(Errors::badParameterType(
                call->token, f, r, i+1, function->token
                ));
        }
    }

    return function->type.returnType();
}
Type Semantics::checkOperands(Node* opNode, vector<Error*>& errors){
    Element* e = dynamic_cast<Element *>(opNode);
    Element* lhNode = dynamic_cast<Element *>(e->getChild(0));
    Element* rhNode = dynamic_cast<Element *>(e->getChild(1));
    Type lhs = (lhNode != NULL)? lhNode->type.runtime() : Type::NONE;
    Type lhsRaw = lhs.returnType();
    Type rhs = (rhNode != NULL)? rhNode->type.runtime() : Type::NONE;
    Type rhsRaw = rhs.returnType();
    int op = opNode->token->token;
    bool unary = (rhNode == NULL);

    // Array index operator
    if(op == '[') {
        if(!lhs.isArray()) {
            errors.push_back(Errors::cannotIndexNonarray(lhNode->token));
        }

        if(rhsRaw != Type::INT && rhsRaw != Type::NONE){
            errors.push_back(Errors::badArrayIndex(lhNode->token, rhs));
        }

        if(rhs.isArray()){
            errors.push_back(Errors::arrayIndexedByArray(rhNode->token));
        }

        return lhs.returnType();
    }

    // Unary operators
    typedef struct UnarySpecs{
        int op;
        bool arrays;
        Type requiredLhs;
    } UnarySpecs;
    UnarySpecs unaries[] = {
        {INC, false, Type::INT},
        {DEC, false, Type::INT},
        {'-', false, Type::INT},
        {NOT, false, Type::BOOL},
        {'*',  true, Type::NONE},
        {'?', false, Type::INT},
    };
    if(unary){
        for(size_t i=0; i<sizeof(unaries)/sizeof(unaries[0]); i++){
            UnarySpecs spec = unaries[i];
            if(op != spec.op) continue;

            if(lhs == Type::NONE) break;

            if(spec.requiredLhs != Type::NONE && spec.requiredLhs != lhsRaw){
                errors.push_back(Errors::unaryTypeMismatch(
                    e->token, lhs, spec.requiredLhs)
                );
            }
            if(spec.arrays && !lhs.isArray()){
                errors.push_back(Errors::opOnlyAcceptsArrays(e->token));
            }
            if(!spec.arrays && lhs.isArray()){
                errors.push_back(Errors::opDoesntAcceptArrays(e->token));
            }
        }
    }

    // Comparison operators
    typedef struct ComparisonSpec{
        int op;
    } ComparisonSpec;
    ComparisonSpec comparisons[] = {
        {LESSEQ},
        { GRTEQ},
        {   '>'},
        {   '<'},
    };
    for(size_t i=0; i<sizeof(comparisons)/sizeof(comparisons[0]); i++){
        ComparisonSpec spec = comparisons[i];
        if(op != spec.op) continue;


        bool badTypes = false;
        if(lhs != Type::NONE && !(lhsRaw == Type::INT || lhsRaw == Type::CHAR)){
            badTypes = true;
            errors.push_back(Errors::incorrectLHS(e->token, lhs, "type char or type int"));
        }
        if(rhs != Type::NONE && !(rhsRaw == Type::INT || rhsRaw == Type::CHAR)){
            badTypes = true;
            errors.push_back(Errors::incorrectRHS(e->token, rhs, "type char or type int"));
        }

        if(lhs == Type::NONE || rhs == Type::NONE) break;

        if(!badTypes && (lhsRaw != rhsRaw)){
            errors.push_back(Errors::mismatchedLR(e->token, lhs, rhs));
        }
        if(lhs.isArray() || rhs.isArray()){
            errors.push_back(Errors::opDoesntAcceptArrays(e->token));
        }
    }

    // Binary operators
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
    if(!unary){
        for(size_t i=0; i<sizeof(binaries)/sizeof(binaries[0]); i++){
            BinarySpec spec = binaries[i];
            if(op != spec.op) continue;

            bool badTypes = false;
            if(lhsRaw != spec.type && lhs!=Type::NONE){
                badTypes = true;
                errors.push_back(Errors::incorrectLHS(e->token, lhs, spec.type));
            }
            if(rhsRaw != spec.type && rhs != Type::NONE){
                badTypes = true;
                errors.push_back(Errors::incorrectRHS(e->token, rhs, spec.type));
            }

            if(lhs == Type::NONE || rhs == Type::NONE) break;

            if(!badTypes && (lhsRaw != rhsRaw)){
                errors.push_back(Errors::mismatchedLR(e->token, lhs, rhs));
            }
            if(lhs.isArray() || rhs.isArray()){
                errors.push_back(Errors::opDoesntAcceptArrays(e->token));
            }
        }
    }

    // Equality
    if((op == '=' || op == EQ || op == NOTEQ) &&
        !(lhs == Type::NONE || rhs == Type::NONE)){

        bool badTypes = false;
        if(lhsRaw == Type::VOID){
            badTypes = true;
            errors.push_back(Errors::incorrectLHS(e->token, lhs, "NONVOID"));
        }
        if(rhsRaw == Type::VOID){
            badTypes = true;
            errors.push_back(Errors::incorrectRHS(e->token, rhs, "NONVOID"));
        }

        if(!badTypes && (lhsRaw != rhsRaw)){
            errors.push_back(Errors::mismatchedLR(e->token, lhs, rhs));
        }

        if(lhs.isArray() ^ rhs.isArray()){
            errors.push_back(Errors::mismatchedArrayStatus(e->token));
        }
    }

    // Subexpression result value
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
bool Semantics::checkConstness(AST::Node* subtree){
    /**
     * Only references to other IDs can cause a tree to be considered non-const
     */
    class Traversal : public Node::Traverser {
    public:
        bool constness;
        Traversal(): constness(true) {}
        void pre(Node * n){
            if( dynamic_cast<const IdToken*>(n->token) != NULL ){
                constness = false;
            }
        }
    };
    Traversal t;
    subtree->traverse(t);
    return t.constness;
}
