/**
 * @file lexer_fsm.h
 * @author Jake Rogers
 * @brief The lexer_fsm, alongside the lexer_reader, make up the
 * driving logic for most of the lexer implemented in this program.
 * 
 * Using a design pattern for Finite State Machines, this object keeps track of 
 * its current state (a LexerState object). Each time it is told to process, it will
 * run some kind of logic associated within the current state. 
 * 
 * These states will make some kind of determination of what to do based on the upcoming
 * byte within the LexerReader (using the peekNext function).
 * 
 * 
 * The NeutralState is the initial state and is always returned to after finishing a string. It
 * will then read in the next byte until some recognizable token pattern begins.
 * 
 * The FSM keeps track of the currently building sequence- a string of characters read since the
 * last time a token was produced. This effectively gives each state a memory of the token being
 * built so that multi-character tokens can be reliably processed.
 * 
 * For example, the StringState will keep reading in characters and attaching them to the
 * currently built sequence of characters for the current string. Once it encounters a ",
 * it will change the state back to the NeutralState and tokenize the string it was building.
 * 
 * Though I am familiar of using FSMs from game development, I used the pattern provided by
 * https://www.aleksandrhovhannisyan.com/blog/finite-state-machine-fsm-tutorial-implementing-an-fsm-in-c/
 * to adapt it to C++ for this assignment.
 */
#ifndef LEXER_FSM_H
#define LEXER_FSM_H

#include "lexer_state.h"        // The FSM will keep track of its current state and process it.
#include "../token.h"           // The FSM builds and keeps a vector of tokens.
#include "../lexer_reader.h"    // The FSM needs an associated reader to pass to its state.

#include <string>
#include <vector>

class LexerState;   // Resolves a circular dependency

class LexerFSM {
public:
    /**
     * @brief Create a new FSM with an attached reader and
     * sets the current state to the NeutralState.
     * 
     * @param reader The reader which works in tandem with this
     * FSM.
     */
    LexerFSM(LexerReader* reader);

    /**
     * @brief A destructor. Currently empty but might
     * be useful in the future? 
     */
    ~LexerFSM();

    /**
     * @return LexerState* A pointer to the current state of the FSM.
     */
    LexerState* getCurrentState() const;

    /**
     * @brief Tells the current state to process the next character within
     * the reader. The actual action implemented will depend on the current
     * state.
     * 
     * Note that some states will process multiple characters from the reader
     * at a time, skipping ahead whenever logical (like when processing unicode).
     */
    void processNextState();

    /**
     * @brief Changes the current state to the SINGLETON instance of
     * another state, which is why it is passed as an address.
     * 
     * The NeutralState will change the state to the appropriate one
     * when a pattern for any given pattern is getting recognized.
     * 
     * The each state will also change the state back to the neutral one
     * once a token has been created, allowing us to start recognizing 
     * the next one.
     * 
     * @param next_state 
     */
    void setState(LexerState& next_state);

    // Self explaintory inline functions
    inline LexerReader* getReader() {return reader;}
    inline std::string getSequence() {return sequence;}
    inline void appendSequence(const char c) {sequence.push_back(c);}
    inline void appendByte(int byte) {sequence += byte;}
    inline void clearSequence() {sequence.clear();}
    inline bool emptySequence() {return sequence.empty();}

    /**
     * @brief Adds an EOF token to the list of tokens, which
     * unfortunately needs to be done manually with this approach
     */
    void addEOF();

    std::vector<Token> tokens;  // The list of tokens recognized by the FSM at the current moment.

private:
    LexerState* current_state;
    LexerReader* reader;

    /**
     * The sequence is a string which represents all of the characters read in from the file
     * since the last token was finished.
     * 
     * This allows for states (which have access to it through the various inline functions above
     * ) to do some checks based on what has been added to the currently building token so far, allowing
     * for multi-character symbols to be made.
    */
    std::string sequence;
};

#endif