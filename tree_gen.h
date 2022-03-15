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

    bool isOperator(Token t);

    /**
     * @brief Determines whether or not the + or - operator
     * given the supplied vector is a unary one or not.
     * 
     * A + or - is considered unary if it matches the following conditions:
     * 1) The token before it (to the left) EITHER be an operator, or the token is the first token.
     * 2) The token after it (to the right) must be an integer, identifier, or '(' (Which, in itself, is expected to be a integer).
     * 
     */
    bool isUnary(std::list<Token>& token_list, std::list<Token>::iterator& tok_it);
    void init_operator_vector();
    bool end_of_tokens();
    void advance_iterator();


    void print_tree(Node* n);
    
    

    Node* head;
    std::vector<char> operator_vector;
};

#endif