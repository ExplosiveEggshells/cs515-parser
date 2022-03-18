#include <iostream>
#include <string.h>

#include "lexer_reader.h"
#include "lexer_error.h"

#include "lexer_reader.h"
#include "lexer_error.h"
#include "fsm/lexer_fsm.h"
#include "id_table.h"
#include "tree_gen.h"
#include "encoded_program.h"

#define RECOVERY true

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
                std::cerr << "Attempting to print and execute any completed expressions up to this point." << "\n\n";
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
        cout << "EXPRESSION #" << i << endl;
        cout << "Code Tree:" << endl;
        parse_tree.print_tree_pretty(expression_heads[i], 0);
        cout << endl;
        EncodedProgram prog(expression_heads[i]);
        prog.assemble();
        prog.execute();

        cout << endl;

        parse_tree.delete_tree(expression_heads[i]);
    }

    return 0;
}