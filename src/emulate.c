#include "common.h"
#include "callbacks.h"

#define EMULATE

/**********************************************************
 * This function simply copies the 32 bit instruction to the 
 * union which holds it. We don't need to worry about anything 
 * else, the bitfields hold the important information regarding 
 * the memory structure of an instruction
 */

instruction disassembleInstruction(binaryInstruction binInstruction) {
	instruction outputInstruction;
	memcpy(&outputInstruction, &binInstruction, sizeof(binaryInstruction));
	return outputInstruction;
}

/**********************************************************
 * Called upon program termination, or more if we're in verbose 
 * mode, this dumps the program state to stderr
 */

void dump_state(state *curState) {
	int i;
	fprintf(stderr, "Program State\n--------------\
					\n  ProgramCounter: %i\n", curState->programCounter);
	for (i = 0; i < MAX_REGISTERS; i++) {
		fprintf(stderr, "    Register[%i]: %i\n", i, curState->reg[i]);
	}
}


/**********************************************************
 * This initialises the program state, allocating memory on the 
 * heap and clearing all the registers and flags. The allocated 
 * memory is all initialised to 0.
 */
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


/**********************************************************
 * This function runs through the program, disassembling the 
 * instructions, and then passing them to the appropriate 
 * callback, via a function pointer, to modify the state as in 
 * the specification for each opcode.
 */
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
		if (main_args.step) 
			waitUntilEnter();
		
	} 
	
	// Dump PC and registers into stderr
	fprintf(stderr, "\nHALT!\n\n");
	dump_state(programState);	
}


/**********************************************************
 * The entry point to the program, this parses the arguments 
 * given to the program, and initialises the state after reading 
 * the file.
 */
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
	free(inputBuffer);
    return EXIT_SUCCESS;
}
