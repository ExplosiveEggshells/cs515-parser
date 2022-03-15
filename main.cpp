#include <iostream>
#include "tree_gen.h"
#include <string.h>

Token current_token;


int main(int argc, char** argv)
{
    std::vector<Token> toks;
    // toks.push_back(Token("1", TypeID::INTEGER));
    // toks.push_back(Token("+"));
    // toks.push_back(Token("2", TypeID::INTEGER));
    // toks.push_back(Token("+"));
    // toks.push_back(Token("3", TypeID::INTEGER));
    // toks.push_back(Token("-"));
    // toks.push_back(Token("4", TypeID::INTEGER));
    // toks.push_back(Token("", TypeID::EOF_CHAR));

    // toks.push_back(Token("1", TypeID::INTEGER));
    // toks.push_back(Token("+"));
    // toks.push_back(Token("2", TypeID::INTEGER));
    // toks.push_back(Token("*"));
    // toks.push_back(Token("3", TypeID::INTEGER));
    // toks.push_back(Token("-"));
    // toks.push_back(Token("4", TypeID::INTEGER));
    // toks.push_back(Token("", TypeID::EOF_CHAR));

    // toks.push_back(Token("1", TypeID::INTEGER));
    // toks.push_back(Token("+"));
    // toks.push_back(Token("2", TypeID::INTEGER));
    // toks.push_back(Token("^"));
    // toks.push_back(Token("3", TypeID::INTEGER));
    // toks.push_back(Token("", TypeID::EOF_CHAR));

    toks.push_back(Token("("));
    toks.push_back(Token("("));
    toks.push_back(Token("("));
    toks.push_back(Token("1", TypeID::INTEGER));
    toks.push_back(Token(")"));
    toks.push_back(Token(")"));
    toks.push_back(Token("20", TypeID::INTEGER));

    tree_gen parse_tree = tree_gen(toks);
    try
    {
        parse_tree.create_parse_tree();
    } catch (ParseException e)
    {
        std::cerr << "";    // Not really sure why, but removing this causes a segmentation fault.
        std::cerr << e.message() << endl;
        exit(-1);
    }

    return 0;
}