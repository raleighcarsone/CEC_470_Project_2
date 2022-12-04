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

unsigned char memory[65536];
unsigned char ACC = 0;
unsigned char IR = 0;
unsigned int MAR = 0;
unsigned int PC = 0;

int main(int argc, char* argv[])
{
/*************************************************************************
* Read memory data in from mem_in.txt
**************************************************************************/
uint8_t file_data [65536];
uint8_t s0;
uint8_t s1;
FILE *file_ptr;
file_ptr = fopen("mem_in (1).txt", "r+");
fread(file_data,sizeof(file_data),1,file_ptr);

for (int i = 0; i < 65537; i+=3) {
    if (file_data[i] >= '0' && file_data[i] <= '9')
        s0 = (file_data[i] - '0') <<4;
    if (file_data[i] >= 'A' && file_data[i] <= 'F')
        s0 = (file_data[i] - 'A' + 10) <<4;
    if (file_data[i] >= 'a' && file_data[i] <= 'f')
        s0 = (file_data[i] - 'a' + 10) <<4;

    if (file_data[i+1]  >= '0' && file_data[i+1]  <= '9')
        s1 = file_data[i+1]  - '0';
    if (file_data[i+1]  >= 'A' && file_data[i+1]  <= 'F')
        s1 = file_data[i+1]  - 'A' + 10;
    if (file_data[i+1]  >= 'a' && file_data[i+1]  <= 'f')
        s1 = file_data[i+1]  - 'a' + 10;

    if (i == 0){
        memory[i] = s1 + s0;
    }else{
        memory[i/3] = s1 + s0;
    }
}

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
* Local Variables
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
void arithmethic_instruction_handle(void)
{

}

void branch_instruction_handle(void)
{

}


void memory_instruction_handle(void)
{
/***************************************************************
* Local Variables
**************************************************************/
uint8_t  method;
uint8_t  register_;
uint8_t  function;
uint16_t data1;
uint16_t pc_offset;

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

/* Get data from memory */
    switch(method)
    {
        case (0):
            data1 = (memory[PC + 1] << 8) + memory[PC + 2];
            break;

        case (1):
            if(register_ == REG_ACC)
            {
                data1 = memory[PC+1];
                pc_offset = 1;
            }else if (register_ == REG_IR_MAR)
            {
                data1 = (memory[PC + 1] << 8) + memory[PC + 2];
                pc_offset = 2;
            }
            break;

        case (2):
                data1 = memory[MAR];
            break;
    }

 /******************************************************
 * Process load/store instructions
 *****************************************************/
 switch (function)
 {
     case (LOAD_OP):
         if (register_ == REG_ACC)
         {
             ACC = (uint8_t)data1;
         }else if(register_ == REG_IR_MAR)
         {
             MAR = data1;
         }
         break;

     case (STORE_OP):
         if (register_ == REG_ACC)
         {
             memory[data1] = ACC;
         }else if(register_ == REG_IR_MAR)
         {
             memory[data1] = memory[MAR];
         }
         break;
 }

 /* Increment program counter (PC) */
 PC += pc_offset;

}