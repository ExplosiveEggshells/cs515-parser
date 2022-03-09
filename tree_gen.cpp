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
}


void tree_gen::insert_bracket(std::list<Token> list_tokens, std::list<Token>::iterator it, bool forward, bool unary)
{
    uint16_t bracket_count = 0; // Number of bracket pairs we need to pass.
    if (!forward)   // Moving left.
    {   
        if (!unary)
        {
            do
            {
                it--;   // Move left once

                if ((*it).id == ')')    // If we hit the start of a pre-existing block, add one to the count
                {
                    bracket_count++;
                }
                else if ((*it).id == '(')   // If we hit the end of a block, substract one from the count
                {
                    if (bracket_count == 0) // If we hit a '(', then there must be an unmatched bracket. End in error.
                    {
                        throw ParseException("Encountered unmatched bracket!", *it);
                        return;
                    }
                    bracket_count--;
                }

            } while (bracket_count != 0);   // Once there are no pairs to skip, we can set a bracket down here.
        }
        Token tok = Token("(");
        tok.is_branching_bracket = true;
        list_tokens.insert(it, tok);
    }

    // If we are going rightwards, the logic is nearly identical with the brackets flipped. We just need to
    // Make sure we iterator forwards once more before insertion since insertion adds the object BEFORE the
    // current iterator.
    else
    {
        do
        {
            it++;
            if ((*it).id == '(')
            {
                bracket_count++;
            }
            else if ((*it).id == ')')
            {
                if (bracket_count == 0)
                {
                    throw ParseException("Encountered unmatched bracket during expression formatting.", *it);
                }
                bracket_count--;
            }
        } while (bracket_count != 0);
        it++;

        Token tok = Token(")");
        tok.is_branching_bracket = true;
        list_tokens.insert(it, tok);
    }
}


