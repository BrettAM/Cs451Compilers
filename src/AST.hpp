#ifndef AST_H
#define AST_H

#include "Token.hpp"
#include "Type.hpp"
#include <list>
#include <vector>

namespace AST{
    template <typename T>
    class listof{
    private:
        std::vector<T> list;
    public:
        listof(){}
        listof<T> operator<<(T data){
            list.push_back(data);
            return *this;
        }
        listof<T>* add(T data){
            list.push_back(data);
            return this;
        }
        listof<T>* addAll(listof<T>* other){
            for(int i=0; i<other->list.size(); i++){
                list.push_back(other->list.at(i));
            }
            return this;
        }
        operator std::vector<T>() const {
            return list;
        }
    };

    /**
     * Deletes a single node pointer
     * Call node->postorder<&deletNode> to recursivly delete the AST
     */
    class Node;
    void deleteNode(Node *);

    /**
     * Returns a new vector that is a copy of the input except it omits nulls
     */
    std::vector<Node*> scrubNulls(const std::vector<Node*>& nodes);
    /**
     * Returns a new vector that is a copy of the input except it deletes leaves
     */
    std::vector<Node*> scrubLeaves(const std::vector<Node*>& nodes);

    /**
     * Abstract Base Class for all AST elements
     */
    class Node {
    public:
        const Token * token;
        virtual ~Node(){};
        /**
         * Print the human readable description of this single node
         */
        virtual std::string toString() const = 0;
        /**
         * Recursivly check if two nodes contain the equally valued trees
         */
        bool operator==(const Node& n) const {
            return Node::equals(n) && this->equals(n);
        }
        /**
         * format the human readable description of the tree rooted here
         */
        std::string formatTree() {
            std::ostringstream oss;
            formatTree(0, "", oss);
            return oss.str();
        }
        virtual void formatTree(int indentlvl, std::string prefix, std::ostream& out) {
            out << prefix << this->toString() << std::endl;
            for(int i=0, size=children.size(); i<size; i++){
                std::ostringstream oss;
                for(int indent=0; indent<indentlvl; indent++){ oss << "!   "; }
                oss << "Child: " << i << "  ";
                children.at(i)->formatTree(indentlvl+1, oss.str(), out);
            }
        }
        /**
         * Call <OP> on each node after <OP> has been called on all its
         * children
         */
        template <void (*OP)(Node*)> void postorder(){
            for(int i=0, size=children.size(); i<size; i++){
                children.at(i)->postorder<OP>();
            }
            OP(this);
        }
        /**
         * Call to recursivly delete each tree node
         */
        void deleteTree() {
            this->postorder<deleteNode>();
        }
    protected:
        std::vector<Node*> children;
        Node(const Token* token): token(token), children() {}
        Node(const Token* token, const std::vector<Node*>& children)
            : token(token), children(scrubNulls(children)) {
        }
        virtual bool equals(const Node& n) const {
            return token==n.token && children==n.children;
        }
    };
    std::ostream& operator<<(std::ostream&, const Node&);
    std::ostream& operator<<(std::ostream&, const Node*);

    class SiblingList: public Node {
    private:
    public:
        SiblingList(std::vector<Node*> children): Node(NULL, scrubLeaves(children)) {}
        std::string toString() const { return "Sibling List"; }
        void formatTree(int indentlvl, std::string prefix, std::ostream& out) {
            // if siblings are on the base line, they can't "retract" one level
            // correctly without a cludge like this
            if(indentlvl == 0) indentlvl++;

            if(children.size() == 0) return;
            children.at(0)->formatTree(indentlvl, prefix, out);
            for(int i=1, size=children.size(); i<size; i++){
                std::ostringstream oss;
                for(int indent=1; indent<indentlvl; indent++){ oss << "!   "; }
                oss << "Sibling: " << i-1 << "  ";
                children.at(i)->formatTree(indentlvl, oss.str(), out);
            }
        }
    protected:
        virtual bool equals(const Node& n) const {
            return dynamic_cast<SiblingList const *>(&n) != NULL;
        }
    };

    enum DeclType { VAR, PARAM, FUNC };
    class Decl: public Node {
    private:
        const DeclType dt;
        Type type;
    public:
        Decl(DeclType dt, const IdToken* tok, Type type, std::vector<Node*> ln):
            Node(tok, ln), dt(dt), type(type) {}
        std::string toString() const {
            std::ostringstream oss;
            switch(dt){
                case VAR:   oss << "Var "; break;
                case PARAM: oss << "Param "; break;
                case FUNC:
                    oss << "Func " << token->nodeLabel() << " returns type ";
                    oss << type.rawString() << " " << token->lineBox();
                    return oss.str();
            }
            oss << token->nodeLabel() << type << " " << token->lineBox();
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
        Record(const Token* token, std::vector<Node*> children):
            Node(token, children) {}
        std::string toString() const {
            std::ostringstream oss;
            oss << "Record " << token->nodeLabel() << "  " << token->lineBox();
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
        Value(const Token* token, cstr label, std::vector<Node*> children):
            Node(token, children), pt(true), label(label) {}
        Value(const Token* token, cstr label, bool pt, std::vector<Node*> children):
            Node(token, children), pt(pt), label(label) {}
        std::string toString() const {
            std::ostringstream oss;
            oss << label;
            if(pt){
                oss << token->nodeLabel() << " ";
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

    class LeafNode: public Node {
    public:
        LeafNode(): Node(NULL) {}
        std::string toString() const { return ""; }
        void formatTree(int indentlvl, std::string s, std::ostream& out) {}
    protected:
        virtual bool equals(const Node& n) const {
            return dynamic_cast<LeafNode const *>(&n) != NULL;
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
    Node* BreakNode(const Token* t);
    Node* Siblings(std::vector<Node*> sibs);
    Node* Leaf();
}

#endif
