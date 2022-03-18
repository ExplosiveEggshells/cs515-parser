#ifndef ENCODED_PROGRAM_H
#define ENCODED_PROGRAM_H

#include <iostream>
#include <sys/mman.h>

#include "node.h"
#include "parse_exception.h"
#include "disasm.h"

#define VERBOSE true
#define ENCODE program[program_offset++]=

class EncodedProgram
{
public:
    EncodedProgram(Node* parse_tree_head);
    void assemble();
    void execute();
private:
    void initialize();
    void traverse(Node* n);
    void encode_imm32(int32_t value);
    uint8_t mod_rm(uint8_t op1, uint8_t op2);

    // Push value directly to the stack by passing value as an immediate.
    void push_imm32(int32_t value);
    void push_reg(uint8_t reg);
    void pop(uint8_t reg);

    // Arithmetic
    void stack_add();
    void stack_subtract();
    void stack_multiply();
    void stack_negation();

    Node* parse_tree_head;
    unsigned char * program;
    int program_offset = 0;
    const unsigned int PROGRAM_SIZE = 50000;
};

#endif