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

    /**
    * @brief Given a list of tokens and an iterator to a position within it, insert
    * a brace either forward or backwards into the list (based on 'forward').
    * 
    * The trick is that we want to respect already-existing bracket pairs. Therefore, 
    * we must skip over any blocks that are already encapsulated by parentheses.
    * 
    * Example: 5 + 3 with the iterator at '+' creates (5 + 3)
    * Example: (5 * 3) + 1 with the iterator at '+' creates
    * ((5 * 3) + 1)
    * 
    * In the last example, we moved left beyond the group to create a bracket
    * on the left-side of the multiplication operation.
    * 
    * @param list_tokens List of tokens for an arithmetic sequence
    * @param it Iterator in list_tokens containing the operation to encapsulate
    * @param forward If true, make a right-side bracket. If false, make a left-side bracket.
    */
    void insert_bracket(std::list<Token> list_tokens, std::list<Token>::iterator it, bool forward, bool unary);

    /**
    * @brief Modifies v_tokens by inserting bracket pairs to explicitly
    * demarcate the order in which the recursive parser should evaluate the
    * operations.
    * 
    * This is performed by parsing over the token list once per each 'order' of operations
    * (*,/, then +, -)
    * 
    * For example, the token list: 5 + 3 * 8 becomes (5 + (3 * 8))
    * Another example:
    * 1 + 5 * 3 + 8 * 4 * 9 becomes:
    * ((1+(5*3))+((8*4)*9))
    * 
    * The purpose of doing so it to tell the recursive descent parser when a subtree needs
    * to be evaluated in order build the operation it is currently working on. Encountering
    * a '(' means we need to drop down a level.
    * 
    * Once nice side effect of this method is that parantheses already in the expression are
    * natively handled.
    * 
    * @param v_tokens An arithmetic operation given as a vector of tokens.
    */
    void format_expr(std::vector<Token>& toks);
    void print_tree(Node* n);
    
    Node* expression();
    void terminal(Node*& n);
    void variable(Node*& n);

    void recursive_parse(Node*& n, bool branching);

    Node* head;
    std::vector<char> operator_vector;
};

#endif