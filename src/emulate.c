#include "common.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// We want to read the file into memory so we can deal with it in a
// faster and easier way


/**************************************************************************
* Callback functions for opcode operations
*/

// no state change will call the termination in the emulation loop
void doOpCode_HALT(instruction * args, state * state) 
{}

void doOpCode_ADD (instruction * args, state * state) {
  state->reg[args->rType.R1] =
  state->reg[args->rType.R2] + state->reg[args->rType.R3];
  state->programCounter += PC_BOUNDARY;
}

void doOpCode_ADDI(instruction * args, state * state) {
  state->reg[args->iType.R1] =
  state->reg[args->iType.R2] + args->iType.immediateValue; 
  state->programCounter += PC_BOUNDARY;
}

void doOpCode_SUB (instruction * args, state * state) {
  state->reg[args->rType.R1] =
  state->reg[args->rType.R2] - state->reg[args->rType.R3];
  state->programCounter += PC_BOUNDARY;
}

void doOpCode_SUBI(instruction * args, state * state) {
  state->reg[args->iType.R1] =
  state->reg[args->iType.R2] - args->iType.immediateValue; 
  state->programCounter += PC_BOUNDARY;
}

void doOpCode_MUL (instruction * args, state * state) {
  state->reg[args->rType.R1] =
  state->reg[args->rType.R2] * state->reg[args->rType.R3];
  state->programCounter += PC_BOUNDARY;
}

void doOpCode_MULI(instruction * args, state * state) {
  state->reg[args->iType.R1] =
  state->reg[args->iType.R2] * args->iType.immediateValue; 
  state->programCounter += PC_BOUNDARY;
}

void doOpCode_LW  (instruction * args, state * state) {
  state->reg[args->iType.R1] =
  state->MEMORY[state->reg[args->iType.R2] + args->iType.immediateValue];
  state->programCounter += PC_BOUNDARY;
}

void doOpCode_SW  (instruction * args, state * state) {
  state->MEMORY[state->reg[args->iType.R2] + args->iType.immediateValue] =
  state->reg[args->iType.R1];
  state->programCounter += PC_BOUNDARY;
}

void doOpCode_BEQ (instruction * args, state * state) {
  if (state->reg[args->iType.R1] == state->reg[args->iType.R2]) {
    state->programCounter += (args->iType.immediateValue * PC_BOUNDARY);
  }
}

void doOpCode_BNE (instruction * args, state * state) {
  if (state->reg[args->iType.R1] != state->reg[args->iType.R2]) {
    state->programCounter += (args->iType.immediateValue * PC_BOUNDARY);
  }
}

void doOpCode_BLT (instruction * args, state * state) {
  if (state->reg[args->iType.R1] < state->reg[args->iType.R2]) {
    state->programCounter += (args->iType.immediateValue * PC_BOUNDARY);
  }
}

void doOpCode_BGT (instruction * args, state * state) {
  if (state->reg[args->iType.R1] > state->reg[args->iType.R2]) {
    state->programCounter += (args->iType.immediateValue * PC_BOUNDARY);
  }
}

void doOpCode_BLE (instruction * args, state * state) {
  if (state->reg[args->iType.R1] <= state->reg[args->iType.R2]) {
    state->programCounter += (args->iType.immediateValue * PC_BOUNDARY);
  }
}

void doOpCode_BGE (instruction * args, state * state) {
  if (state->reg[args->iType.R1] >= state->reg[args->iType.R2]) {
    state->programCounter += (args->iType.immediateValue * PC_BOUNDARY);
  }
}

void doOpCode_JMP (instruction * args, state * state) {
  state->programCounter = args->jType.address;
}

void doOpCode_JR  (instruction * args, state * state) {
  state->programCounter = args->rType.R1;
}

void doOpCode_JAL (instruction * args, state * state) {
  state->reg[31] = state->programCounter + PC_BOUNDARY;
  state->programCounter = args->jType.address;
}

void doOpCode_OUT (instruction * args, state * state) {
  printf("%i", state->reg[args->rType.R1]);
  state->programCounter += PC_BOUNDARY;
}


void printBinaryInstruction(binaryInstruction i) {

	 char so[kDisplayWidth+1]; // working buffer for pBin
	 unsigned int val=i;
	 printf("Instruction: %s\n", pBin(val,so));
}

void dec2bin(unsigned int decimal, char *binary)
{
  int  k = 0, n = 0;
  int  neg_flag = 0;
  int  remain;
  
  int  old_decimal;  // for test
  char temp[80];
  // take care of negative input
  if (decimal < 0)
  {
    decimal = -decimal;
    neg_flag = 1;
  }
  do
  {
    old_decimal = decimal;   // for test
    remain    = decimal % 2;
    // whittle down the decimal number
    decimal   = decimal / 2;
    // this is a test to show the action
     printf("%d/2 = %d  remainder = %d\n", old_decimal, decimal, remain);
    // converts digit 0 or 1 to character '0' or '1'
    temp[k++] = remain + '0';
  } while (decimal > 0);
  if (neg_flag)
    temp[k++] = '-';       // add - sign
  else
    temp[k++] = ' ';       // space
  // reverse the spelling
  while (k >= 0)
    binary[n++] = temp[--k];
  binary[n-1] = 0;         // end with NULL
}

