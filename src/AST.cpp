#include "AST.hpp"

using namespace std;
using namespace AST;

std::ostream& AST::operator<<(std::ostream& os, const Node& n){
    os << n.toString();
    return os;
}

std::ostream& AST::operator<<(std::ostream& os, const Node* n){
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
        if( *itr != NULL && dynamic_cast<LeafNode const *>(*itr) == NULL ) {
            result.push_back(*itr);
        } else {
            delete *itr;
        }
    }
    return result;
}

/**
 * This function in PrintStyle wraps `virtual print`
 * It should set up the output stream and print elements common to all
 * element lines
 */
std::string PrintStyle::toString(const Element* n, bool types){
    std::ostringstream oss;
    this->print(n, oss);
    if(types){
        oss << " " << n->location.textBox() << n->type.typeBox();
    }
    oss << " " << n->token->lineBox();
    return oss.str();
}

/**
 * Various different Elements have different styles of printing themselves
 * but are not meaningfully different in many other ways, so each Elements is
 * assigned a print style when created
 */
namespace Printers {
    class LabelAndNamePrinter : public PrintStyle {
    cstr label;
    public:
        LabelAndNamePrinter(cstr label): label(label) {}
    protected:
        void print(const Element* n, std::ostringstream& s){
            s << label << n->token->nodeLabel();
        }
    };
    class LabelPrinter : public PrintStyle {
    cstr label;
    public:
        LabelPrinter(cstr label): label(label) {}
    protected:
        void print(const Element* n, std::ostringstream& s){
            s << label;
        }
    };
    class LabelAndTypePrinter : public PrintStyle {
    cstr label;
    public:
        LabelAndTypePrinter(cstr label): label(label) {}
    protected:
        void print(const Element* n, std::ostringstream& s){
            s << label << n->token->nodeLabel() << n->type.predicate();
        }
    };
    class FuncDeclPrinter : public PrintStyle {
    cstr label;
    public:
        FuncDeclPrinter(cstr label): label(label) {}
    protected:
        void print(const Element* n, std::ostringstream& s){
            s << label << n->token->nodeLabel() << " returns type ";
            s << n->type.rawString();
        }
    };
    class RecordPrinter : public PrintStyle {
    protected:
        void print(const Element* n, std::ostringstream& s){
            s << "Record " << n->token->nodeLabel() << " ";
        }
    };
}

using namespace Printers;

Node* AST::IdNode(const IdToken* t){
    static LabelAndNamePrinter printer("Id: ");
    return new Element(VALUE, &printer, t, listof<Node*>());
}
Node* AST::ConstNode(const Token* t){
    static LabelAndNamePrinter printer("Const: ");
    Node* e = new Element(VALUE, &printer, t, listof<Node*>());
    e->type = t->getType();
    return e;
}
Node* AST::RecordNode(const IdToken* t, Node* contents){
    static RecordPrinter printer;
    return new Element(RECORD, &printer, t, listof<Node*>() << contents);
}
Node* AST::CallNode(const IdToken* t, Node* args){
    static LabelAndNamePrinter printer("Call: ");
    return new Element(CALL, &printer, t, listof<Node*>() << args);
}
Node* AST::OpNode(const Token* t, Node* lhs, Node* rhs){
    static LabelAndNamePrinter printer("Op: ");
    return new Element(OPERATION, &printer, t, listof<Node*>() << lhs << rhs);
}
Node* AST::AssignNode(const Token* t, Node* lhs, Node* rhs){
    static LabelAndNamePrinter printer("Assign: ");
    return new Element(OPERATION, &printer, t, listof<Node*>() << lhs << rhs);
}
Node* AST::VarDecl(const IdToken* t, Type type){
    static LabelAndTypePrinter printer("Var ");
    Element* e = new Element(DECLARATION, &printer, t, listof<Node*>());
    e->type = type;
    return e;
}
Node* AST::VarDecl(const IdToken* t, Type type, Node* ivalue){
    static LabelAndTypePrinter printer("Var ");
    Element* e = new Element(DECLARATION, &printer, t, listof<Node*>() << ivalue);
    e->type = type;
    return e;
}
Node* AST::Parameter(const IdToken* t, Type type){
    static LabelAndTypePrinter printer("Param ");
    Element* e = new Element(PARAMETER, &printer, t, listof<Node*>());
    e->type = type;
    return e;
}
Node* AST::FuncDecl(const IdToken* t, Type rtnt, Node* params, Node* compnd){
    static FuncDeclPrinter printer("Func ");
    Element* e = new Element(FUNCTIONDECL, &printer, t, listof<Node*>() << params << compnd);
    e->type = rtnt;
    return e;
}
Node* AST::Compound(const Token* t, Node* inits, Node* stmts){
    static LabelPrinter printer("Compound");
    Element* e = new Element(COMPOUND, &printer, t, listof<Node*>() << inits << stmts);
    e->type = Type::VOID;
    return e;
}
Node* AST::IfNode(const Token* t, Node* cond, Node* tcase, Node* fcase){
    static LabelPrinter printer("If");
    Element* e = new Element(CONTROL, &printer, t, listof<Node*>() << cond << tcase << fcase);
    e->type = Type::VOID;
    return e;
}
Node* AST::WhileNode(const Token* t, Node* cond, Node* stmts){
    static LabelPrinter printer("While");
    Element* e = new Element(CONTROL, &printer, t, listof<Node*>() << cond << stmts);
    e->type = Type::VOID;
    return e;
}
Node* AST::ReturnNode(const Token* t, Node* expr){
    static LabelPrinter printer("Return");
    return new Element(RETURNSTMT, &printer, t, listof<Node*>() << expr);
}
Node* AST::BreakNode(const Token* t){
    static LabelPrinter printer("Break");
    Element* e = new Element(BREAK, &printer, t, listof<Node*>());
    e->type = Type::VOID;
    return e;
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
