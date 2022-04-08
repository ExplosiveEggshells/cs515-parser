/**
 * @file lexer_reader.h
 * @author Jake Rogers
 * @brief The lexer_reader is essentially file stream object with some extra
 * bells and whistles to make it do exactly what we need and not much more.
 * 
 * The reader is constructed with a file to open in binary mode, and can then advance
 * through that file and optionally peek at the next character without extracting it from the
 * stream. 
 * 
 * Additionally, a ReaderPosition struct is synchronized with the reader to tell us the exact
 * line and column position the reader is currently at within the file, which can then be accessed
 * by the FSM to book-keep the location of errors and start of tokens.
 */
#ifndef LEXER_READER_H
#define LEXER_READER_H

#include <iostream>
#include <fstream>
#include <stdexcept>

struct ReaderPosition {
    uint16_t line;
    uint16_t column;
};

class LexerReader {
public:
    /**
     * @brief Construct a new Lexer Reader object. Will attempt
     * to immediately open a file stream to the path provided.
     * 
     * @param file_path The file path to open a stream to
     */
    LexerReader(const char* file_path);

    /**
     * @brief Closes the file stream when this object goes out of scope
     * or gets destroyed so we don't need to manually do it.
     * 
     */
    ~LexerReader();

    /**
     * @brief An implicit conversion for this object will return true if the stream
     * is good and not at the end of file, allowing it to be used solely within
     * conditionals.
     * 
     * @return True if the stream is good at not at EOF. 
     */
    operator bool() const;

    /**
     * @brief Extract the next character from the stream and return it,
     * advancing the reader position.
     * 
     * @return char of the byte which was read from the file.
     */
    char advance();

    /**
     * @brief Peek at the next-up byte within the file without extracting
     * it or advancing the reader position. Vital for state machine logic.
     * 
     * @return char of the next byte to be extracted.
     */
    char peekNext();

    /**
     * @brief Returns a copy of the position within the file at this
     * moment in time.
     * 
     * @return ReaderPosition 
     */
    ReaderPosition getPositionData();
private:
    std::ifstream src_stream;
    ReaderPosition current_position;
};

#endif