#ifndef NODE_H
#define NODE_H

#include "token.h"

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

#endif