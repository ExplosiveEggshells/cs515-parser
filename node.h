/**
 * @file node.h
 * @author Jake Rogers (z1826513)
 * @brief A singular node in an arithmetic expression's parse
 * tree. Trees with these nodes are m-ary and connected in a zig-zag
 * pattern: A parent connects to it's first child, and each child contains
 * a pointer to it's next potential sibling.
 * 
 * The node's primary data is the token it holds.
 * 
 */

#ifndef NODE_H
#define NODE_H

#include "token.h"

enum NodeType
{
    NULL = 0;
    INTEGER = 1;
    STRING_CONST = 2;
    BOOLEAN = 3;    // Not used for A3
}

struct Node
{
    Node()
    {
        token = Token();
        type = NodeType::NULL;
        child = nullptr;
        sibling = nullptr;
    }

    Node(Token token) {
        this->token = token;
        type = NodeType::NULL;
        child = nullptr;
        sibling = nullptr;
    }

    Node(Token token, NodeType type) {
        this->token = token;
        this->type = type;
        child = nullptr;
        sibling = nullptr;
    }

    Node* child;
    Node* sibling;

    Token token;
    NodeType type;
};

#endif