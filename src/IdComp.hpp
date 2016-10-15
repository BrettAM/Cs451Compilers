#ifndef IDCOMP_H
#define IDCOMP_H

/**
 * Pod class used in bison to bring data about an ID in a parameter or
 * declaration list up to where the type specifier is available
 */
struct IdComp {
    const IdToken* id;
    int arraylen;
    AST::Node* init;
};

#endif

