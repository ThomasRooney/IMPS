#include "common.h"
#include "callbacks.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>



// We want to read the file into memory so we can deal with it in a
// faster and easier way

void printBinaryInstruction(binaryInstruction i) {

	 char so[kDisplayWidth+1]; // working buffer for pBin
	 unsigned int val=i;
	 printf("Instruction: %s\n", pBin(val,so));
}

int instructionFromOpcode(opCode opCode) {
	if (opCode < NUMBER_OF_OPCODES) {
		return instructionType [opCode];
	}
	return INSTRUCTION_TYPE_UNKNOWN;
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
	fprintf(stderr, "PROGRAM_HALTED\n--------------\
					\nProgramCounter: %i\n", curState.programCounter);
	for (i = 0; i < MAX_REGISTERS; i++) {
		fprintf(stderr, "  Register[%i]: %i\n", i, curState.reg[i]);
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
