/**
 * @file lexer_states.cpp
 * @author Jake Rogers
 * @brief Contains definitions for all of the states declared
 * in lexer_states.h.
 * 
 * The purpose and general process for each state is documented in
 * lexer_states.h!
 */

#include "lexer_states.h"

// Shorthand for the parent fsm's file reader
#define BUFFER parent_state->getReader()

// Shorthand for the most common exit condition- 'x' is either whitespace, punctuation, or EOF.
#define WS_PUNCT_EOF(x) isspace(x)||ispunct(x)||iscntrl(x)||x==EOF
#define WS_EOF(x) isspace(x)||iscntrl(x)||x==EOF

/**
 * @brief Reads in the next 6 characters from the file as
 * a UTF-8 code point and returns a vector containing
 * its encoded bytes.
 * 
 * The bytes need to be read in reverse order to be printed
 * properly!
 */
std::vector<uint8_t> getEncodedUnicode(LexerFSM* parent_state);

/**
 * @brief Tokenizes the current sequence, set the token's
 * ID to id, and assigns the token's value depending on its
 * ID. Then clears the sequence for the next token after
 * adding it to the parent_fsm's token list.
 * 
 * Most states will build their tokens using this function,
 * but some may build their token some other way.
 * 
 * @param parent_state 
 * @param id 
 */
void finishAndClearToken(LexerFSM* parent_state, char id)
{   
    std::string value = "";
    if (id == TypeID::IDENT || id == TypeID::INTEGER 
        || id == TypeID::STRING || id == TypeID::REAL
    )
    {
        value = parent_state->getSequence();
    }

    Token * finished_tok = &parent_state->tokens.back();
    finished_tok->setContent(value, id);
    
    if (finished_tok->id == TypeID::INTEGER)
    {
        finished_tok->i_value = 
            std::stoi(finished_tok->value, nullptr, 10);
    }
    else
    {
        // Not exactly a great sentinel value, but it'll do.
        finished_tok->i_value = INT32_MIN;
    }

    if (finished_tok->id == TypeID::IDENT)
    {
        if (finished_tok->value == "mod")
        {
            finished_tok->id = TypeID::MOD;
        }
    }

    parent_state->clearSequence();
}

/**
 * @brief Throws a LexicalException using a custom
 * error message described by the state. The position
 * used will be the current position of the parent's
 * reader at this moment in time.
 * 
 * @param parent_state 
 * @param bad_char Character that causes the error.
 * @param msg Error message
 */
void throwError(LexerFSM* parent_state, char bad_char, const char * msg)
{
    ReaderPosition pos = BUFFER->getPositionData();
    throw LexicalException(msg, bad_char, pos);
}

void NeutralState::process(LexerFSM* parent_state)
{
    char peek = BUFFER->peekNext();
    bool state_change = true;
    if (isalpha(peek) || peek == '_')
    {
        parent_state->setState(IdentState::getInstance());
        // Don't advance, we want the proper state to handle
        // building the token.
    }
    else if (isdigit(peek))
    {
        parent_state->setState(IntegerState::getInstance());
    }
    else if (ispunct(peek) && (peek != '#' && peek != '"'))
    {
        parent_state->setState(PunctuationState::getInstance());
    }
    else if (peek == '#')
    {
        parent_state->setState(InlineCommentState::getInstance());
        state_change = false;   // We don't want to create a token for comments!
    }
    else if (peek == '"')
    {
        parent_state->setState(StringState::getInstance());
        BUFFER->advance();  // Advance, since we don't want the string state to absorb the " char.
    }
    else
    {
        BUFFER->advance();
        state_change = false;
        // Continue until we find something we can tokenize.
    }

    if (state_change)
    {
        ReaderPosition current_position = BUFFER->getPositionData();
        Token next_token(current_position.line, current_position.column);
        parent_state->tokens.push_back(next_token);
    }
}

LexerState& NeutralState::getInstance()
{
    static NeutralState singleton;
    return singleton;
}

void IdentState::process(LexerFSM* parent_state) {
    char peek = BUFFER->peekNext();
    if (isalnum(peek) || peek == '_')  // Neutral state ensures first character is alphabetical. Subsequent alnums or '_' will continue the sequence.
    {
        // Keep the current state and add next char to the buffer.
        parent_state->appendSequence(BUFFER->advance());
    }
    else if (WS_PUNCT_EOF(peek))
    // Acceptable exits are whitespace, punctuation, control characters, or EOF.
    {
        finishAndClearToken(parent_state, TypeID::IDENT);
        parent_state->setState(NeutralState::getInstance());
    }
    else
    {
        throwError(parent_state, peek, "Encountered illegal character value during IDENTIFIER recognition:");
    }
}

LexerState& IdentState::getInstance()
{
    static IdentState singleton;
    return singleton;
}

