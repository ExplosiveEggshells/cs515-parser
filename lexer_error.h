/**
 * @file lexer_error.h
 * @author Jake Rogers
 * @brief This header-only class lays out an implementation for 
 * generic exceptions which can occur during the lexer's operation, allowing any
 * part of the process to throw a detailed error message at will without having
 * to constantly return error codes with nominal data.
 * 
 * LexicalExceptions contain a message (which is set by the caller who throws it) as
 * well as the current position of the error, which is usually passed in by the state
 * when it throws the exception. This allows for any state in the FSM to throw an exception
 * with a very specific error message and an exact line and column number. This exception will
 * then flow the up calling stack and can be caught and processed anywhere along the line for maximum
 * versatility.
 * 
 * The exception optionally allows the thrower to specify the exact character which caused the error,
 * which is very useful for debugging.
 * 
 * This exception-driven approach is hopefully a valid alternative to passing around
 * error codes with every step of the lexer's process.
 */

#ifndef LEXER_ERROR_H
#define LEXER_ERROR_H

#include "lexer_reader.h"   // For the ReaderPosition struct
#include <string>
#include <sstream>

class LexicalException : public std::exception
{
public:
    /**
     * @brief Construct a new Lexical Exception object
     * 
     * @param msg   Error message
     * @param where Line and column number where it occured.
     */
    LexicalException(std::string msg, ReaderPosition where)
    {
        this->msg = msg;
        this->where = where;
        bad_char = -99;     // The bad character is just set to a sentinel value, since 0 or -1 won't work here.
    }

    /**
     * @brief Construct a new Lexical Exception object with the bad character specified.
     * 
     * @param msg       Error message
     * @param bad_char  The exact character which caused the error
     * @param where     Line and column number where it occured.
     */
    LexicalException(std::string msg, int bad_char, ReaderPosition where)
    {
        this->msg = msg;
        this->where = where;
        this->bad_char = bad_char;
    }

    ~LexicalException() {}

    /**
     * @brief Returns a printable string detailing what went wrong
     * and where, using all of the elements passed in through the
     * constructor
     * 
     * @return std::string 
     */
    std::string message() {
        std::stringstream ss;
        if (bad_char != -99)
        {
            ss << "[" << where.line << ":" << where.column << "] : " 
            << msg << " " << (char)bad_char << " (" << bad_char << ")";
        }
        else
        {
            ss << "[" << where.line << ":" << where.column << "] : " << msg;
        }

        return ss.str();
    }

private:
    std::string msg;
    ReaderPosition where;

    int bad_char;
};

#endif