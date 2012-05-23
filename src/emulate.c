#include "common.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// We want to read the file into memory so we can deal with it in a
// faster and easier way


void printBinaryInstruction(binaryInstruction instruction) {
	int i;
	for (i = 0; i < 8; ++i) { printf("%i",(instruction.w0 >> i) & 1); }
	for (i = 0; i < 8; ++i) { printf("%i",(instruction.w1 >> i) & 1); }
	for (i = 0; i < 8; ++i) { printf("%i",(instruction.w2 >> i) & 1); }
	for (i = 0; i < 8; ++i) { printf("%i",(instruction.w3 >> i) & 1); }
	printf("\n");
}

//TODO: Implement
int instructionFromOpcode(opCode opCode) {
	return INSTRUCTION_TYPE_UNKNOWN;
}


//TODO: Implement
instruction disassembleInstruction(binaryInstruction binInstruction) {
	instruction outputInstruction;
	opCode opCode;
	int instructionType = instructionFromOpcode(opCode);

	// First work out what kind of instruction it is


	return outputInstruction;
}

//TODO: Implement a state and the map between instruction and function
void emulation_loop(const char * inputBuffer, int inputLength) {
	unsigned int programCounter;
	instruction instruction;
	binaryInstruction binaryInstruction;
	for (programCounter=0; programCounter < inputLength;programCounter++ ) {
		memcpy(&binaryInstruction, inputBuffer+programCounter, 4);
		printBinaryInstruction(binaryInstruction);
		instruction = disassembleInstruction(binaryInstruction);

	}
}





int readFile(const char * fileName, int * outputLength, char * outputBuffer) {
	FILE *file;

	// Open file
	file = fopen(fileName, "rb");

	if (!file)
	{
		printf("FATAL ERROR: Unable to open file %s", fileName);
		return FATAL_ERROR;
	}

	// Get file length
	fseek(file, 0, SEEK_END);
	*outputLength=ftell(file);
	fseek(file, 0, SEEK_SET);

	// Allocate memory
	outputBuffer=(char *)malloc(*outputLength+1);
			//Note: it is the caller's requirement to free the memory
	if (!outputBuffer)
	{
		fprintf(stderr, "Memory error!");
                                fclose(file);
		return FATAL_ERROR;
	}

	// Read file contents into buffer
	fread(outputBuffer, *outputLength, 1, file);
	fclose(file);
	return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
	int iter;
	char * inputBuffer;
	int  * inputLength;

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

	if (readFile(main_args.file_name, inputLength, inputBuffer)>EXIT_SUCCESS) {
		return FATAL_ERROR;
	}
	printf("File Read Success. Length is %i bits\n", (*inputLength)*32);
	emulation_loop(inputBuffer, *inputLength);

    return EXIT_SUCCESS;
}