bool tree_gen::isUnary(std::list<Token>& token_list, std::list<Token>::iterator& tok_it)
{
    cout << "test left side " << endl;
    if (tok_it == token_list.begin())
    {
        cout << "Truth brother" << endl;
    }
    // The left side only needs to be checked if this is not the first token.
    // A first-token + or - CANNOT be a binary operator, so it needs to be unary.
    if (tok_it != token_list.begin())
    {
        cout << "Creating previous iterator" << endl;
        // Create a copy of the current position and go backwards by one.
        std::list<Token>::iterator previous = tok_it;
        previous--;

        cout << "See if previous is op" << endl;
        // If the previous token is not an operator at this point, then the token
        // is not unary.
        if (!isOperator(*previous) && (*previous).id != '(')
        {
            return false;
        }
    }

    cout << "test right side " << endl;
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

void tree_gen::format_expr(std::vector<Token>& v_tokens)
{
    // Orders are separted by \n characters, then the list is terminated
    // by \0.
    // Items on the same order are grouped simultaneously 
    char order_of_operations[] = {
        '^', '\n',
        '*','/', TypeID::MOD, '\n',
        '+','-','\n',
        '\0'
    };
    int ord_it = 0;

    // Convert this vector of tokens to a list for better insertion efficiency- we'll
    // be doing a lot of that!
    std::list<Token> list_tokens;
    list_tokens.insert(list_tokens.begin(), v_tokens.begin(), v_tokens.end());

    // Unary pass:
    std::vector<char> unary_chars;
    unary_chars.push_back('+');
    unary_chars.push_back('-');
    for (auto it = list_tokens.begin(); it != list_tokens.end(); it++)
    {
        if (std::find(unary_chars.begin(), unary_chars.end(), (*it).id) != unary_chars.end())
        {
            cout << "testing " << (*it).id << endl;
            if (isUnary(list_tokens, it))
            {
                insert_bracket(list_tokens, it, false, true);
                insert_bracket(list_tokens, it, true, true);
                std::cout << "Found unary for " << (*it).id << endl;
                (*it).is_unary = true;
            }
        }
    }

    cout << "Unary pass done" << endl;
    for (auto it = list_tokens.begin(); it != list_tokens.end(); it++)
    {
        std::cout << (*it).value;
    }
    cout << endl;

    // Main loop of the function. Continue until we reach that '\0' terminator in the
    // order_of_operations
    while (order_of_operations[ord_it] != '\0')
    {
        // Each loop builds a vector of the operators in this order.
        std::vector<char> current_order;
        while(order_of_operations[ord_it] != '\n')  // Add ops to the vector until we reach the end of this order ('\n')
        {
            current_order.push_back(order_of_operations[ord_it]);
            ord_it++;
        }
        ord_it++;   // Prep the next order by advancing past '\n'

        // Okay, now parse through the list of tokens, looking for any instance of the operators
        // in this order.
        for (auto it = list_tokens.begin(); it != list_tokens.end(); it++)
        {
            // If we do find an operator in this order, insert brackets to the left and right of it.
            if (std::find(current_order.begin(), current_order.end(), (*it).id) != current_order.end())
            {
                // If we've already determined this op to be unary, ignore it.
                if ((*it).is_unary)
                    continue;
                insert_bracket(list_tokens, it, false, false);
                insert_bracket(list_tokens, it, true, false);

                // Print out the list for debugging purposes.
                for (auto it = list_tokens.begin(); it != list_tokens.end(); it++)
                {
                    std::cout << (*it).value;
                }
                cout << endl;
            }
        }
    }

    tokens.erase(tokens.begin(), tokens.end());
    tokens.insert(tokens.begin(), list_tokens.begin(), list_tokens.end());    
}


void tree_gen::create_parse_tree()
{
    format_expr(tokens);
    advance_iterator();
    recursive_parse(head, true);
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

void tree_gen::variable(Node*& n)
{
    if (current_token.id == TypeID::INTEGER)
    {
        n = new Node(current_token);
        advance_iterator();
    }
}

void tree_gen::terminal(Node*& n)
{
    if (current_token.id == TypeID::INTEGER)
    {
        variable(n);
    }
}

Node* tree_gen::expression()
{
    Node *t1, *t2, *t3;
    terminal(t1);
    
    // In the special case of there only being one singlular terminal, don't
    // enter the loop and just return a pointer to it.
    if (end_of_tokens())
    {
        return t1;
    }

    while(true)
    {
        if (end_of_tokens())
        {
            break;
        }

        if (current_token.id == '+' || current_token.id == '-')
        {
            t3 = new Node(current_token);
            advance_iterator();
            terminal(t2);

            t3->child = t1;
            t3->child->sibling = t2;
            t1 = t3;
        }
        else
        {
            std::cerr << "Encountered Illegal token during parsing!" << std::endl;
            break;
        }
    }

    return t3;
}

void tree_gen::recursive_parse(Node*& n, bool branching)
{
    if (!branching)
    {
        bool should_branch = current_token.is_branching_bracket;
        advance_iterator();
        recursive_parse(n, should_branch);
        return;
    }
    cout << "a" << endl;
    if (end_of_tokens())
    {
        return;
    }

    cout << "b" << endl;

    if (token_iterator == 0 && current_token.id == '(')
    {
        cout << "yoink!" << endl;
        advance_iterator();
    }

    cout << "c" << endl;

    if (current_token.is_unary)
    {
        cout << "Parse found unary " << current_token.id << endl;
        Node *top, *bottom;
        if (!isOperator(current_token))  // Yikes, how did we get a non-operator character as a unary? Time to raise the alarm.
        {
            throw ParseException("Encountered non-operator unary token during recursive parsing.", current_token);
            return;
        }

        top = new Node(current_token);
        advance_iterator();

        if (current_token.id == '(')
        {
            bool should_branch = current_token.is_branching_bracket;
            advance_iterator();
            recursive_parse(bottom, should_branch);
        }
        else if (current_token.id == TypeID::INTEGER || current_token.id == TypeID::IDENT)
        {
            terminal(bottom);
        }
        top->child = bottom;
        n = top;
    }
    else {
        Node *t1, *t2, *t3;

        cout << "doing t1" << endl;

        // t1: left-side
        if (current_token.id == TypeID::INTEGER)
            terminal(t1);
        else if (current_token.id == '(')
        {
            bool should_branch = current_token.is_branching_bracket;
            advance_iterator();
            recursive_parse(t1, should_branch);
            int x = 1;
            x = +x;
        }
        else
            throw ParseException("Encountered illegal character on left side of recursive parsing. Expected terminal or '('.", current_token);

        cout << "doing t3" << endl;

        // t3: operator
        if (!isOperator(current_token))
            throw ParseException("Encountered Illegal character for operator of recursive parsing.", current_token);
        else
        {
            t3 = new Node(current_token);
            advance_iterator();
        }

        cout << "doing t2" << endl;

        if (current_token.id == TypeID::INTEGER)
            terminal(t2);
        else if (current_token.id == '(')
        {
            bool should_branch = current_token.is_branching_bracket;
            advance_iterator();
            recursive_parse(t2, should_branch);
        }
        else
            throw ParseException("Encountered illegal character on right side of recursive parsing. Expected terminal or '('.", current_token);


        t3->child = t1;
        t3->child->sibling = t2;
        n = t3;
        cout << "z" << endl;
    }

    while(current_token.id == ')' && !end_of_tokens())
    {
        advance_iterator();
    }
}