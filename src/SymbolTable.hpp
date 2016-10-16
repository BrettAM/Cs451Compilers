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
    SymTab(SymTab* parent): parent(parent) {}
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
    bool add(std::string name, AST::Node* value){
        AST::Node* existing = values[name];
        if(existing == NULL){
            values[name] = value;
            return true;
        }
        return false;
    }
};

class SymbolTable{
private:
    SymTab* table;
public:
    SymbolTable(){
        table = new SymTab(NULL);
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
    void enter(){
        table = new SymTab(table);
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
     * Adds a (name,value) pair to this scope
     * return true if the operation succeeds, false if `name` is already
     * associated
     */
    bool add(std::string name, AST::Node* value){
        return table->add(name, value);
    }
};

#endif
