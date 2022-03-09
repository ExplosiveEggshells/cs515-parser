CC = g++
CXXFLAGS = -Wall -pedantic

lexer_fsm.o: lexer_reader.o lexer_states.o fsm/lexer_fsm.cpp fsm/lexer_fsm.h
	$(CC) $(CXXFLAGS) -c -o lexer_fsm.o fsm/lexer_fsm.cpp

lexer_states.o: fsm/lexer_states.cpp fsm/lexer_states.h fsm/lexer_state.h
	$(CC) $(CXXFLAGS) -c -o lexer_states.o fsm/lexer_states.cpp

lexer_reader.o: lexer_reader.h lexer_reader.cpp 
	$(CC) $(CXXFLAGS) -c -o lexer_reader.o lexer_reader.cpp

clean:
	rm -r *.o