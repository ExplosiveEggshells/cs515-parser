#include <iostream>
#include <string.h>

#include "lexer_reader.h"
#include "lexer_error.h"

#include "lexer_reader.h"
#include "lexer_error.h"
#include "fsm/lexer_fsm.h"
#include "id_table.h"
#include "tree_gen.h"

#define RECOVERY true

Token current_token;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: ./ncc src_file" << std::endl;
        exit(1);
    }

    LexerReader reader(argv[1]);
    LexerFSM fsm(&reader);

    try
    {
        while (reader)
        {
            fsm.processNextState();
        }
        fsm.addEOF();
    }
    catch (LexicalException &e)
    {
        std::cout << e.message() << std::endl;
    }

    std::vector<Node*> expression_heads;
    tree_gen parse_tree = tree_gen(fsm.tokens);

    while (!parse_tree.finished())
    {
        Node* next_head;
        try
        {
            parse_tree.create_parse_tree(next_head);
            expression_heads.push_back(next_head);
        }
        catch (ParseException &e)
        {
            std::cerr << e.message() << endl;
            if (RECOVERY)
            {
                std::cerr << "Attempting to print and execute any completed expressions up to this point." << endl;
                break;
            }
            else
            {
                exit(-1);
            }
        }

    }

    for (size_t i = 0; i < expression_heads.size(); i++)
    {
        parse_tree.print_tree(expression_heads[i]);
        cout << endl;
    }

    return 0;
}