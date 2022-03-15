#include "tree_gen.h"

tree_gen::tree_gen() 
{
    token_iterator = -1;
    init_operator_vector();
}

tree_gen::tree_gen(std::vector<Token> tokens)
{
    token_iterator = -1;
    this->tokens = tokens;
    init_operator_vector();
}

bool tree_gen::isOperator(Token t)
{
    return std::binary_search(operator_vector.begin(), operator_vector.end(), t.id);
}

void tree_gen::init_operator_vector()
{
    char ops[] = {'+', '-', '*', '/', TypeID::MOD};
    operator_vector = std::vector<char>(ops, ops + sizeof(ops) / sizeof(char));
    std::sort(operator_vector.begin(), operator_vector.end());
}

bool tree_gen::end_of_tokens()
{
    return token_iterator >= tokens.size();
}

void tree_gen::advance_iterator()
{
    token_iterator++;
    if (!end_of_tokens())
        current_token = tokens[token_iterator];

    while(current_token.id == ')' && !end_of_tokens())
    {
        advance_iterator();
    }
}


bool tree_gen::isUnary(std::list<Token>& token_list, std::list<Token>::iterator& tok_it)
{
    // The left side only needs to be checked if this is not the first token.
    // A first-token + or - CANNOT be a binary operator, so it needs to be unary.
    if (tok_it != token_list.begin())
    {
        // Create a copy of the current position and go backwards by one.
        std::list<Token>::iterator previous = tok_it;
        previous--;

        // If the previous token is not an operator at this point, then the token
        // is not unary.
        if (!isOperator(*previous) && (*previous).id != '(')
        {
            return false;
        }
    }

    // If the checked token is not the last one in the iterator...
    if (tok_it != --token_list.end())
    {
        // Create a copy of the current position and go forwards by one.
        std::list<Token>::iterator next = tok_it;
        next++;
        char next_id = (*next).id;

        // The right-side of the +/- needs to be an integer, identifier, or '(' for
        // it to be unary.
        if (next_id != TypeID::INTEGER && next_id != TypeID::IDENT && next_id != '(')
            return false;  
    }
    else
    {
        return false;
    }

    return true;
}


void tree_gen::create_parse_tree()
{
    advance_iterator();
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

