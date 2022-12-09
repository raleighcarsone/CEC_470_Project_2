#include <stdio.h>
#include <stdint.h>
#include "memory_instruction_lib.h"
#include <stdlib.h>

#define HALT_OPCODE 0x19

void fetchNextInstruction(void);
void executeInstruction(void);
void arithmethic_instruction_handle(void);
void branch_instruction_handle(void);
void memory_instruction_handle(void);
void halt_instruction_handle(void);
void NOP_instruction_handle(void);

//unsigned char memory[65536];
unsigned char ACC = 0;
unsigned char IR = 0;
unsigned int MAR = 0;
unsigned int PC = 0;
uint8_t error_counter = 1;
uint8_t opcodes[65536] = {};

int main(int argc, char* argv[])
{
    // Execution loop. Continue fetching and executing
    // until PC points a HALT instruction
    while(memory[PC] != HALT_OPCODE /* && PC < 150  */ )
    {
        fetchNextInstruction();
        executeInstruction();
    }

    /* Print results */
    printf("\n");
    for (int i = 0; i < 65536; i++) {
        if(memory[i] != expected_output[i])
        {
            printf("#%d\t Error at memory index %d, opcode: 0x%02x, expected: 0x%02x, actual 0x%02x\n", error_counter, i, opcodes[i], expected_output[i], memory[i]);
            error_counter += 1;
        }
    }
    printf("\nTest Complete");

  return 0;
}

/*************************************************************************
* Function: fetchNextInstruction
*
* Purpose: Gets the next instruction from memory and stores to IR
**************************************************************************/
void fetchNextInstruction(void)
{
    printf("\nPrev PC: %d\t Prev IR: 0x%02x\t", PC, IR );
    IR = memory[PC];
    opcodes[PC] = IR;
    PC = PC + 1;
    printf("Next PC: %d\t New IR: 0x%02x\t", PC, IR );
    //in function need to determine if it is 2 or 3 bytes to increment PC according
}

