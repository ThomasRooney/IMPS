#ifndef COMMON_HEADER_PARSED
#define COMMON_HEADER_PARSED
#define FATAL_ERROR 10
#define MEMORY_SIZE 65535
#define MAX_REGISTERS 32
#define PC_BOUNDARY 4
#define GENERIC_ERROR -1
#define kDisplayWidth 36

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef unsigned char opCode;
typedef unsigned int registerIndex;
typedef unsigned int address;

/**************************************************************
 * Instruction Structure Definition.
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
        char * output_file;
	int verbose;
	int step;
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

int instructionFromOpcode(opCode opCode) {
	if (opCode < NUMBER_OF_OPCODES) {
		return instructionType [opCode];
	}
	return INSTRUCTION_TYPE_UNKNOWN;
}


/**************************************************************
 * State information containing registers for emulation
 */


typedef struct { 
  unsigned int reg [32];
  char * MEMORY;
  int programCounter;
}state;
 
 typedef enum stateSignal{ 
	STATE_INCREMENTPC = 1,
	STATE_HALT = 2,
	STATE_CONTINUE = 3
}stateSignal;
 
/************************************************************
 * Parsing Arguments - Common Function
 */
void parseArguments(int argc, char **argv) {
	int iter;
	if (argc < 2 ) {
		printf("FATAL ERROR: Filename of assembled file needs to be given\n");
		exit(FATAL_ERROR);
	}
	main_args.file_name = argv[1];
 	if (argc >= 2) {
		for (iter=1; iter < argc; iter++) {
			if(strcmp(argv[iter],"-v") == 0) {
					main_args.verbose = 1;
					printf("Debug Mode (verbose) on\n");
			} else {
			if(strcmp(argv[iter],"-s") == 0) {
					main_args.step = 1;
					main_args.verbose = 1;
					printf("Step Through Debugging Mode on\n");
			} else { 
			if(strcmp(argv[iter],"-h") == 0) {
					#ifdef EMULATE
					printf("\nUsage: \"./emulate <filename> [-v|-s|-h]\"\n");
					#endif
					#ifdef ASSEMBLE
					printf("\nUsage: \"./assemble <filename> [-v|-s|-h]\"\n");
					#endif
					printf("Argument \"-v\": Verbose Mode\n");
					printf("Argument \"-s\": Step Through Mode\n");
					printf("Argument \"-h\": Help\n");
					exit(EXIT_SUCCESS);
			} else {
                        if (iter == 1) 
                                main_args.output_file = argv[2];
                        else
 				iter > 1 ? printf("ERROR: Unknown Argument: %s\n", argv[iter]) : 1;
			}
			}
			}
		}
	}
}

/************************************************************
 * Pauses program until enter is pressed
 */
void waitUntilEnter()
{
	while (1)
	{
		if ('\n'==getchar())
			break;
	}
}
/************************************************************
 * File Reading Function
 */
int readFile(const char * fileName, int * outputLength, char ** outputBuffer) {
	FILE *file;

	// Open file
	file = fopen(fileName, "rb");

	if (!file)
	{
		printf("FATAL ERROR: Unable to open file %s\n", fileName);
		return FATAL_ERROR;
	}

	// Get file length
	fseek(file, 0, SEEK_END);
	*outputLength=ftell(file);
	fseek(file, 0, SEEK_SET);

	// Allocate memory
	*outputBuffer=(char *)calloc(*outputLength+1, sizeof(char));
			//Note: it is the caller's requirement to free the memory
	if (!*outputBuffer)
	{
		fprintf(stderr, "Memory error!\n");
                                fclose(file);
		return FATAL_ERROR;
	}

	// Read file contents into buffer
	fread(*outputBuffer, *outputLength, 1, file);
	fclose(file);
	return EXIT_SUCCESS;
}

/*****************************************************************
 * Debugging Functions
 */

// PRE: #so = kDisplayWidth
char* pBin(unsigned int x,char *so)
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
 sprintf(so,"%s",s+i); // stick it in the given string
 return so;
}

void printBinaryInstruction(binaryInstruction i) {

	 char so[kDisplayWidth+1]; // working buffer for pBin
	 unsigned int val=i;
	 printf("Instruction: %s\n", pBin(val,so));
}



void dump_instruction(instruction parsedInstruction) {
	printf("Parsed Instruction Dump: \n");
	printf("  opCode = %i\n", parsedInstruction.raw.opCode);
	int instructionType = instructionFromOpcode(parsedInstruction.raw.opCode);
	switch(instructionType) {
			case INSTRUCTION_TYPE_R:
				printf("  Instruction Type R... Arguments:\n");
				printf("    R1: %u\n", parsedInstruction.rType.R1);
				printf("    R2: %u\n", parsedInstruction.rType.R2);
				printf("    R3: %u\n", parsedInstruction.rType.R3);
				break;
			case INSTRUCTION_TYPE_I:
				printf("  Instruction Type R... Arguments:\n");
				printf("    R1: %u\n", parsedInstruction.iType.R1);
				printf("    R2: %u\n", parsedInstruction.iType.R2);
				printf("    immediateValue: %u\n", parsedInstruction.iType.immediateValue);
				break;
			case INSTRUCTION_TYPE_J:
				printf("  Instruction Type J... Arguments:\n");
				printf("    address: %u\n", parsedInstruction.jType.address);
    			break;
			default:
				printf("Unknown Instruction Type!\n");
	}
}
#endif
