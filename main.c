#include <stdio.h>
#include <stdint.h>
#define HALT_OPCODE 0x19

void fetchNextInstruction(void);
void executeInstruction(void);
void arithmethic_instruction_handle(void);
void branch_instruction_handle(void);
void memory_instruction_handle(void);

unsigned char memory[65536];
unsigned char ACC = 0;
unsigned char IR = 0;
unsigned int MAR = 0;
unsigned int PC = 0;

int main(int argc, CHAR* argv[])
{
    // Execution loop. Continue fetching and executing
    // until PC points a HALT instruction
    while(memory[PC] != HALT_OPCODE)
    {
        fetchNextInstruction();
        executeInstruction();
    }
    return 0;
}

/*************************************************************************
* Function: fetchNextInstruction
*
* Purpose: Gets the next instruction from memory and stores to IR
**************************************************************************/
void fetchNextInstruction(void)
{
    IR = memory[PC++];
}

/*************************************************************************
* Function: executeInstruction
*
* Purpose:
**************************************************************************/
void executeInstruction(void)
{
/***************************************************************
 * 1. Decode the instruction
 * 2. Call the correct function
 * 3. Update whatever registers are affected
 **************************************************************/
}

void arithmethic_instruction_handle(void)
{

}

void branch_instruction_handle(void)
{

}


void memory_instruction_handle(void)
{

}