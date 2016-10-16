#include "semantic.hpp"

using namespace std;
using namespace AST;
using namespace Semantics;

ChkResult Semantics::checkCall(AST::Node* call, AST::Node* function){
    return ChkResult(Type::NONE);
}
ChkResult Semantics::checkOperands(AST::Node* op, AST::Node* lhs, AST::Node* rhs){
    return ChkResult(Type::NONE);
}
ChkResult Semantics::checkOperands(AST::Node* op, AST::Node* rhs){
    return ChkResult(Type::NONE);
}
std::vector<Error*> Semantics::analyze(AST::Node* root){
    return std::vector<Error*>();
}