void printBinary(int decimal) {
	char * binary = malloc(250);
	dec2bin(decimal, binary);
	printf("%s", binary);
	free(binary);
}

int instructionFromOpcode(opCode opCode) {
	if (opCode < NUMBER_OF_OPCODES) {
		return instructionType [opCode];
	}
	return INSTRUCTION_TYPE_UNKNOWN;
}


// Inclusive
int readBitField(binaryInstruction bin,unsigned char start, unsigned char end)
{
	unsigned int mask = ((2<<((end-start+1)))-1)<<(sizeof(bin)*8-end);
	unsigned int extracted;
	printf("Reading Bit Field...\n   start = %i\n   end = %i\n   mask =      ", start, end);

	printBinary(mask);
	printf("   decimal: %u\n   instruction: ", mask);printBinaryInstruction(bin>>start);
	printf("   Extracted = ");
	extracted = (int)(((bin>>start) & mask)>>start);
	printBinary( extracted );
	printf("   decimal: %u\n", extracted);
	return extracted;
}

inline void endian_swap(unsigned int *x)
{
    *x = (*x>>24) |
        ((*x<<8) & 0x00FF0000) |
        ((*x>>8) & 0x0000FF00) |
        (*x<<24);
}

instruction disassembleInstruction(binaryInstruction binInstruction) {
	instruction outputInstruction;
	opCode opCode;
	memcpy(&outputInstruction, &binInstruction, sizeof(binaryInstruction));
	endian_swap((unsigned int *)&outputInstruction);
	return outputInstruction;
}




void dump_state(state curState) {
	int i;
	fprintf(stderr, "PROGRAM_HALTED\n--------------\n\
					ProgramCounter: %i\n", curState.programCounter);
	for (i = 0; i < MAX_REGISTERS; i++) {
		fprintf(stderr, "Register[%i]: %i\n", i, curState.reg[i]);
	}
}

void dump_instruction(instruction parsedInstruction) {
	printf("Parsed Instruction Dump: \n");
	printf("  opCode = %i\n", parsedInstruction.opCode);
	int instructionType = instructionFromOpcode(parsedInstruction.opCode);
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
				printf("    address: %u\n", parsedInstruction.jType);
    			break;
			default:
				printf("Unknown Instruction Type!\n");
	}
}

state initialise_state(void) {
	state virtualState;
	memset(virtualState.reg, 0, sizeof(virtualState.reg));
	virtualState.programCounter = 0;
	virtualState.MEMORY = calloc(MEMORY_SIZE, sizeof(int));
	// allocate 65535 addresses with 32 bit boundary.
	return virtualState;
}

void emulation_loop(const char * inputBuffer, int inputLength) {
	instruction parsedInstruction;
	binaryInstruction binaryInstruction;
	int testBuffer;
	void (*opCodeFunction)(instruction *, state *);
	state stateOld;
	state state = initialise_state();

	memcpy(&testBuffer, inputBuffer, 4);
	printBinaryInstruction(testBuffer);

	// Whilst state is changing every iteration, else something's gone wrong
	do
	{
		memcpy(&stateOld, &state, sizeof(state));
		memcpy(&binaryInstruction, inputBuffer+state.programCounter, sizeof(binaryInstruction));
		parsedInstruction = disassembleInstruction(binaryInstruction);
		if (main_args.verbose == 1) {
			dump_instruction(parsedInstruction);
		}
		opCodeFunction = (opCodeDictionary[parsedInstruction.opCode]);
		opCodeFunction(&parsedInstruction, &state);
	} while (memcmp(&state,&stateOld,sizeof(state)) != 0);
	// Dump PC and registers into stderr
	dump_state(state);
}





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

int main(int argc, char **argv) {
	int iter;
	char * inputBuffer;
	int  * inputLength = malloc(sizeof(int));
	if (argc < 2 ) {
		printf("FATAL ERROR: Filename of assembled file needs to be given\n");
		return EXIT_SUCCESS;
	}

	main_args.file_name = argv[1];
 
	if (argc > 2) {
		for (iter=2; iter < argc; iter++) {
			if(strcmp(argv[iter],"-v") == 0) {
					main_args.verbose = 1;
					printf("Debug Mode (verbose) on\n");
			} else {
				printf("ERROR: Unknown Argument: %s\n", argv[iter]);
			}
		}
	}

	if (readFile(main_args.file_name, inputLength, &inputBuffer)>EXIT_SUCCESS) {
		return FATAL_ERROR;
	}
	printf("File Read Success. Length is %i bits\n", (*inputLength)*32);
	emulation_loop(inputBuffer, *inputLength);
	free(inputLength);
    return EXIT_SUCCESS;
}
