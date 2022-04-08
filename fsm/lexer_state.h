/**
 * @file lexer_state.h
 * 
 * @brief A lexer state represents one state or node on a
 * finite state machine. This base class should *not* be used alone,
 * and instead is an inheritable template for actual states. 
 * 
 * Each state that inherits from this will do some processing on the
 * input buffer according to a set of rules reacting to the incoming
 * character.
 * 
 * For instance, an identifier state will start when the token is empty
 * and the input buffer gives an alpha character. It will then loop
 * through the indentifier state until a space or punctuation charatcer
 * shows up, which will finish the token. Any unexpected results will
 * generate an error.
 * 
 */

#ifndef LEXER_STATE_H
#define LEXER_STATE_H

#include "lexer_fsm.h"
#include <cstdio>
#include <queue>

class LexerFSM; // Resolves a circular dependency

class LexerState {
public:
    LexerState() {}
    
    virtual void process(LexerFSM* parent_fsm) = 0;
    virtual ~LexerState() {}

private:


};

#endif