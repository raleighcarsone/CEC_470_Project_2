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
* Variables
**************************************************************/
uint8_t math_operation_mask = 1 << 7;
uint8_t memory_operation_mask = 0xF << 4;
uint8_t branch_operation_mask = 1 << 4;

/***************************************************************
 * 1. Decode the instruction
 * 2. Call the correct function
 * 3. Update whatever registers are affected
 **************************************************************/

/* Check for mathematical operation */
if((IR & math_operation_mask) == 0x80)
{
    arithmethic_instruction_handle();
}

if(IR & memory_operation_mask < 0x10)
{
    memory_instruction_handle();
}

if((IR & branch_operation_mask) == 0x10)
{
    branch_instruction_handle();
}

}
void arithmethic_instruction_handle(void) {
/***************************************************************
* Variables
**************************************************************/
    int numberOfBytes = 0; //for size of the instruction
    uint8_t opcode = 0; //will hold the 8 bits of opcode
    unsigned int function = 0;
    unsigned int destination = 0; //holds source value being used
    unsigned int source = 0; //holds destination value

/***************************************************************
 * 1.
 * 2.
 * 3.
 **************************************************************/

    //identifies 8 bits of opcode
    numberOfBytes = sizeof(memory[PC]); //find size of instruction as it can be 1, 2, or 3 bytes
    if (numberOfBytes == 1) {
        opcode = memory[PC];
    }
    if (numberOfBytes == 2) {
        opcode = memory[PC] >> 8;
    }
    if (numberOfBytes == 3) {
        opcode = memory[PC] >> 16;
    }
    source = opcode << 6; //right two bits
    destination = (opcode & 12) >> 2; //12=1100. middle two bits
    function = (opcode & 112) >> 4; //112=01110000. middle 3 bits

    //store two values being used with switch statements
    switch (source) {
        case 0: //indirect (MAR used as pointer)
            source = memory[MAR];
            break;
        case 1: //accumulator ACC
            source = ACC;
            break;
        case 2: //constant
            source = 0; //WHAT IS CONSTANT
            break;
        case 3: //memory
            source = memory[PC];
            break;
    }
    switch (destination) {
        case 0: //indirect (MAR used as pointer)
            destination = memory[MAR];
            break;
        case 1: //accumulator ACC
            destination = ACC;
            break;
        case 2: //address register MAR
            destination = MAR;
            break;
        case 3: //memory
            destination = memory[PC];
            break;
    }
    //now perform function and save back to proper destination
    switch (function){
        case 0: //AND
            destination = source & destination;
            break;
        case 1: //OR
            destination = source | destination;
            break;
        case 2: //XOR
            destination = source ^ destination;
            break;
        case 3: //ADD
            destination = source + destination;
            break;
        case 4: //SUB
            destination = source - destination;
            break;
        case 5: //INC
            destination = source++;
            break;
        case 6: //DEC
            destination = source--;
            break;
        case 7: //NOT
            destination = ~source;
            break;
    }

    switch(destination){
        case 0: //indirect (MAR used as pointer)
            memory[MAR] = destination;
            break;
        case 1: //accumulator ACC
            ACC = destination;
            break;
        case 2: //address register MAR
            MAR = destination;
            break;
        case 3: //memory
            memory[PC] = destination;
            break;
    }

}

void branch_instruction_handle(void)
{
    switch (IR & 0x07)
    {
        case 0: // Unconditional branch -- Load PC with (memory[pc+1] << 8) + memory[pc+2]
            B target_label
            break;
        case 1: // Branch if ACC=0
            // Branch if Zero (BRZ)
            BRZ $register, $register, target_label

        case 2: // Branch if ACC!=0
            // Branch on not equal(BNE)
            BNE $register, $register, target_label
        case 3: // Branch if ACC<0
            // Branch if less than (BLT)
            BLT $register, $register, target_label
        case 4: // Branch if ACC<=0
            // Branch if less than or Equal(BLE)
            BLE $register, $register, target_label
        case 5: // Branch if ACC>0
            //Branch if greater than (BGT)
            BGT $register, $register, target_label
        case 6: // Branch if ACC>=0
            // Branch if greater than or equal(BGE)
            BGE $register, $register, target_label
}


void memory_instruction_handle(void)
{

}