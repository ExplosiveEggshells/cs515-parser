#include "tree_gen.h"

tree_gen::tree_gen() 
{
    token_iterator = -1;
}

tree_gen::tree_gen(std::vector<Token> tokens)
{
    token_iterator = -1;
    this->tokens = tokens;
}

void tree_gen::advance_iterator()
{
    cout << current_token.value << endl;
    if (current_token.id == 3)
    {
        throw ParseException("Attempted to advance beyond End of Expression during code tree generation.", current_token);
    }
    token_iterator++;
    current_token = tokens[token_iterator];
}


void tree_gen::create_parse_tree()
{
    tokens.push_back(Token("Parser EOX", 3));   // Add a ETX to signify an End of Expression. There should be no attempt to advance beyond this token.
    advance_iterator();
    expression(head);
    print_tree(head);
    cout << endl;
}

void tree_gen::print_tree(Node* n)
{
    if (n->child != nullptr)
    {
        print_tree(n->child);
    }

    std::cout << n->token.value << ",";

    if (n->sibling != nullptr)
    {
        print_tree(n->sibling);
    }
    
}

void tree_gen::expression(Node*& n)
{
    Node *t1, *t2, *t3;
    term(t1);

    while(true)
    {
        if (current_token.id == '+' || current_token.id == '-')
        {
            Token op_token = current_token;
            advance_iterator();
            term(t2);
            t3 = new Node(op_token);
            t3->child = t1;
            t3->child->sibling = t2;
            t1 = t3;
        }
        else
        {
            n = t1;
            break;
        }
    }
}
void tree_gen::term(Node*& n)
{
    Node *t1, *t2, *t3;
    power(t1);

    while (true)
    {
        if (current_token.id == '*' || current_token.id == '/' || current_token.id == TypeID::MOD)
        {
            Token op_token = current_token;
            advance_iterator();
            power(t2);
            t3 = new Node(op_token);
            t3->child = t1;
            t3->child->sibling = t2;
            t1 = t3;
        }
        else
        {
            n = t1;
            break;
        }
    }
}
void tree_gen::power(Node*& n)
{
    Node *t1, *t2, *t3;
    negation(t1);

    while (true)
    {
        if (current_token.id == '^')
        {
            Token op_token = current_token;
            advance_iterator();
            power(t2);
            t3 = new Node(op_token);
            t3->child = t1;
            t3->child->sibling = t2;
            t1 = t3;
        }
        else
        {
            n = t1;
            break;
        }
    }
}
void tree_gen::negation(Node*& n)
{
    if (current_token.id == TypeID::INTEGER || current_token.id == '(')
    {
        unit(n);
    }
}
void tree_gen::unit(Node*& n)
{
    if (current_token.id == TypeID::INTEGER)
    {
        n = new Node(current_token);
        advance_iterator();
    }
    else if (current_token.id == '(')
    {
        advance_iterator();
        expression(n);
        if (current_token.id != ')')
        {
            if (current_token.id == 3)
                throw ParseException("Expected matching ')' to enclose parenthesized expression before End of Expression.", current_token);
            else
                throw ParseException("Expected matching ')' to enclose parenthesized expression. The following token was found instead:", current_token);
        }
        advance_iterator();
    }
}

