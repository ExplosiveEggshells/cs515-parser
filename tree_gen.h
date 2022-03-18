#ifndef TREE_GEN_H
#define TREE_GEN_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <list>

#include "node.h"
#include "parse_exception.h"

using std::cout;
using std::endl;

class tree_gen
{
public:
    tree_gen();
    tree_gen(std::vector<Token> tokens);
    
    bool finished();
    void create_parse_tree(Node*& n);
    void print_tree(Node* n);
    void print_tree_pretty(Node* n, uint depth);
    void delete_tree(Node* n);
private:
    Token current_token;
    std::vector<Token> tokens;
    int token_iterator;

    void advance_iterator();

    
    void expression(Node*& n);
    void term(Node*& n);
    void power(Node*& n);
    void negation(Node*& n);
    void unit(Node*& n);

};

#endif