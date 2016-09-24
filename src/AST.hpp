#ifndef AST_H
#define AST_H

#include "Token.hpp"
#include "Type.hpp"
#include <list>

namespace AST{
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
        const Type& type;
    public:
        Decl(DeclType dt, const IdToken* tok, const Type& type):
            Node(tok), dt(dt), type(type) {}
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

    class Value: public Node {
    private:
        cstr label;
    public:
        Value(const IdToken* id): Node(id), label("Id: ") {}
        Value(const Token* tok): Node(tok), label("Const: ") {}
        std::string toString() const {
            std::ostringstream oss;
            oss << label << token->text << " " << token->lineBox();
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
}

#endif
