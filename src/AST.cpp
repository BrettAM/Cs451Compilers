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