/*************************************************************************
* Function: executeInstruction
*
* Purpose:
**************************************************************************/
void executeInstruction(void)
{
/***************************************************************
* Local Variables
**************************************************************/
    uint8_t math_operation_mask = 0x80;
    uint8_t memory_operation_mask = 0xF0;
    uint8_t branch_operation_mask = 0xF8;

/***************************************************************
 * 1. Decode the instruction
 * 2. Call the correct function
 * 3. Update whatever registers are affected
 **************************************************************/

/* Check for mathematical operation */
    if((IR & math_operation_mask) == 0x80)
    {
        printf("arithmethic");
        arithmethic_instruction_handle();
    }

    else if((IR & memory_operation_mask) == 0)
    {
        printf("memory");
        memory_instruction_handle();
    }

    else if((IR & branch_operation_mask) == 0x10)
    {
        printf("branch");
        branch_instruction_handle();
    }
    else if (IR == 0x19)
    {
        printf("halt");
        halt_instruction_handle();
    }
    else if (IR == 0x18)
    {
        printf("NOP");
        NOP_instruction_handle();
    }

}
void arithmethic_instruction_handle(void) {
/***************************************************************
* Variables
**************************************************************/
    //int numberOfBytes = 0; //for size of the instruction
    //uint8_t opcode = 0; //will hold the 8 bits of opcode
    unsigned int function = 0;
    unsigned int destination = 0; //holds source value being used
    unsigned int source = 0; //holds destination value

/***************************************************************
 * 1.
 * 2.
 * 3.
 * loading 8 bit register: ACC = memory[PC + 1]
 * loading 16 bit register: MAR = (memory[PC+1] << 8) + memory[PC+2]
 * does this mean I loaded opcode wrong?
 **************************************************************/

///////identifies 8 bits of opcode////////////////////////////////////////
    source = IR & 3; //3=11. right two bits. use in decimal
    destination = (IR & 12) >> 2; //12=1100. middle two bits. use in decimal
    function = IR & 112; //112=01110000. middle 3 bits. use with hex
////////////////////////////////////////////////////////////////////////////

//////////get destination and source and save to variable//////////////////////
    switch ((IR & 12) >> 2) {
        case 0: //indirect (MAR used as pointer) is 8 bits. +0
            destination = memory[MAR];
            break;
        case 1: //accumulator ACC is 8 bits. +0
            destination = ACC;
            break;
        case 2: //address register MAR is 16 bits. only time constant is 2 bytes. +1
            destination = MAR;
            break;
        case 3: //memory. +2
            destination = memory[(memory[PC] << 8) + memory[PC+1]];
            break;
    }
    //store two values being used with switch statements
    switch (IR & 3) {
        case 0: //indirect (MAR used as pointer). +0
        if(((IR & 12) >> 2) == 2)
        {
            source = (memory[MAR] << 8) + memory[MAR+1];
        } else {
            source = memory[MAR];
        }
//            source = memory[MAR];
            break;
        case 1: //accumulator ACC. +0
                source = ACC;
            break;
        case 2: //constant +1 or 2
            if(((IR & 12) >> 2) == 2)//constant will be 16 bits if the destination is MAR. 8 otherwise
            {
                source =  (memory[PC] << 8) + memory[PC+1];
            }else {
                source = memory[PC];
            }
            break;
        case 3: //memory address. +2
            if(((IR & 12) >> 2) == 2) {
                source = memory[(memory[PC] << 8) + memory[PC+1]] + memory[((memory[PC] << 8) + memory[PC+1])+ 1];//address is 16 bits
            }else{
                source = memory[(memory[PC] << 8) + memory[PC+1]];
            }
            break;
    }
///////////////////////////////////////////////////////////////////////////////

//////////////now perform function/////////////////////////////////////////
    switch (function){
        case 0x00: //AND
            destination = source & destination;
            break;
        case 0x10: //OR
            destination = source | destination;
            break;
        case 0x20: //XOR
            destination = source ^ destination;
            break;
        case 0x30: //ADD
            destination = source + destination;
            break;
        case 0x40: //SUB
            destination = source - destination;
            break;
        case 0x50: //INC
            destination++;
            break;
        case 0x60: //DEC
            destination--;
            break;
        case 0x70: //NOT
            destination = ~source;
            break;
    }
/////////////////////////////////////////////////////////////////////////

///////save back to proper place///////////////////////////////////////
    switch ((IR & 12) >> 2) {
        case 0: //indirect (MAR used as pointer)
            memory[MAR] = destination & 0xff;
            break;
        case 1: //accumulator ACC
            ACC = destination & 0xff;
            break;
        case 2: //address register MAR
            MAR = destination & 0xffff;
            break;
        case 3: //memory
            memory[PC] = destination;
            break;
    }
///////////////////////////////////////////////////////////////////////////

///////need to increment PC for next opcode run//////////////////////////////////////////////////
    switch ((IR & 12) >> 2) {
        case 2: //address register MAR is 16 bits. only time constant is 2 bytes. +1
            PC=PC+1;
            break;
        case 3: //memory. +2
            PC=PC+2;
            break;
    }
    //store two values being used with switch statements
    switch (IR & 3) {
        case 2: //constant. +1 or 2
            if (((IR & 12) >> 2)==2){ //constant will be 16 bits if the destination is MAR. 8 otherwise
                PC=PC+1;
            }else{
                PC=PC+1;
            }
            break;
        case 3: //memory address. +2
            PC=PC+2; //address is 16 bits
            break;
    }
////////////////////////////////////////////////////////////////////////////////////////////////
}

//I do not know where to branch to so I set up the condition statements
void branch_instruction_handle(void)
{// initialize variables
//    uint8_t branch_opcode = (IR<<5)>>5;
    uint8_t branch_opcode = (IR & 0x7);
    uint16_t branch_address = (memory[PC] << 8) + memory[PC + 1];
    switch (branch_opcode) {
        case 0: // Unconditional branch -- Load PC with (memory[pc+1] << 8) + memory[pc+2]
            // B target_label
            PC = branch_address;
            break;
        case 1: // Branch if ACC=0
            // Branch if Zero (BRZ)
            // BRZ $register, $register, target_label
            if (ACC == 0)
                PC = branch_address;
            break;
        case 2: // Branch if ACC!=0
            // Branch on not equal(BNE)
            // BNE $register, $register, target_label
            if (ACC != 0)
                PC = branch_address;
            break;
        case 3: // Branch if ACC<0
            // Branch if less than (BLT)
            // BLT $register, $register, target_label
            if (((ACC & (1 << 7))) != 0)
                PC = branch_address;
            break;
        case 4: // Branch if ACC<=0
            // Branch if less than or Equal(BLE)
            // BLE $register, $register, target_label
            if ((((ACC & (1 << 7))) != 0) || (ACC == 0))
                PC = branch_address;
            break;
        case 5: // Branch if ACC>0
            //Branch if greater than (BGT)
            // BGT $register, $register, target_label
            if (ACC > 0)
                PC = branch_address;
            break;
        case 6: // Branch if ACC>=0
            // Branch if greater than or equal(BGE)
            // BGE $register, $register, target_label
            if (ACC >= 0)
                PC = branch_address;
            break;
    }}


