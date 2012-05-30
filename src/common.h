#ifndef COMMON_HEADER_PARSED
#define COMMON_HEADER_PARSED
#define FATAL_ERROR 10
#define MEMORY_SIZE 65535
#define MAX_REGISTERS 32
#define PC_BOUNDARY 4

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef unsigned char opCode;
typedef unsigned int registerIndex;
typedef unsigned int address;

/**************************************************************
 * Utility
 */
char* pBin(long int x, char *so);                   // version without fill
#define kDisplayWidth 36
char* pBin(long x,char *so)
{
 char s[kDisplayWidth+1];
 int i=kDisplayWidth;
 s[i--]=0x00;   // terminate string
 do
 { // fill in array from right to left
  if (!(i%9))
	s[i--] = ' ';
  else {
	s[i--] = (x&1) ? '1':'0';
	(x>>=1);
  }
 } while( i > 0); 
 i++;   // point to last valid character
 sprintf(so,"%s",s+i); // stick it in the temp string string
 return so;
}

/**************************************************************
 * Instructions
 */

#pragma pack(1)
typedef struct {
	int padding :11;
	registerIndex R3:5; // second source register index
	registerIndex R2:5; // first source register index
	registerIndex R1:5;  // destination register index
	opCode opCode:6;
}RTypeInstruction;

 // Reversed ordering for weird endian reasons
#pragma pack(1)
typedef struct {

	unsigned short immediateValue:16; // 16 bits for the intermediate value
	registerIndex R2:5;       // source register index
	registerIndex R1:5;       // destination register index
	opCode opCode:6;
}ITypeInstruction;


#pragma pack(1)
typedef struct {;
	 address address:26;
	 opCode opCode:6;
}JTypeInstruction;

#pragma pack(1)
typedef struct {

	// The union causes padding between bitfields
	// This causes sizeof(instruction) == 5
	union {
		RTypeInstruction rType;
		ITypeInstruction iType;
		JTypeInstruction jType;
		struct { 
			int padding : 26;
			opCode opCode : 6;
		} raw;
	};
}instruction;

typedef struct {
	char * file_name;
	int verbose;
}arguments;

// Every instruction is 32 bits long, or 4 bytes
typedef unsigned int binaryInstruction;

arguments main_args;


// A char is 8 bits
// a int is 32 bits

/**************************************************************
 * OpCode Definitions
 */
typedef enum opCodeDef {
	OPCODE_HALT = 0,
	OPCODE_ADD = 1,
	OPCODE_ADDI = 2,
	OPCODE_SUB = 3,
	OPCODE_SUBI = 4,
	OPCODE_MUL = 5,
	OPCODE_MULI = 6,
	OPCODE_LW = 7,
	OPCODE_SW = 8,
	OPCODE_BEQ = 9,
	OPCODE_BNE = 10,
	OPCODE_BLT = 11,
	OPCODE_BGT = 12,
	OPCODE_BLE = 13,
	OPCODE_BGE = 14,
	OPCODE_JMP = 15,
	OPCODE_JR = 16,
	OPCODE_JAL = 17,
	OPCODE_OUT = 18,
	NUMBER_OF_OPCODES = 19
}opCodeDef;


/**************************************************************
 * Instruction Type Definitions
 */
#define INSTRUCTION_TYPE_UNKNOWN   0
#define INSTRUCTION_TYPE_R         1
#define INSTRUCTION_TYPE_I         2
#define INSTRUCTION_TYPE_J         3
#define INSTRUCTION_TYPE_NA        4
 
 
const int instructionType [NUMBER_OF_OPCODES] = {
	INSTRUCTION_TYPE_NA, 
	INSTRUCTION_TYPE_R,
	INSTRUCTION_TYPE_I,
	INSTRUCTION_TYPE_R,
	INSTRUCTION_TYPE_I,
	INSTRUCTION_TYPE_R,
	INSTRUCTION_TYPE_I,
	INSTRUCTION_TYPE_I,
	INSTRUCTION_TYPE_I,
	INSTRUCTION_TYPE_I,
	INSTRUCTION_TYPE_I,
	INSTRUCTION_TYPE_I,
	INSTRUCTION_TYPE_I,
	INSTRUCTION_TYPE_I,
	INSTRUCTION_TYPE_I,
	INSTRUCTION_TYPE_J,
	INSTRUCTION_TYPE_R,
	INSTRUCTION_TYPE_J,
	INSTRUCTION_TYPE_R
};


/**************************************************************
 * State information containing registers for emulation
 */


typedef struct { 
  unsigned int reg [32];
  char * MEMORY;
  unsigned int programCounter;
}state;
 
 typedef enum stateSignal{ 
	STATE_INCREMENTPC = 1,
	STATE_HALT = 2,
	STATE_CONTINUE = 3
}stateSignal;
 


#endif
