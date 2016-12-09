#ifndef AST_H
#define AST_H

#include "Token.hpp"
#include "Type.hpp"
#include "Location.hpp"
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
            for(size_t i=0; i<other->list.size(); i++){
                list.push_back(other->list.at(i));
            }
            return this;
        }
        listof<T>* addAll(const std::vector<T>* const other){
            for(size_t i=0; i<other->size(); i++){
                list.push_back(other->at(i));
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
        /** Type of the value represented by or returned by this node */
        Type type;
        /** Location in data space where this nodes value will be stored */
        Location location;
        /** Location of the first instruction to computer/enter this node */
        Location codeStart;

        const Token * token;
        virtual ~Node(){};
        /**
         * Print the human readable description of this single node
         */
        virtual std::string toString(bool printTypes) const = 0;
        std::string toString() const { return toString(false); }
        /**
         * Recursivly check if two nodes contain the equally valued trees
         */
        bool operator==(const Node& n) const {
            return Node::equals(n) && this->equals(n);
        }
        /**
         * format the human readable description of the tree rooted here
         */
        std::string formatTree() { return formatTree(false); }
        std::string formatTree(bool types) {
            std::ostringstream oss;
            formatTree(0, "", oss, types);
            return oss.str();
        }
        virtual void formatTree(int indentlvl, std::string prefix, std::ostream& out, bool types) {
            out << prefix << this->toString(types) << std::endl;
            for(int i=0, size=children.size(); i<size; i++){
                std::ostringstream oss;
                for(int indent=0; indent<indentlvl; indent++){ oss << "!   "; }
                oss << "Child: " << i << "  ";
                children.at(i)->formatTree(indentlvl+1, oss.str(), out, types);
            }
        }
        /**
         * Call a pre and/or post method while traversing the tree from
         * this location
         */
        template <typename NodeSubclass = Node>
        class Traverser {
        public:
            /** Called before a node's children are traversed */
            virtual void pre(NodeSubclass *){}
            /**
             * Called as a node's children are traversed.
             * Index will be the index of the node just finished being
             *   traversed.
             */
            virtual void inorder(NodeSubclass *, int index){}
            /** Called after a node's children are traversed */
            virtual void post(NodeSubclass *){}
        };
        template <typename NodeSubclass>
        void traverse(Traverser<NodeSubclass>& t){
            NodeSubclass* cast = dynamic_cast<NodeSubclass *>(this);
            if(cast != NULL) t.pre(cast);
            for(int i=0, size=children.size(); i<size; i++){
                children.at(i)->traverse(t);
                if(cast != NULL) t.inorder(cast, i);
            }
            if(cast != NULL) t.post(cast);
        }
        /**
         * Call to recursivly delete each tree node
         */
        void deleteTree() {
            class Deleter : public Traverser<Node> {
                void post(Node * n){ deleteNode(n); }
            } deleter;
            this->traverse(deleter);
        }
        /**
         * Return the pointer to this node's i'th child, or NULL if it doesn't
         *   exist.
         */
        Node* getChild(size_t i){
            if(i < 0 || i >= children.size()) return NULL;
            return children[i];
        }
        const std::vector<Node*>* const viewChildren(){
            return &children;
        }
    protected:
        std::vector<Node*> children;
        Node(const Token* token)
            : type(Type::NONE), location(), token(token), children() {}
        Node(const Token* token, const std::vector<Node*>& children)
            : type(Type::NONE), location(), token(token), children(scrubNulls(children)) {
        }
        virtual bool equals(const Node& n) const {
            bool tokensEqual = (token == NULL && n.token == NULL) ||
                                ((token != NULL) && (*token==*n.token));
            if(!tokensEqual) return false;
            if(type != n.type) return false;
            if(children.size() != n.children.size()) return false;
            for(size_t i=0; i<children.size(); i++){
                if( !(*children[i] == *n.children[i]) ) return false;
            }
            return true;
        }
    };
    std::ostream& operator<<(std::ostream&, const Node&);
    std::ostream& operator<<(std::ostream&, const Node*);

    class SiblingList: public Node {
    private:
    public:
        SiblingList(std::vector<Node*> children): Node(NULL, scrubLeaves(children)) {}
        std::string toString(bool types) const { return "Sibling List"; }
        void formatTree(int indentlvl, std::string prefix, std::ostream& out, bool types) {
            // if siblings are on the base line, they can't "retract" one level
            // correctly without a cludge like this
            if(indentlvl == 0) indentlvl++;

            if(children.size() == 0) return;
            children.at(0)->formatTree(indentlvl, prefix, out,types);
            for(int i=1, size=children.size(); i<size; i++){
                std::ostringstream oss;
                for(int indent=1; indent<indentlvl; indent++){ oss << "!   "; }
                oss << "Sibling: " << i-1 << "  ";
                children.at(i)->formatTree(indentlvl, oss.str(), out, types);
            }
        }
    protected:
        virtual bool equals(const Node& n) const {
            return dynamic_cast<SiblingList const *>(&n) != NULL;
        }
    };

    class LeafNode: public Node {
    public:
        LeafNode(): Node(NULL) {}
        std::string toString(bool types) const { return ""; }
        void formatTree(int indentlvl, std::string s, std::ostream& out, bool types) {}
    protected:
        virtual bool equals(const Node& n) const {
            return dynamic_cast<LeafNode const *>(&n) != NULL;
        }
    };

    class Element;
    class PrintStyle {
    public:
        std::string toString(const Element* n, bool types);
    protected:
        virtual void print(const Element* n, std::ostringstream& s) = 0;
    };

    enum ElementType {
        VALUE,
        DECLARATION,
        FUNCTIONDECL,
        CALL,
        OPERATION,
        ASM,
        CONTROL,
        COMPOUND,
        BREAK,
        RECORD,
        PARAMETER,
        RETURNSTMT
    };

    class Element: public Node {
    private:
        PrintStyle* printer;
    public:
        const ElementType nodeType;
        Element(ElementType nodeType, PrintStyle* printer,
                const Token* token, std::vector<Node*> children):
            Node(token, children), printer(printer), nodeType(nodeType) {}
        std::string toString(bool types) const { return printer->toString(this, types); }
    protected:
        virtual bool equals(const Node& n) const {
            if(Element const * p = dynamic_cast<Element const *>(&n)){
                return nodeType == p->nodeType && printer == p->printer;
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
    Node* BreakNode(const Token* t);
    Node* ASMNode(const Token* t);
    Node* Siblings(std::vector<Node*> sibs);
    Node* Leaf();
}

#endif
