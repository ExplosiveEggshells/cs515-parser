#include <iostream>
#include <string.h>

#include "lexer_reader.h"
#include "lexer_error.h"

#include "lexer_reader.h"
#include "lexer_error.h"
#include "fsm/lexer_fsm.h"
#include "id_table.h"
#include "tree_gen.h"

Token current_token;


int main(int argc, char** argv)
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
        while(reader)
        {
            fsm.processNextState();
        }
        fsm.addEOF();
    }
    catch (LexicalException& e)
    {
        std::cout << e.message() << std::endl;
    }

    // for (auto it = fsm.tokens.begin(); it != fsm.tokens.end(); ++it)
    // {
    //     Token tok = *(it);
    //     printf("TOKEN_%-13s @ [%3d: %3d] ID = %3d", 
    //     token_strings[(int)tok.id].c_str(),
    //     tok.line,
    //     tok.column,
    //     tok.id
    //     );
    //     if (!tok.value.empty())     // Print out the value if applicable.
    //         printf(" -> %s", tok.value.c_str());
    //     printf("\n");
        
    // }
    
    std::vector<Token> toks;
    toks.push_back(Token("1", TypeID::INTEGER));
    toks.push_back(Token("+"));
    toks.push_back(Token("2", TypeID::INTEGER));
    toks.push_back(Token("", 56));

    tree_gen parse_tree = tree_gen(fsm.tokens);
    try
    {
        parse_tree.create_parse_tree();
    } catch (ParseException& e)
    {
        std::cerr << e.message() << endl;
        exit(-1);
    }

    return 0;
}