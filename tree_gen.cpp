#include "tree_gen.h"

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

    // Tokens will be printed by i_value, value, then ID, 
    // depending on which one is valid first.
    if (n->token.i_value != INT32_MIN) 
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
    // PRE-order, visit the node, then handle the child, then sibling.

    // Add indent a number of times equal to the depth at this level.
    for (uint i = 0; i < depth; i++)
    {
        printf("  ");
    }

    // Tokens will be printed by i_value, value, then ID, 
    // depending on which one is valid first.
    if (n->token.i_value != INT32_MIN)
        std::cout << n->token.i_value;
    else if (!n->token.value.empty())
        std::cout << n->token.value;
    else
        std::cout << n->token.id;

    std::cout << std::endl;

    // Going to the child increases the depth by one.
    if (n->child != nullptr)
        print_tree_pretty(n->child, depth + 1);

    // Going to the sibling retains the same depth.
    if (n->sibling != nullptr)
        print_tree_pretty(n->sibling, depth);
}

void tree_gen::delete_tree(Node *n)
{
    // Leaves will blow themselves up in POST-order.
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
        // Note, a slight difference to make powers right-associative,
        // power calls itself.
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

    // No while loop is needed in negation, since a sequence
    // of immediate unary operators is not valid in the language.
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
    // Ripple a single integer back up the stack.
    if (current_token.id == TypeID::INTEGER)
    {
        n = new Node(current_token);
        advance_iterator();
    }
    // Otherwise, we need to evaluate a new expression here.
    else if (current_token.id == '(')
    {
        advance_iterator();
        expression(n);

        // After a parentheized expression is handled, we necessarily MUST see
        // a ')' character, otherwise we have an unmatched bracket.
        if (current_token.id != ')')
        {
            // Print out a more descriptive message if we hit EOF.
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
