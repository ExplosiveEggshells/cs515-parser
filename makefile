CC = g++
CXXFLAGS = -Wall -pedantic
LEX_SRC = ./lexer-src/

make: main.o tree_gen.o
	$(CC) -o ncc main.o tree_gen.o

main.o: main.cpp tree_gen.o
	$(CC) -c -o main.o main.cpp

tree_gen.o: tree_gen.cpp tree_gen.h parse_exception.h
	$(CC) -c -o tree_gen.o tree_gen.cpp

clean:
	rm -rf ncc *.o