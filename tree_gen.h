#ifndef TREE_GEN_H
#define TREE_GEN_H

#include "token.h"
#include "parse_exception.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <list>

using std::cout;
using std::endl;

struct Node
{
    Node()
    {
        token = Token();
        child = nullptr;
        sibling = nullptr;
        test_num = 10;
    }

    Node(Token token) {
        this->token = token;
        child = nullptr;
        sibling = nullptr;
        test_num = 10;
    }

    Node* child;
    Node* sibling;
    Node* last_child;

    int test_num;
    Token token;
};

class tree_gen
{
public:
    tree_gen();
    tree_gen(std::vector<Token> tokens);
    
    void create_parse_tree();

private:
    Token current_token;
    std::vector<Token> tokens;
    int token_iterator;

    bool end_of_tokens();
    void advance_iterator();

    void print_tree(Node* n);
    
    void expression(Node*& n);
    void term(Node*& n);
    void power(Node*& n);
    void negation(Node*& n);
    void unit(Node*& n);

    Node* head;
    std::vector<char> operator_vector;
};

#endif