#ifndef SYMTAB_H
#define SYMTAB_H

#include <vector>
#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <map>

#include "Token.hpp"
#include "AST.hpp"

class SymTab{
private:
    std::map<std::string,AST::Node*> values;
public:
    SymTab* parent;
    AST::Node* block;
    SymTab(SymTab* parent, AST::Node* block): parent(parent), block(block) {}
    AST::Node* lookup(std::string name){
        AST::Node* val = values[name];
        if(val != NULL){
            return val;
        } else if(parent != NULL){
            return parent->lookup(name);
        } else {
            return NULL;
        }
    }
    bool lookupLocal(std::string name){
        return (values[name] != NULL);
    }
    bool add(std::string name, AST::Node* value){
        AST::Node* existing = values[name];
        if(existing == NULL){
            values[name] = value;
            return true;
        }
        return false;
    }
    AST::Node* getBlock() {
        return block;
    }
};

class SymbolTable{
private:
    SymTab* table;
public:
    SymbolTable(){
        table = new SymTab(NULL, NULL);
    }
    ~SymbolTable(){
        while(table->parent != NULL){
            exit();
        }
        delete table;
    }
    /**
     * Enter a new scope
     */
    void enter(AST::Node* block){
        table = new SymTab(table,block);
    }
    /**
     * Exit the current scope
     */
    void exit(){
        SymTab* newtable = table->parent;
        if(newtable == NULL) {
            std::cerr << "Attempt to exit global scope";
            return;
        }
        delete table;
        table = newtable;
    }
    /**
     * Returns the node associated with `name`, or NULL if not found
     */
    AST::Node* lookup(std::string name){
        return table->lookup(name);
    }
    /**
     * Returns if a node is associated with `name` in the local scope
     */
    bool lookupLocal(std::string name){
        return table->lookupLocal(name);
    }
    /**
     * Adds a (name,value) pair to this scope
     * return true if the operation succeeds, false if `name` is already
     * associated
     */
    bool add(std::string name, AST::Node* value){
        return table->add(name, value);
    }
    /**
     * Returns the Node* corresponding to the current scopes tree block
     * will return NULL if the symbol table is in the global scope
     */
    AST::Node* getBlock(){
        return table->block;
    }
};

#endif