void IntegerState::process(LexerFSM* parent_state)
{
    char peek = BUFFER->peekNext();
    if (isdigit(peek))  // Neutral ensures first is a digit, Accept digits to continue sequence or a '.' to transition to a real number.
    {
        parent_state->appendSequence(BUFFER->advance());
    }
    else if (WS_PUNCT_EOF(peek))
    {
        finishAndClearToken(parent_state, TypeID::INTEGER);
        parent_state->setState(NeutralState::getInstance());
    }
    else
    {
        throwError(parent_state, peek, "Encountered illegal character value during INTEGER recognition:");
    }
}

LexerState& IntegerState::getInstance()
{
    static IntegerState singleton;
    return singleton;
}

void PunctuationState::process(LexerFSM* parent_state)
{
    char peek = BUFFER->peekNext();
    std::string seq = parent_state->getSequence();
    if (seq.empty())
    {
        parent_state->appendSequence(BUFFER->advance());
    }
    else if (seq.length() == 1)
    {
        char front_char = parent_state->getSequence().front();
        if (WS_EOF(peek))
        {
            finishAndClearToken(parent_state, front_char);
            parent_state->setState(NeutralState::getInstance());
        }
        else if (front_char == '>')
        {
            if (peek == '=')
            {
                parent_state->appendSequence(BUFFER->advance());
                finishAndClearToken(parent_state, TypeID::GREATER_EQUAL);
                parent_state->setState(NeutralState::getInstance());
            }
            else
            {
                finishAndClearToken(parent_state, front_char);
                parent_state->setState(NeutralState::getInstance());
            }
        }
        else if (front_char == '<')
        {
            if (peek == '-' || peek == '=')
            {
                TypeID id = (peek == '-') ? TypeID::ASSIGN : TypeID::LESS_EQUAL;
                parent_state->appendSequence(BUFFER->advance());
                finishAndClearToken(parent_state, id);
                parent_state->setState(NeutralState::getInstance());
            }
            else if (peek == '<')
            {
                parent_state->appendSequence(BUFFER->advance());
            }
            else
            {
                finishAndClearToken(parent_state, front_char);
                parent_state->setState(NeutralState::getInstance());
            }
        }
        else if (front_char == '~')
        {
            if (peek == '=')
            {
                parent_state->appendSequence(BUFFER->advance());
                finishAndClearToken(parent_state, TypeID::NOT_EQUAL);
                parent_state->setState(NeutralState::getInstance());
            }
            else
            {
                finishAndClearToken(parent_state, front_char);
                parent_state->setState(NeutralState::getInstance());
            }
        }
        else
        {
            finishAndClearToken(parent_state, front_char);
            parent_state->setState(NeutralState::getInstance());
        }
    }
    else
    {
        if (peek != '-')
        {
            finishAndClearToken(parent_state, '<');
            ReaderPosition current_position = BUFFER->getPositionData();
            Token next_token(current_position.line, current_position.column);
            parent_state->tokens.push_back(next_token);
            finishAndClearToken(parent_state, '<');
            parent_state->setState(NeutralState::getInstance());
        }
        else 
        {
            // Empty the current sequence, discard the token,
            // and transition to a multi-line comment state
            parent_state->clearSequence();
            parent_state->tokens.pop_back();
            parent_state->setState(MultiLineCommentState::getInstance());
        }
    }   
}

LexerState& PunctuationState::getInstance()
{
    static PunctuationState singleton;
    return singleton;
}

void StringState::process(LexerFSM* parent_state)
{
    char peek = BUFFER->peekNext();

    if (peek == '\\')
    {
        BUFFER->advance();  // Advance beyond the '\', but discard it, the upcoming state will decide what should be appended.
        parent_state->setState(EscapedCharacterState::getInstance());
    }
    else if (peek == '"')
    {
        BUFFER->advance();  // Advance past the " and discard.
        finishAndClearToken(parent_state, TypeID::STRING);
        parent_state->setState(NeutralState::getInstance());
    }
    else if (peek == EOF)
    {
        throwError(parent_state, peek, "Encountered unexpected EOF during string recognition!");
    }
    else
    {
        parent_state->appendSequence(BUFFER->advance());
    }
}

LexerState& StringState::getInstance()
{
    static StringState singleton;
    return singleton;
}

void EscapedCharacterState::process(LexerFSM* parent_state)
{
    char peek = BUFFER->peekNext();

    if (peek == 'n')
    {
        parent_state->appendSequence('\n');
    }
    else if (peek == 't')
    {
        parent_state->appendSequence('\t');
    }
    else if (peek == 'r')
    {
        parent_state->appendSequence('\r');
    }
    else if (peek == '"')
    {
        parent_state->appendSequence('\"');
    }
    else if (peek == '\\')
    {
        parent_state->appendSequence('\\');
    }
    else if (peek == 'a')
    {
        parent_state->appendSequence('\a');
    }
    else if (peek == 'b')
    {
        parent_state->appendSequence('\b');
    }
    else if (peek == '\n')
    {
        // Purposefully do nothing to avoid the else-error state
    }
    else if (peek == 'u')
    {
        BUFFER->advance();  // Skip past the 'u'
        std::vector<uint8_t> encoded_bytes = getEncodedUnicode(parent_state);
        for (auto it = encoded_bytes.rbegin(); it != encoded_bytes.rend(); it++)
        {
            parent_state->appendByte(*it);
        }
    }
    else {
        throwError(parent_state, peek, "Encountered illegal escape code!");
    }

    // If we made it this far, it's time to go back to the string we were processing!
    if (peek != 'u')
        BUFFER->advance();  // Skip past the escape's code so we're ready to continue.
    parent_state->setState(StringState::getInstance());
}

