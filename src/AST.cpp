#include "AST.hpp"

using namespace AST;

std::ostream& operator<<(std::ostream& os, const Node& n){
    os << n.toString();
    return os;
}

std::ostream& operator<<(std::ostream& os, const Node* n){
    os << n->toString();
    return os;
}

Node* AST::IdNode(const IdToken* id){
    return new Value(id, "Id: ", listof<Node*>());
}
Node* AST::ConstNode(const Token* t){
    return new Value(t, "Const: ", listof<Node*>());
}
Node* AST::RecordNode(const IdToken* id, Node* contents){
    return new Record(id, listof<Node*>() << contents);
}
Node* AST::CallNode(const IdToken* id, Node* args){
    return new Value(id, "Call: ", listof<Node*>() << args);
}
Node* AST::OpNode(const Token* t, Node* lhs, Node* rhs){
    return new Value(t, "Op: ", listof<Node*>() << lhs << rhs);
}
Node* AST::AssignNode(const Token* t, Node* lhs, Node* rhs){
    return new Value(t, "Assign: ", listof<Node*>() << lhs << rhs);
}
Node* AST::VarDecl(const IdToken* id, Type type){
    return new Decl(VAR, id, type, listof<Node*>());
}
Node* AST::VarDecl(const IdToken* id, Type type, Node* ivalue){
    return new Decl(VAR, id, type, listof<Node*>() << ivalue);
}
Node* AST::Parameter(const IdToken* id, Type type){
    return new Decl(PARAM, id, type, listof<Node*>());
}
Node* AST::FuncDecl(const IdToken* id, Type rtnt, Node* params, Node* compnd){
    return new Decl(FUNC, id, rtnt, listof<Node*>() << params << compnd);
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
Node* AST::Siblings(std::vector<Node*> sibs){
    return new SiblingList(sibs);
}
