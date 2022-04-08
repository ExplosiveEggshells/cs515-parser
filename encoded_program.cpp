#include "encoded_program.h"

EncodedProgram::EncodedProgram(Node *parse_tree_head)
{
    this->parse_tree_head = parse_tree_head;
    initialize();
}

void EncodedProgram::assemble()
{
    traverse(parse_tree_head);

    // The result should be the final item in the stack. Pop it out to EAX
    // and return it!
    pop(0);

    ENCODE 0xc3;   // RET
}

void EncodedProgram::execute()
{
    int value = 0;
    
    value = ((int(*)())program)();
    printf("Program Length: %u bytes\n", program_offset);
    printf("Output: %d\n", value);

    munmap(program, PROGRAM_SIZE);
}

void EncodedProgram::initialize()
{
    program_offset = 0;
    program = (unsigned char *)mmap(
        0,
        PROGRAM_SIZE,
        PROT_EXEC | PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0);

    // std::cout << "Allocated " << PROGRAM_SIZE << " bytes at " << (int *)program << " for program." << std::endl;

    if (errno)
    {
        perror("mmap");
        throw "Failed to allocate memory for program!";
    }
}

void EncodedProgram::traverse(Node *n)
{
    // Post-order for the zig-zagging tree involves
    // calling our child first, then visting here, and finishing
    // with the sibling.

    if (n->child != nullptr)
    {
        traverse(n->child);
    }

    // Based on the ID of the current token, we'll want
    // to either encode a push of it's i-value if it's an integer,
    // or encode an operation on the previous two items in the stack
    // (unless it is unary, of course).
    switch (n->token.id)
    {
    case (TypeID::INTEGER):
        push_imm32(n->token.i_value);
        break;

    case ('+'):
        stack_add();
        break;

    case ('-'):
        stack_subtract();
        break;

    case ('*'):
        stack_multiply();
        break;

    case ('/'):
        stack_divide(false);
        break;

    case (TypeID::MOD):
        stack_divide(true); // divide returns the remainder if parm1 is true.
        break;

    case ('^'):
        stack_exponentiate();
        break;

    case (TypeID::NEGATE):
        stack_negation();
        break;

    case (TypeID::UPLUS):
        stack_uplus();
        break;

    default:    // Didn't recognize the ID? that's no good, throw an error!
        throw ParseException("Encountered unsupported symbol during assembly.", n->token);
        // exit(-1);
    }
    

    if (n->sibling != nullptr)
    {
        traverse(n->sibling);
    }
}

void EncodedProgram::encode_imm32(int32_t value)
{
    // Add the lowest-order eight bits to the program,
    // then shift the number rightwards by 8 bits. Do
    // this four times.
    for (int i = 0; i < 4; i++)
    {
        ENCODE value & 0xff;
        value >>= 8;
    }
}

uint8_t EncodedProgram::mod_rm(uint8_t op1, uint8_t op2)
{
    // ModR/M ascends from 0xc0 to 0xFF in column-major order,
    // so we can navigate it like a flattened 2D array!
    return 0xc0 + (op1 * 8) + op2;
}

void EncodedProgram::push_imm32(int32_t value)
{
    if (VERBOSE)
        printf("PUSH %d\n", value);
    
    // PUSH id
    ENCODE 0x68;
    encode_imm32(value);
}

void EncodedProgram::push_reg(uint8_t reg)
{
    if (VERBOSE)
        printf("PUSH r%d\n", reg);

    // PUSH r#
    ENCODE 0x50 + reg;
}

void EncodedProgram::pop(uint8_t reg)
{
    if (VERBOSE)
        printf("POP r%d\n", reg);

    // POP r#
    ENCODE 0x58 + reg;
}

void EncodedProgram::stack_add()
{
    if (VERBOSE)
        printf("->STACK ADD\n");

    // Grab the two operands from the stack into EAX, ECX
    pop(1);
    pop(0);

    // ADD EAX, ECX
    ENCODE 0x01;
    ENCODE mod_rm(1, 0);

    // The result goes back on the stack.
    // POP EAX
    push_reg(0);

    if (VERBOSE)
        printf("<-STACK ADD\n");
}

void EncodedProgram::stack_subtract()
{
    if (VERBOSE)
        printf("->STACK_SUB\n");
    
    // Grab the two operands from the stack into EAX, ECX. Order matters here.
    pop(1); 
    pop(0); 

    // SUB EAX, ECX
    ENCODE 0x29;
    ENCODE mod_rm(1, 0);

    // PUSH r0
    push_reg(0);

    if (VERBOSE)
        printf("<-STACK_SUB\n");
}

void EncodedProgram::stack_multiply()
{
    if (VERBOSE)
        printf("->STACK_MULT\n");

    pop(1);
    pop(0);

    // IMULT EAX, ECX
    ENCODE 0x0f;
    ENCODE 0xaf;
    ENCODE mod_rm(0, 1);

    push_reg(0);

    if (VERBOSE)
        printf("<-STACK_MULT\n");
}

void EncodedProgram::stack_divide(bool mod)
{
    if (VERBOSE)
        printf("->STACK_DIV\n");

    pop(1);
    pop(0);

    // Zero out EDX.
    // XOR EDX, EDX
    ENCODE 0x31;
    ENCODE mod_rm(2, 2);

    // IDIV EAX:EDX, ECX
    ENCODE 0xf7;
    ENCODE 0xf9;

    // Push either EAX for the Quotient or EDX for the remainder.
    uint8_t returned_reg = (mod) ? 2 : 0;
    push_reg(returned_reg);

    if (VERBOSE)
        printf("<-STACK_DIV\n");
}

void EncodedProgram::stack_exponentiate()
{
    if (VERBOSE)
        printf("->STACK_EXP\n");

    pop(1);
    pop(0);

    // In the future, exponentiation will occur here!
    // For now, just return the left operand.

    push_reg(0);

    if (VERBOSE)
        printf("<-STACK_EXP\n");
}

void EncodedProgram::stack_uplus()
{
    // Grab the last value of the stack, do nothing with it,
    // and then push it back on the stack. A very apathetic operator.
    pop(0);

    push_reg(0);
}

void EncodedProgram::stack_negation()
{
    if (VERBOSE)
        printf("->STACK_NEG\n");

    pop(0);

    // NEG EAX
    ENCODE 0xf7;
    ENCODE 0xd8;

    push_reg(0);

    if (VERBOSE)
        printf("<-STACK_NEG\n");
}