LexerState& EscapedCharacterState::getInstance()
{
    static EscapedCharacterState singleton;
    return singleton;
}

void InlineCommentState::process(LexerFSM* parent_state)
{
    char peek = BUFFER->peekNext();

    if (peek == '\n' || peek == EOF)
    {   // If the next char is a new line or end of file, the inline is finished.
        parent_state->setState(NeutralState::getInstance());
    }
    else
    {   // Otherwise, keep eating up characters. They do not need to be stored.
        BUFFER->advance();
    }
}

LexerState& InlineCommentState::getInstance()
{
    static InlineCommentState singleton;
    return singleton;
}

void MultiLineCommentState::process(LexerFSM* parent_state)
{
    char peek = BUFFER->peekNext();
    if (peek == EOF)
    {
        throwError(parent_state, TypeID::EOF_CHAR, "Encountered EOF during block comment recognition:");
    }
    if (parent_state->emptySequence())
    {
        if (peek == '-')
        {
            parent_state->appendSequence(peek);
        }
    }
    else if (parent_state->getSequence().length() == 1)
    {
        if (peek == '>')
        {
            parent_state->appendSequence(peek);
        }
        else
        {
            parent_state->clearSequence();
        }
    }
    else if (parent_state->getSequence().length() == 2)
    {
        if (peek == '>')
        {
            BUFFER->advance();
            parent_state->setState(NeutralState::getInstance());
        }
        parent_state->clearSequence();
    }

    BUFFER->advance();
}

LexerState& MultiLineCommentState::getInstance()
{
    static MultiLineCommentState singleton;
    return singleton;
}

std::vector<uint8_t> getEncodedUnicode(LexerFSM* parent_state)
{
    char code_point[6];
    for (int i = 0; i < 6; i++)
    {
        char next = BUFFER->advance();
        
        if (!isxdigit(next))
        {
            throwError(parent_state, next, "Encountered illegal character during six-digit unicode sequence.");
        }

        code_point[i] = next;
    }

    unsigned int unicode_int = 0;   // The value of this unicode character's code point as an integer
    for (int i = 0; i < 6; i++)
    {
        if (isdigit(code_point[i]))
        {
            unicode_int = unicode_int * 16 + (code_point[i] - '0');
            // printf("%c = %d | ", code_point[i], code_point[i] - '0');
        }
        else
        {
            unicode_int = unicode_int * 16 + (tolower(code_point[i]) - 'a' + 10);
            // printf("%c = %d | ", code_point[i], code_point[i] - 'a' + 10);
        }

        //printf("%d ", unicode_int);
    }

    // printf("\n%s as integer: %d\n", code_point, unicode_int);

    std::queue<uint8_t> bit_masks;
    std::queue<uint8_t> fix_masks;
    std::vector<uint8_t> encoded_bytes;
    int unicode_length = 0;

    if (unicode_int < 0x80)
    {
        bit_masks.push(0x7f);      // 0111 1111
        fix_masks.push(0x80);      // 1000 0000
        unicode_length = 1;
    }
    else if (unicode_int < 0x800)
    {
        bit_masks.push(0x3f);      // 0011 1111
        fix_masks.push(0x80);      // 1000 0000

        bit_masks.push(0x1f);      // 0001 1111
        fix_masks.push(0xc0);      // 1100 0000

        unicode_length = 2;
        
    }
    else if (unicode_int < 0x10000)
    {
        bit_masks.push(0x3f);      // 0011 1111
        fix_masks.push(0x80);      // 1000 0000

        bit_masks.push(0x3f);      // 0011 1111
        fix_masks.push(0x80);      // 1000 0000

        bit_masks.push(0x0f);      // 0000 1111
        fix_masks.push(0xe0);      // 1110 0000

        unicode_length = 3;
    }
    else
    {
        throwError(parent_state, ' ', "Unicode code point out of range.");
    }


    for (int i = 0; i < unicode_length; i++)
    {
        uint8_t next_byte = 0;
        next_byte = (unicode_int & bit_masks.front());      // Mask the first bits of the current byte into the encode

        next_byte = (next_byte | fix_masks.front());        // Set the highest order bits of the current byte
        
        encoded_bytes.push_back(next_byte);                      // Add this to the queue of encoded bytes.

        if (bit_masks.size() != 1)                          // Shift out the now irrelevant bits
        {
            unicode_int >>= 6;
        }
        
        bit_masks.pop();
        fix_masks.pop();
    }

    return encoded_bytes;
}