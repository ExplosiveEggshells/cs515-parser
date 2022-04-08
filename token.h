/**
 * @file token.h
 * @author Jake Rogers
 * @brief A token represents a valid, identified 'word' extracted from the source file.
 * These tokens will either be a rather simple one identified from a single character, or
 * a complex one based off of a continuous sequence of related characters.
 * 
 * Each token has an ID which identifies its type, a position of where it was found in the
 * source file, and potentially a 'value'.
 * 
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <cstdint>
#include <string>

/**
 * For most simple tokens, their ID is equivalent to the
 * ASCII value of the character which was used to tokenize
 * them.
 * 
 * For more complicated tokens which was build from a sequence 
 * of characters, their ID is equal to some unused characted in the
 * ASCII space so that the ID can still fit within one byte.
 * 
 */
enum TypeID {
    GREATER_EQUAL = 48,
    LESS_EQUAL = 49,
    NOT_EQUAL = 50,
    ASSIGN = 51,
    IDENT = 52,
    INTEGER = 53,
    REAL = 54,
    STRING = 55,
    EOF_CHAR = 56,
    MOD = 65,
    UPLUS = 66,
    NEGATE = 67
};

struct Token {

    Token()
    {
        this->line = 0;
        this->column = 0;

        this->id = -1;
        this->value = "";
    }

    Token(uint16_t line, uint16_t column)
    {
        Token();
        this->line = line;
        this->column = column;

        this->id = -1;
        this->value = "";
    }

    Token(std::string one_char_value)
    {
        this->line = 0;
        this->column = 0;
        this->value = one_char_value;
        this->id = one_char_value.front();
    }

    Token(std::string value, char id)
    {
        this->line = 0;
        this->column = 0;

        this->value = value;
        this->id = id;
    }
    
    void setContent(std::string value, char id)
    {
        this->value = value;
        this->id = id;
    }


    char id;

    // Save a bit of mem by using smaller, unsigned ints
    // It is fair to assume line and col counts will be 
    // < 65k... at least I hope so.
    uint16_t  line;
    uint16_t column;

    std::string value;
    int i_value;
};



#endif