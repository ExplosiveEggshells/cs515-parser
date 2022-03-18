CC = g++
CXXFLAGS = -Wall -pedantic
LEX_SRC = ./lexer-src/

make: main.o \
	lexer_reader.o lexer_fsm.o lexer_states.o \
	tree_gen.o encoded_program.o
	$(CC) $(CXXFLAGS) -o ncc main.o tree_gen.o encoded_program.o lexer_reader.o lexer_fsm.o lexer_states.o 

main.o: main.cpp \
	id_table.h lexer_states.o lexer_reader.o lexer_fsm.o lexer_error.h \
	tree_gen.o encoded_program.o
	$(CC) $(CXXFLAGS) -c -o main.o main.cpp

# PARSER TARGETS

tree_gen.o: tree_gen.cpp tree_gen.h parse_exception.h node.h
	$(CC) $(CXXFLAGS) -c -o tree_gen.o tree_gen.cpp

encoded_program.o: encoded_program.cpp encoded_program.h node.h
	$(CC) $(CXXFLAGS) -c -o encoded_program.o encoded_program.cpp

# LEXER TARGETS

lexer_fsm.o: lexer_reader.o lexer_states.o fsm/lexer_fsm.cpp fsm/lexer_fsm.h
	$(CC) $(CXXFLAGS) -c -o lexer_fsm.o fsm/lexer_fsm.cpp

lexer_states.o: fsm/lexer_states.cpp fsm/lexer_states.h fsm/lexer_state.h
	$(CC) $(CXXFLAGS) -c -o lexer_states.o fsm/lexer_states.cpp

lexer_reader.o: lexer_reader.h lexer_reader.cpp 
	$(CC) $(CXXFLAGS) -c -o lexer_reader.o lexer_reader.cpp
clean:
	rm -rf ncc *.o