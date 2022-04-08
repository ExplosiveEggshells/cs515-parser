/**
 * @file tree_gen.h
 * @author Jake Rogers (z1826513)
 * @brief Given a bank of tokens, a parse tree will be generated headed at
 * any given uninitialized node.
 *
 * The given parse tree will be the next valid expression within the
 * bank of tokens, so 1+2 3*4-5 would need to call create_parse_tree twice
 * to get two trees. The 'finished' function will return true when no more
 * expressions can be built from the file to allow this to be easily used in
 * a loop.
 *
 * @note When a ParseException is thrown during the tree generation process, the
 * entire process is halted and the invalid expression will be incomplete in the
 * Node used as the head. This means the remainder of the tokens will not be processed,
 * since an error recovery function would be needed to figure out the next best
 * place to continue.
 */
#ifndef TREE_GEN_H
#define TREE_GEN_H

#include <iostream>
#include <vector>

#include "node.h"
#include "parse_exception.h"

using std::cout;
using std::endl;

class tree_gen
{
public:
    // Create a generator and give it a bank of tokens to create
    // expressions from.
    tree_gen(std::vector<Token> tokens);

    // When true, the token bank is empty. No more valid expressions can
    // be created.
    bool finished();

    // Parse through the token vector to build an expression and head it's
    // parse tree at n.
    void create_parse_tree(Node *&n);

    // Given the head of a parse tree, print out it's POST-order traversal as
    // a comma separated line. This is how the machine will view it
    void print_tree(Node *n);

    // Print out a somewhat easier to view PRE-order traversal of a parse tree,
    // using indentation to denote parent-child relations.
    void print_tree_pretty(Node *n, uint depth);

    // Delete all of the memory in a parse tree in a post-order traversal.
    // This can be done immediately once the tree is stored as a program!
    void delete_tree(Node *n);

private:
    // Moves the integer-iterator of 'tokens' forward by one and assigns
    // the next token to 'current_token'.
    void advance_iterator();


    /**
     * The following functions creates a recursive descent parser for 
     * arithmetic expressions.
     * 
     * Note that any 'sequence' described below and contain ONE or more
     * elements to be considered valid, so a power can be just one negation, or
     * many negations exponentiated against each other.
     */

    // An expression is a sum or difference of a sequence of terms.
    void expression(Node *&n);

    // A term is a product or quotient of a sequence of powers.
    void term(Node *&n);

    // A power is a sequence of exponentiations on negations.
    void power(Node *&n);

    // A negation is a unary + or negation on a single number.
    void negation(Node *&n);

    // A unit is either a single number, or the result of a new parentheized expression.
    void unit(Node *&n);

    Token current_token;        // tokens[token_iterator]
    std::vector<Token> tokens;  // List of tokens potentially describing one or many arithmetic expressions
    int token_iterator;         // Iterator for tokens.
};

#endif