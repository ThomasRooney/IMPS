#include "common.h"
#include "callbacks.h"

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
		memcpy(&binaryInstruction, programState->MEMORY+programState->programCounter, 
				sizeof(binaryInstruction));
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

int main(int argc, char **argv) {
	char * inputBuffer;
	int  * inputLength = malloc(sizeof(int)); // Kept on heap due to non-local use

    // populate main_args
	parseArguments(argc, argv);
	
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
