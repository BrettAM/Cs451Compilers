#ifndef SYMTAB_H
#define SYMTAB_H

#include <vector>
#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <map>

#include "Token.hpp"
#include "AST.hpp"
#include "Error.hpp"

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
    Error* add(std::string name, AST::Node* value){
        AST::Node* existing = values[name];
        if(existing == NULL){
            values[name] = value;
            return NULL;
        } else {
            int lineno = (existing->token != NULL)? existing->token->line : -1;
            return Errors::alreadyDefined(name, lineno);
        }
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
     * Adds a (name,value) pair to this scope, returning an error
     * if it is already defined
     */
    Error* add(std::string name, AST::Node* value){
        return table->add(name, value);
    }
};

#endif
