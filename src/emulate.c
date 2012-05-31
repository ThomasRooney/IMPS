#include "common.h"
#include "callbacks.h"



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

instruction disassembleInstruction(binaryInstruction binInstruction) {
	instruction outputInstruction;
	memcpy(&outputInstruction, &binInstruction, sizeof(binaryInstruction));
	return outputInstruction;
}

void dump_state(state *curState) {
	int i;
	fprintf(stderr, "Program State\n--------------\
					\n  ProgramCounter: %i\n", curState->programCounter);
	for (i = 0; i < MAX_REGISTERS; i++) {
		fprintf(stderr, "    Register[%i]: %i\n", i, curState->reg[i]);
	}
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

state *initialise_state(const char * inputBuffer, const int inputLength) {
	state * virtualState = malloc(sizeof(state));
	int i = 0;
	char so[kDisplayWidth+1];
	memset(virtualState->reg, 0, sizeof(virtualState->reg));
	virtualState->programCounter = 0;
	virtualState->MEMORY = calloc(MEMORY_SIZE, sizeof(char));
	// allocate 65535 addresses with 32 bit boundary.	
	memcpy((virtualState->MEMORY), inputBuffer, inputLength);
	if (main_args.verbose) {
		printf("Moving program data to MEMORY: First 15 values are..\n");
		for (i=0; i <= 15; i++)
		{
			printf("  M[%i]=%s\n", 4*i, pBin(*(unsigned int *)&(virtualState->MEMORY[i*4]),so));
		}
	}	
	return virtualState;
}

void emulation_loop(state *programState) {
	instruction parsedInstruction;
	binaryInstruction binaryInstruction;
	stateSignal (*opCodeFunction)(instruction *, state *);
	while ( programState->programCounter >= 0 )
	{
		memcpy(&binaryInstruction, programState->MEMORY+programState->programCounter, sizeof(binaryInstruction));
		parsedInstruction = disassembleInstruction(binaryInstruction);
		if (main_args.verbose == 1) {
			printf("Current State..\n");
			dump_state(programState);
			dump_instruction(parsedInstruction);
			 
		}
		opCodeFunction = (opCodeDictionary[parsedInstruction.raw.opCode]);
		switch (opCodeFunction(&parsedInstruction, programState))
		{
			case STATE_CONTINUE:
				break;
			case STATE_HALT:
				programState->programCounter = -1; 
				break;
			case STATE_INCREMENTPC:
				programState->programCounter += PC_BOUNDARY;
				break;
		}
		if (main_args.step) {
			while (1)
			{
				if ('\n'==getchar())
					break;
			}
		}
	} 
	
	// Dump PC and registers into stderr
	fprintf(stderr, "\nHALT!\n\n");
	dump_state(programState);	
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
			if(strcmp(argv[iter],"-s") == 0) {
					main_args.step = 1;
					main_args.verbose = 1;
					printf("Step Through Debugging Mode on\n");
			}
			else
			{
				printf("ERROR: Unknown Argument: %s\n", argv[iter]);
			}}
		}
	}

	if (readFile(main_args.file_name, inputLength, &inputBuffer)>EXIT_SUCCESS) {
		return FATAL_ERROR;
	}
	if (main_args.verbose)
		printf("File Read Success. Length is %i bits\n", (*inputLength)*32);
	state *virtualState = initialise_state(inputBuffer, *inputLength);
	
	emulation_loop(virtualState);
	free(inputLength);
	free(virtualState);
    return EXIT_SUCCESS;
}
