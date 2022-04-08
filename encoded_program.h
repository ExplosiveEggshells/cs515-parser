/**
 * @file encoded_program.h
 * @author Jake Rogers (z1826513)
 * 
 * An EncodedProgram is an object for assembling and executing an encoded arithmetic
 * expression from a given parse tree generated from tree_gen. 
 * 
 */

#ifndef ENCODED_PROGRAM_H
#define ENCODED_PROGRAM_H

#include <iostream>
#include <sys/mman.h>

#include "node.h"
#include "parse_exception.h"

#define VERBOSE false   // Prints out heaps of debugging info, pretty ugly
#define ENCODE program[program_offset++]=   // Shorthand for adding one byte to the program and advancing the pointer.

class EncodedProgram
{
public:
    // Create a new program for the arithmetic expression described in
    // parse_tree_head
    EncodedProgram(Node* parse_tree_head);

    // Starts the traversal of the parse tree to encode the expression, then
    // an instruction to return the EAX register.
    void assemble();

    // Run the program and print the output and number of bytes taken to encode
    // it. Also releases the memory for the program once finished.
    void execute();
private:
    // Use mmap to space from memory for our program.
    void initialize();

    // Traversing the parse tree in post-order, create an appropriate encoding
    // per each node visited. More info in the definition.
    void traverse(Node* n);

    // Helper function to add four bytes representing 'value' to the program
    // in little-endian order.
    void encode_imm32(int32_t value);

    // Returns the mod_rm byte for the two given register operands (order matters,
    // of course!)
    uint8_t mod_rm(uint8_t op1, uint8_t op2);

    // Push value directly to the stack by passing value as an immediate.
    void push_imm32(int32_t value);

    void push_reg(uint8_t reg);

    // Pop a value off the stack into the specified register.
    void pop(uint8_t reg);

    // The following stack operators perform their respective operation
    // on the last two values in the stack (or just one value in the case
    // of negation and uplus), then pushes the result back into the stack.
    void stack_add();
    void stack_subtract();
    void stack_multiply();
    
    // mod = false will return the quotient, mod = true will return the
    // remainder.
    void stack_divide(bool mod);
    void stack_exponentiate();
    void stack_uplus();
    void stack_negation();

    Node* parse_tree_head;      // Parse tree to build the program from.
    unsigned char * program;    // Address of our program in memory
    int program_offset = 0;     // offset to 'program' shows where the next encoded byte should go.
    const unsigned int PROGRAM_SIZE = 50000;    // Big buffer for our program!
};

#endif