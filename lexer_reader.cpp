#include "lexer_reader.h"

LexerReader::LexerReader(const char* file_path) 
{
    src_stream.open(file_path, std::ios::binary);
    if (!src_stream)    // Check if our file opened successfully
    {
        std::string error_path(file_path);
        throw std::runtime_error("Lexer failed to open source file at " + error_path + "!\n");
        return;
    }

    current_position = {1, 1};  // Initialize the line and column to 1 & 1.
}

LexerReader::operator bool() const 
{
    return (!src_stream.eof() && src_stream.is_open());
}

LexerReader::~LexerReader() 
{
    if (src_stream.is_open())   // Close the source file if it is open.
        src_stream.close();
}

char LexerReader::advance() 
{
    char next = src_stream.get();
    
    if (next == '\n')      // Advance the line counter and reset the column number
    {
        current_position.line++;
        current_position.column = 1;
    } else 
    {
        current_position.column++;
    }

    return next;
}

char LexerReader::peekNext()
{
    return src_stream.peek();
}

ReaderPosition LexerReader::getPositionData() 
{
    return current_position;
}