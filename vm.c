/*
  VM that implements the LC-3 architecture
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/mman.h>

// our total available memory -> 16bit address space where each location holds 16bits
unint16_t memory[UINT16_MAX]

// registers -> 8 general purpose, one program counter and one condition flag
enum
{
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC, /* program counter */
    R_COND,
    R_COUNT
};

// store registers in an array
uint16_t reg[R_COUNT];

// opcodes -> stuff our virtual cpu can compute
enum
{
    OP_BR = 0, /* branch */
    OP_ADD,    /* add  */
    OP_LD,     /* load */
    OP_ST,     /* store */
    OP_JSR,    /* jump register */
    OP_AND,    /* bitwise and */
    OP_LDR,    /* load register */
    OP_STR,    /* store register */
    OP_RTI,    /* unused */
    OP_NOT,    /* bitwise not */
    OP_LDI,    /* load indirect */
    OP_STI,    /* store indirect */
    OP_JMP,    /* jump */
    OP_RES,    /* reserved (unused) */
    OP_LEA,    /* load effective address */
    OP_TRAP    /* execute trap */
};

// flags -> that we will set them on the R_COND register that stores info regarding the last executed instruction
enum
{
    FL_POS = 1 << 0, /* P */
    FL_ZRO = 1 << 1, /* Z */
    FL_NEG = 1 << 2, /* N */
};

// main loop should follow the following
/*
1. Load one instruction from memory at the address of the PC register.
2. Increment the PC register.
3. Look at the opcode to determine which type of instruction it should perform.
4. Perform the instruction using the parameters in the instruction.
5. Go back to step 1.
*/
int main(int argc, const char* argv[])
{
    // location where our vm should fetch first instruction from
    enum { PC_START = 0x3000 };
    // set value of PC register as address of this instruction
    reg[R_PC] = PC_START;

    int running = 1;
    while (running)
    {
        // fetch the instruction and increment PC by 1
        unint16_t instr = mem_read(reg[R_PC]++)
        // get the operation by right shifting the instruction
        // https://justinmeiners.github.io/lc3-vm/supplies/lc3-isa.pdf
        unint16_t op = instr >> 12;

        switch (op)
        {
            case OP_ADD:
                add(instr);
                break;
            case OP_AND:
                {AND, 7}
                break;
            case OP_NOT:
                {NOT, 7}
                break;
            case OP_BR:
                {BR, 7}
                break;
            case OP_JMP:
                {JMP, 7}
                break;
            case OP_JSR:
                {JSR, 7}
                break;
            case OP_LD:
                {LD, 7}
                break;
            case OP_LDI:
                {LDI, 6}
                break;
            case OP_LDR:
                {LDR, 7}
                break;
            case OP_LEA:
                {LEA, 7}
                break;
            case OP_ST:
                {ST, 7}
                break;
            case OP_STI:
                {STI, 7}
                break;
            case OP_STR:
                {STR, 7}
                break;
            case OP_TRAP:
                {TRAP, 8}
                break;
            case OP_RES:
            case OP_RTI:
            default:
                {BAD OPCODE, 7}
                break;
        }
    }
}

/*
Update the condition flag based on latest computation
*/
void update_flags(uint16_t r)
{
    if (reg[r] == 0)
    {
        reg[R_COND] = FL_ZRO;
    }
    else if (reg[r] >> 15) /* a 1 in the left-most bit indicates negative */
    {
        reg[R_COND] = FL_NEG;
    }
    else
    {
        reg[R_COND] = FL_POS;
    }
}

/*
Sign extending a number:
- if number is positive, add 0s to the left
- if number is negative, add 1s
(two's complement)
*/
uint16_t sign_extend(uint16_t x, int bit_count)
{
    if ((x >> (bit_count - 1)) & 1) {
        x |= (0xFFFF << bit_count);
    }
    return x;
}

/* Define operations */
/* 
ADD operation

Two modes; register mode vs immediate

- Register mode: get both the operands from registers sr1 and sr2
op   dr  sr1 m    sr2
0001 001 010 0 00 011

- Immediate mode: get one operand from register sr1 and the other from the instr (5 bit number)
op   dr  sr1 m imm5
0001 001 010 1 00101

We require sign extending to expand 5 bit number to a 16 bit number
*/
void add(unint16_t instr) {
    // destination register
    unint16_t dr = (instr >> 9) & 0x7;
    // source register (sr1)
    unint16_t sr1 = (instr >> 6) & 0x7;
    // get mode
    unint16_t mode = (instr >> 5) & 0x1;

    if (mode)
    {
        // immediate mode
        unint16_t imm5 = sign_extend(instr & 0x1F, 5);
        reg[dr] = reg[sr1] + imm5;
    }
    else
    {
        unint16_t sr2 = instr & 0x7;
        reg[dr] = reg[sr1] + reg[sr2];
    }

    update_flags(dr);
}

/*
LDI instruction 

op   dr  pc_offset
1010 001 000001010

Loads the value stored in a memory location into a destination register
The memory location's value itself is calculated by adding the 9 bit number (after 
sign extending it) to the the program counter value. Therefore, it loads a value
stored in the the 'neighbourhood' of the instruction being executed.
*/
void ldi(uint16_t instr) {
    // destination register where value needs to be stored
    unint16_t dr = (instr >> 9) & 0x7;
    // get the pc_offset value by sign extending the 9 bits
    unint16_t offset = instr & 0x

}