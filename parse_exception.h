#ifndef PARSE_ERROR_H
#define PARSE_ERROR_H

#include "token.h"
#include <string>

class ParseException : public std::exception
{
public:
    ParseException(std::string msg, Token bad_token)
    {
        this->msg = msg;
        this->bad_token = bad_token;
    }

    std::string message()
    {
        char * error_msg;
        sprintf(error_msg, "Parse Error: %s @ %d:%d (%c)\n", msg.c_str(),
            bad_token.line, bad_token.column, bad_token.id);
        return std::string(error_msg);
    }

private:
    std::string msg;
    Token bad_token;
};

#endif