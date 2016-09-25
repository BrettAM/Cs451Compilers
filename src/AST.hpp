#ifndef AST_H
#define AST_H

#include "Token.hpp"
#include "Type.hpp"
#include <list>

namespace AST{
    template <typename T>
    class listof{
    private:
        std::list<T> list;
    public:
        listof(){}
        listof<T> operator<<(T data){
            list.push_back(data);
            return *this;
        }
        operator std::list<T>() const {
            return list;
        }
    };

    /**
     * Abstract Base Class for all AST elements
     */
    class Node {
    public:
        const Token * token;
        /**
         *
         */
        virtual std::string toString() const = 0;
        /**
         * Recursivly check if two nodes contain the equally valued trees
         */
        bool operator==(const Node& n) const {
            return Node::equals(n) && this->equals(n);
        }
    protected:
        std::list<Node*> children;
        Node(const Token* token): token(token), children() {}
        Node(const Token* token, const std::list<Node*>& children)
            : token(token), children(children) {}
        virtual bool equals(const Node& n) const{
            return token==n.token && children==n.children;
        }
    };
    std::ostream& operator<<(std::ostream&, const Node&);
    std::ostream& operator<<(std::ostream&, const Node*);

    enum DeclType { VAR, PARAM, FUNC };
    class Decl: public Node {
    private:
        const DeclType dt;
        Type type;
    public:
        Decl(DeclType dt, const IdToken* tok, Type type, std::list<Node*> ln):
            Node(tok, ln), dt(dt), type(type) {}
        std::string toString() const {
            std::ostringstream oss;
            switch(dt){
                case VAR:   oss << "Var "; break;
                case PARAM: oss << "Param "; break;
                case FUNC:
                    oss << "Func " << token->text << " returns type ";
                    oss << type.rawString() << " " << token->lineBox();
                    return oss.str();
            }
            oss << token->text << type << " " << token->lineBox();
            return oss.str();
        }
    protected:
        virtual bool equals(const Node& n) const {
            if(Decl const * p = dynamic_cast<Decl const *>(&n)){
                return dt == p->dt && type == p->type;
            }
            return false;
        }
    };

    class Record: public Node {
    private:
    public:
        Record(const Token* token, std::list<Node*> children):
            Node(token, children) {}
        std::string toString() const {
            std::ostringstream oss;
            oss << "Record " << token->text << "  " << token->lineBox();
            return oss.str();
        }
    protected:
        virtual bool equals(const Node& n) const {
            return dynamic_cast<Record const *>(&n) != NULL;
        }
    };

    class Value: public Node {
    private:
        bool pt;
        cstr label;
    public:
        Value(const Token* token, cstr label, std::list<Node*> children):
            Node(token, children), pt(true), label(label) {}
        Value(const Token* token, cstr label, bool pt, std::list<Node*> children):
            Node(token, children), pt(pt), label(label) {}
        std::string toString() const {
            std::ostringstream oss;
            oss << label;
            if(pt){
                oss << token->text << " ";
            }
            oss << token->lineBox();
            return oss.str();
        }
    protected:
        virtual bool equals(const Node& n) const {
            if(Value const * p = dynamic_cast<Value const *>(&n)){
                return label == p->label;
            }
            return false;
        }
    };

    Node* IdNode(const IdToken* id);
    Node* ConstNode(const Token* t);
    Node* RecordNode(const IdToken* id, Node* l);
    Node* CallNode(const IdToken* id, Node* args);
    Node* OpNode(const Token* t, Node* lhs, Node* rhs);
    Node* AssignNode(const Token* t, Node* lhs, Node* rhs);
    Node* VarDecl(const IdToken* id, Type type);
    Node* VarDecl(const IdToken* id, Type type, Node* initialvalue);
    Node* Parameter(const IdToken* id, Type type);
    Node* FuncDecl(const IdToken* id, Type rtntype, Node* params, Node* compnd);
    Node* Compound(const Token* t, Node* inits, Node* stmts);
    Node* IfNode(const Token* t, Node* cond, Node* tcase, Node* fcase);
    Node* WhileNode(const Token* t, Node* cond, Node* stmts);
    Node* ReturnNode(const Token* t, Node* expr);
}

#endif
