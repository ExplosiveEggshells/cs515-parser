#include "lexer_fsm.h"
#include "lexer_states.h"

LexerFSM::LexerFSM(LexerReader* reader)
{
    this->reader = reader;
    setState(NeutralState::getInstance());  // The neutral state is used to identify the next type of token coming up, so we want to start there.
    sequence = "";  // Empty out the sequence.
}

LexerFSM::~LexerFSM()
{
}

void LexerFSM::processNextState()
{
    current_state->process(this);
}

void LexerFSM::addEOF()
{
    ReaderPosition current_position = reader->getPositionData();
    Token eof_token(current_position.line, current_position.column);
    eof_token.setContent("", TypeID::EOF_CHAR);
    tokens.push_back(eof_token);
}

void LexerFSM::setState(LexerState& next_state)
{
    current_state = &next_state;
}