void memory_instruction_handle(void)
{
/***************************************************************
* Local Variables
**************************************************************/
    uint8_t  method = 0;
    uint8_t  register_ = 0;
    uint8_t  function = 0 ;
    uint16_t  prevMAR = 0;

/******************************************************
* Check instruction bit [2] for "Register":
* If 0b0 - Accumulator ACC
* IF 0b1 - Index register MAR
*****************************************************/
    register_   = (IR & MEM_REGISTER_BIT_MASK) >> MEM_REGISTER_BIT_POSITION;

/******************************************************
 * Check instruction bits [1:0] for "Method":
 * 1. If 0b00 - Operand is used as address
 *
 * 2. If 0b01 - Operand is used as a constant
 *
 * 3. If 0b10 - Indirect (MAR used as pointer)
 *
 * NOTE: If 0b00, data1 is 16 bits
 * NOTE: If 0b01, 8bits for ACC, 16 bits for MAR
 *****************************************************/
    method = (IR & MEM_METHOD_BIT_MASK);

    /******************************************************
    * Check instruction bit [3] for "Function":
    * 1. 0 - Store (STORE)
    * 2. 1 - Load  (LOAD)
    *****************************************************/
    function = (IR & MEM_FUNCTION_BIT_MASK) >> MEM_FUNCTION_BIT_POSITION;

    switch (function) {
        case STORE_OP: /* Store operation */
            if (method == 0 && register_ == REG_ACC)/* Operand is used as address --> Storing from ACC */
            {
                memory[((memory[PC] << 8) + memory[PC + 1])] = ACC;
                PC += 2;
            }

            if (method == 1 && register_ == REG_ACC) /* Do nothing */
            {
                PC++;
            }

            if (method == 2 && register_ == REG_ACC) /* Indirect (MAR used as pointer) */
            {
                memory[MAR] = ACC;
            }

            if (method == 0 && register_ == REG_IR_MAR)/* Operand is used as address --> Storing from IR_MAR */
            {
                memory[((memory[PC] << 8) + memory[PC + 1])] = (MAR >> 8) & 0x00FF;
                memory[((memory[PC] << 8) + memory[PC + 1]) + 1] = (MAR & 0x00FF);
                PC += 2;
            }

            if (method == 1 && register_ == REG_IR_MAR) /* Do nothing */
            {
                PC++;
            }

            if (method == 2 && register_ == REG_IR_MAR) /* Indirect (MAR used as pointer) */
            {
                memory[MAR] = (MAR >> 8) & 0x00FF;
                memory[MAR + 1] = MAR & 0x00FF;
            }
            break;

        case LOAD_OP:
            prevMAR = MAR;

            if (method == 0 && register_ == REG_ACC)
            {
                ACC = memory[((memory[PC] << 8) + memory[PC + 1])];
                PC += 2;
            }

            if (method == 1 && register_ == REG_ACC)
            {
                ACC = memory[PC];
                PC++;
            }

            if (method == 2 && register_ && register_ == REG_ACC)
            {
                ACC = memory[MAR];
            }

            if (method == 0 && register_ == REG_IR_MAR)
            {
                MAR = (memory[((memory[PC] << 8) + memory[PC+1])]<<8) + memory[((memory[PC] << 8) + memory[PC+1]) + 1];
                PC += 2;
            }

            if (method == 1 && register_ == REG_IR_MAR)
            {
                MAR = (memory[PC]<<8) + memory[PC + 1];
                PC += 2;
            }

            if (method == 2 && register_ == REG_IR_MAR)
            {
                MAR = (memory[MAR]<<8) + memory[MAR + 1];
            }
            break ;
    }
}
 void halt_instruction_handle(void)
 {
    memory[PC] = HALT_OPCODE; 
 }
void NOP_instruction_handle(void)
{
   PC +=1; 
}
