#include "tree_gen.h"

tree_gen::tree_gen()
{
    token_iterator = -1;
    tokens.push_back(Token("Parser EOX", 3)); // Add a ETX to signify an End of Expression. There should be no attempt to advance beyond this token.
}

tree_gen::tree_gen(std::vector<Token> tokens)
{
    token_iterator = -1;
    this->tokens = tokens;
    tokens.push_back(Token("Parser EOX", 3)); // Add a ETX to signify an End of Expression. There should be no attempt to advance beyond this token.
}

void tree_gen::advance_iterator()
{
    if (current_token.id == 3)
    {
        throw ParseException("Attempted to advance beyond End of Expression during code tree generation.", current_token);
    }
    token_iterator++;
    current_token = tokens[token_iterator];
}

bool tree_gen::finished()
{
    if (token_iterator == -1)
        return false;

    if (current_token.id != 3 && current_token.id != TypeID::EOF_CHAR)
        return false;

    return true;
}

void tree_gen::create_parse_tree(Node *&head)
{
    if (token_iterator == -1)
        advance_iterator();
    expression(head);
}

void tree_gen::print_tree(Node *n)
{
    if (n->child != nullptr)
    {
        print_tree(n->child);
    }

    if (n->token.i_value != INT32_MIN) // Print the token's ID, unless it has a non-empty value.
        std::cout << n->token.i_value;
    else if (!n->token.value.empty())
        std::cout << n->token.value;
    else
        std::cout << n->token.id;

    std::cout << ", ";
    if (n->sibling != nullptr)
    {
        print_tree(n->sibling);
    }
}

void tree_gen::print_tree_pretty(Node* n, uint depth)
{
    for (uint i = 0; i < depth; i++)
    {
        printf("  ");
    }

    if (n->token.i_value != INT32_MIN) // Print the token's ID, unless it has a non-empty value.
        std::cout << n->token.i_value;
    else if (!n->token.value.empty())
        std::cout << n->token.value;
    else
        std::cout << n->token.id;

    std::cout << std::endl;

    if (n->child != nullptr)
    {
        print_tree_pretty(n->child, depth + 1);
    }

    if (n->sibling != nullptr)
    {
        print_tree_pretty(n->sibling, depth);
    }
}

void tree_gen::delete_tree(Node *n)
{
    if (n->child != nullptr)
    {
        delete_tree(n->child);
    }

    if (n->sibling != nullptr)
    {
        delete_tree(n->sibling);
    }

    delete(n);
}

void tree_gen::expression(Node *&n)
{
    Node *t1, *t2, *t3;
    term(t1);

    while (true)
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
void tree_gen::term(Node *&n)
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
void tree_gen::power(Node *&n)
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
void tree_gen::negation(Node *&n)
{
    Node *t1, *t2;

    if (current_token.id == '-')
    {
        Token op_token = current_token;
        op_token.id = TypeID::NEGATE;
        op_token.value = "u-";
        advance_iterator();
        unit(t2);

        t1 = new Node(op_token);
        t1->child = t2;
        n = t1;
    }
    else if (current_token.id == '+')
    {
        Token op_token = current_token;
        op_token.id = TypeID::UPLUS;
        op_token.value = "u+";
        advance_iterator();
        unit(t2);

        t1 = new Node(op_token);
        t1->child = t2;
        n = t1;
    }
    else
    {
        unit(n);
    }
}
void tree_gen::unit(Node *&n)
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
    else
    {
        if (current_token.id == ')')
            throw ParseException("Unmatched bracket detected within expression.", current_token);
        else
            throw ParseException("Invalid symbol detected within expression.", current_token);
    }
}
