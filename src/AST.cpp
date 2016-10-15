#include "AST.hpp"

using namespace std;
using namespace AST;

std::ostream& operator<<(std::ostream& os, const Node& n){
    os << n.toString();
    return os;
}

std::ostream& operator<<(std::ostream& os, const Node* n){
    os << n->toString();
    return os;
}

std::vector<Node*> AST::scrubNulls(const std::vector<Node*>& nodes){
    std::vector<Node*> result;
    for(vector<Node*>::const_iterator itr = nodes.begin();
        itr != nodes.end();
        ++itr)
      {
        if(*itr != NULL) result.push_back(*itr);
    }
    return result;
}

std::vector<Node*> AST::scrubLeaves(const std::vector<Node*>& nodes){
    std::vector<Node*> result;
    for(vector<Node*>::const_iterator itr = nodes.begin();
        itr != nodes.end();
        ++itr)
      {
        if( dynamic_cast<LeafNode const *>(*itr) == NULL )
            result.push_back(*itr);
        else
            delete *itr;
    }
    return result;
}

Node* AST::IdNode(const IdToken* t){
    return new Value(t, "Id: ", listof<Node*>());
}
Node* AST::ConstNode(const Token* t){
    return new Value(t, "Const: ", listof<Node*>());
}
Node* AST::RecordNode(const IdToken* t, Node* contents){
    return new Record(t, listof<Node*>() << contents);
}
Node* AST::CallNode(const IdToken* t, Node* args){
    return new Value(t, "Call: ", listof<Node*>() << args);
}
Node* AST::OpNode(const Token* t, Node* lhs, Node* rhs){
    return new Value(t, "Op: ", listof<Node*>() << lhs << rhs);
}
Node* AST::AssignNode(const Token* t, Node* lhs, Node* rhs){
    return new Value(t, "Assign: ", listof<Node*>() << lhs << rhs);
}
Node* AST::VarDecl(const IdToken* t, Type type){
    return new Decl(VAR, t, type, listof<Node*>());
}
Node* AST::VarDecl(const IdToken* t, Type type, Node* ivalue){
    return new Decl(VAR, t, type, listof<Node*>() << ivalue);
}
Node* AST::Parameter(const IdToken* t, Type type){
    return new Decl(PARAM, t, type, listof<Node*>());
}
Node* AST::FuncDecl(const IdToken* t, Type rtnt, Node* params, Node* compnd){
    return new Decl(FUNC, t, rtnt, listof<Node*>() << params << compnd);
}
Node* AST::Compound(const Token* t, Node* inits, Node* stmts){
    return new Value(t, "Compound ", false, listof<Node*>() << inits << stmts);
}
Node* AST::IfNode(const Token* t, Node* cond, Node* tcase, Node* fcase){
    return new Value(t, "If ", false, listof<Node*>() << cond << tcase << fcase);
}
Node* AST::WhileNode(const Token* t, Node* cond, Node* stmts){
    return new Value(t, "While ", false, listof<Node*>() << cond << stmts);
}
Node* AST::ReturnNode(const Token* t, Node* expr){
    return new Value(t, "Return ", false, listof<Node*>() << expr);
}
Node* AST::BreakNode(const Token* t){
    return new Value(t, "Break ", false, listof<Node*>());
}
Node* AST::Siblings(std::vector<Node*> sibs){
    // empty sibling lists should be omitted from the tree
    if(sibs.size() == 0) return Leaf();
    return new SiblingList(sibs);
}
Node* AST::Leaf(){
    return new LeafNode();
}
void AST::deleteNode(Node * p){
    delete p;
}
