/**
 * @file lexer_states.h
 * @author Jake Rogers
 * @brief All of the states declared in this class inherit from the
 * lexer_state class, general information about states is found in the
 * docbox for lexer_state.h
 * 
 * This class simply creates specialized states for recognizing patterns
 * for tokens and processing them.
 * 
 * Each class state also has a 'getInstance' function which is used to turn
 * the states into Singletons.
 */
#ifndef LEXER_STATES_H
#define LEXER_STATES_H

#include "../lexer_error.h"
#include "../token.h"
#include "lexer_state.h"
#include "lexer_fsm.h"

/**
 * @brief The neutral state is used at start-up or when a token has just been created. 
 * When process it ran, it will examine the next character coming up and try to determine
 * which type of token is starting, changing the state to one which can process it.
 * 
 * For example, when the neutral state processes, it will peek at the next character.
 * If the next character is '1', it will assume that an integer is coming and will change
 * the state to the IntegerState.
 * 
 */
class NeutralState : LexerState
{
public:
    NeutralState() {}
    void process(LexerFSM* parent_fsm);
    static LexerState& getInstance();
};

/**
 * @brief Used for tokenizing identifiers.
 * When process is ran, the following occurs:
 *  - If the next char is '_' or alpha numeric, append it to the sequence.
 *  - If the next char is whitespace or punctuation (not '_'), turn the sequence
 * into a token and go back to neutral.
 *  - Otherwise, throw an error.
 * 
 */
class IdentState : LexerState 
{
public:
    IdentState() {}
    void process(LexerFSM* parent_fsm);
    static LexerState& getInstance();
};

/**
 * @brief Used for tokenizing integers.
 * When process is ran, the following occurs:
 *  - If the next char is numeric, append it to the sequence
 *  - If the next char is punctuation or EOF, turn the sequence
 * into a token and return to neutral.
 *  - Otherwise, throw an error.
 * 
 */
class IntegerState : LexerState
{
public:
    IntegerState() {}
    void process(LexerFSM* parent_fsm);
    static LexerState& getInstance();
};

/**
 * @brief Used for tokenizing single or multi-character
 * punctuation-operators.
 * Since the punctation state can process single or multi-character
 * tokens AS WELL as multi-line comment starters, it becomes quite complex.
 * Each process heavily depends on the current state of the sequence.
 * 
 * - If the sequence is empty, take in any punctuation and add it to
 * the sequence.
 *      - If there is one character in the sequence already, and it matches
 *      the start of a multi-character operator, check if the next character
 *      creates a multi-character operator with the first.
 *          - If it does, create that multi-character token and return to
 *          neutral UNLESS the next character would turn it into a block
 *          comment.
 *              - If the third character would make a block comment, clear
 *              the sequence and change state to MultiLineCommentState
 *          - If it doesn't, turn those two characters into seperate single-
 *          character tokens and return to neutral.
 */
class PunctuationState : LexerState
{
public:
    PunctuationState() {}
    void process(LexerFSM* parent_fsm);
    static LexerState& getInstance();
};

/**
 * @brief Used for tokenizing strings.
 * When process is ran, the following occurs:
 *  - If the next char is \', go to the escaped character
 *  state.
 *      - If the char produced from the escaped character state
 *      is \", tokenize the current sequence and go back to
 *      neutral state.
 *  - If the next char is EOF, throw an error.
 *  - Otherwise, add the next char to the sequeence.
 *
 */
class StringState : LexerState
{
public:
    StringState() {}
    void process(LexerFSM* parent_fsm);
    static LexerState& getInstance();
};

/**
 * @brief During a string, the '\' character indicates
 * that a special character is coming up. The combination created
 * from this escaped character needs to be added to the sequence
 * and then the state will return back to the STRING.
 * 
 */
class EscapedCharacterState : LexerState
{
public:
    EscapedCharacterState() {}
    void process(LexerFSM* parent_fsm);
    static LexerState& getInstance();
};

/**
 * @brief Will disregard any contents of the file until the next
 * new line or EOF.
 * 
 */
class InlineCommentState : LexerState
{
public:
    InlineCommentState() {}
    void process(LexerFSM* parent_fsm);
    static LexerState& getInstance();
};

/**
 * @brief Will disregard the contents of the file until the sequence
 * '->>' is seen. Will throw an error if EOF occurs.
 * 
 */
class MultiLineCommentState : LexerState
{
public:
    MultiLineCommentState() {}
    void process(LexerFSM* parent_fsm);
    static LexerState& getInstance();
};

#endif