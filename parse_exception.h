#ifndef PARSE_ERROR_H
#define PARSE_ERROR_H

#include "token.h"
#include <string>
#include <cstring>
#include <cstdio>   // sprintf

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
        char error_msg[1024];
        sprintf(error_msg, "Parse Error: %s | ID = %d @ %d:%d '%c'", msg.c_str(),
            bad_token.id, bad_token.line, bad_token.column, bad_token.id);
        
        if (!bad_token.value.empty())
        {
            char value_extension[128];
            sprintf(value_extension, " -> (%s)", bad_token.value.c_str());
            strcat(error_msg, value_extension);
        }
        return std::string(error_msg);
    }

private:
    std::string msg;
    Token bad_token;
};

#endif