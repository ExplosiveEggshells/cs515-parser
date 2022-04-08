/**
 * @file main.cpp
 * @author Jake Rogers (z1826513)
 * @brief A driver program to demonstrate the parser
 * and lexer working together to assemble a machine-code
 * program.
 * 
 */

#include <iostream>
#include <string.h>

// Stuff for lexer
#include "fsm/lexer_fsm.h"
#include "lexer_reader.h"
#include "lexer_error.h"
#include "lexer_reader.h"
#include "lexer_error.h"

#include "id_table.h"
#include "tree_gen.h"
#include "encoded_program.h"

// Somewhat deceptively named. If true, the program will try to execute
// any successfully parsed expressions should the most recent one
// is invaliud. 
#define RECOVERY true   

int main(int argc, char **argv)
{
    // Usage
    if (argc != 2)
    {
        std::cerr << "Usage: ./ncc src_file" << std::endl;
        exit(1);
    }

    // Initialize the lexer
    LexerReader reader(argv[1]);
    LexerFSM fsm(&reader);

    // Read in tokens from the file
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

    // Vector of heads to arithmetic expressions.
    std::vector<Node*> expression_heads;

    // Set up the tree generator with our token vector
    tree_gen parse_tree = tree_gen(fsm.tokens);

    // While there are still more expressions to create trees from...
    while (!parse_tree.finished())
    {
        // Make a new head and generate a tree from it.
        Node* next_head;
        try
        {
            parse_tree.create_parse_tree(next_head);
            expression_heads.push_back(next_head);
        }
        catch (ParseException &e)
        {   // If an error occurs, we'll either stop and execute what we have, or just explode.
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

    /**
     * For each tree...
     *  - Print out it's pretty representation
     *  - Create an EncodedProgram from the head
     *  - Assemble the program
     *  - Execute the program
     *  - Delete the tree
     */
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