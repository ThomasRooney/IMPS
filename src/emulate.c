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
int const int_conversion = 4;

void doOpCode_HALT(instruction * args, state * state)
{}

void doOpCode_ADD (instruction * args, state * state) {
  state.reg[args.RTypeInstruction.dstRegIndex] = 
  state.reg[args.RTypeInstruction.src1RegIndex] + state.reg[args.RTypeInstruction.src2RegIndex];
}

void doOpCode_ADDI(instruction * args, state * state) {
  state.reg[args.ITypeInstruction.dstRegIndex] =
  state.reg[args.ITypeInstruction.srcRegIndex] + args.ITypeInstruction.intermediateValue; //considering we can operate on short to long
}

void doOpCode_SUB (instruction * args, state * state) {
  state.reg[args.RTypeInstruction.dstRegIndex] =
  state.reg[args.RTypeInstruction.src1Regindex] - state.reg[args.Rtypeinstruction.src2RegIndex];
}

void doOpCode_SUBI(instruction * args, state * state) {
  state.reg[args.ITypeInstruction.dstRegIndex] = 
  state.reg[args.ITypeInstruction.srcRegIndex] - args.ITypeInstruction.intermediateValue; //considering we can operate on short to long
}

void doOpCode_MUL (instruction * args, state * state) {
  state.reg[args.RTypeInstruction.dstRegIndex] = 
  state.reg[args.RTypeInstruction.src1RegIndex] * state.reg[args.RTypeInstruction.src2RegIndex];
}

void doOpCode_MULI(instruction * args, state * state) {
  state.reg[args.ITypeInstruction.dstRegIndex] = 
  state.reg[args.ITypeInstruction.srcRegIndex] * args.ITypeInstruction.intermediateValue; //considering we can operate on short to long
}

void doOpCode_LW  (instruction * args, state * state) {
  state.reg[args.ITypeInstruction.dstRegIndex] = 
  state.MEMORY[state.reg[args.ITypeInstruction.srcRegIndex] + intermediateValue]];
}

void doOpCode_SW  (instruction * args, state * state) {
  state.MEMORY[state.reg[args.ITypeInstruction.srcRegIndex] + intermediateValue]] =
  state.reg[args.ITypeInstruction.dstRegindex];
}

void doOpCode_BEQ (instruction * args, state * state) {
  if (state.reg[args.ITypeInstruction.dstRegIndex] == state.reg[args.ITypeInstruction.srcRegIndex]) {
    programCount += (args.ITypeInstruction.intermediateValue * int_conversion); 
  }
}

void doOpCode_BNE (instruction * args, state * state) {
  if (state.reg[args.ITypeInstruction.dstRegIndex] != state.reg[args.ITypeInstruction.srcRegIndex]) {
    programCount += (args.ITypeInstruction.intermediateValue * int_conversion); 
  }
}

void doOpCode_BLT (instruction * args, state * state) {
  if (state.reg[args.ITypeInstruction.dstRegIndex] < state.reg[args.ITypeInstruction.srcRegIndex]) {
    programCount += (args.ITypeInstruction.intermediateValue * int_conversion); 
  }
}

void doOpCode_BGT (instruction * args, state * state) {
  if (state.reg[args.ITypeInstruction.dstRegIndex] > state.reg[args.ITypeInstruction.srcRegIndex]) {
    programCount += (args.ITypeInstruction.intermediateValue * int_conversion); 
  }
}

void doOpCode_BLE (instruction * args, state * state) {
  if (state.reg[args.ITypeInstruction.dstRegIndex] <= state.reg[args.ITypeInstruction.srcRegIndex]) {
    programCount += (args.ITypeInstruction.intermediateValue * int_conversion); 
  }
}

void doOpCode_BGE (instruction * args, state * state) {
  if (state.reg[args.ITypeInstruction.dstRegIndex] >= state.reg[args.ITypeInstruction.srcRegIndex]) {
    programCount += (args.ITypeInstruction.intermediateValue * int_conversion); 
  }
}

void doOpCode_JMP (instruction * args, state * state) {
  programCounter = args.JTypeInstruction.immediateAddress;
}

void doOpCode_JR  (instruction * args, state * state) {
  programCounter = args.RTypeInstruction.dstRegIndex;
}

void doOpCode_JAL (instruction * args, state * state) {
  
}

void doOpCode_OUT (instruction * args, state * state) {
  printf("%i", args.reg[RtypeInstruction.srcRegIndex]);


void printBinaryInstruction(binaryInstruction instruction) {
	int i;
	for (i = 0; i < 32; ++i) { printf("%i",(instruction >> i) & 1); }
	printf("\n");
}

int instructionFromOpcode(opCode opCode) {
	if (opCode < opCodeDefinition.NUMBER_OF_OPCODES) {
		return instructionType [opCode];
	}
	return INSTRUCTION_TYPE_UNKNOWN;
}

instruction disassembleInstruction(binaryInstruction binInstruction) {
	instruction outputInstruction;
	opCode opCode;
	// First 5 bits of any instruction is the opCode
	opCode = readBitField(binInstruction,0, 5);
	// Work out the type of the instruction, and thus data formatting
	int instructionType = instructionFromOpcode(opCode);
	if (instructionType == INSTRUCTION_TYPE_UNKNOWN) {
		printf("Fatal Error: Unknown Instruction Type Read");
		exit(EXIT_FAILURE); 
	}
	switch(instructionType) {
		case INSTRUCTION_TYPE_R:
			outputInstruction.rType.R1 = (short)readBitField(binInstruction,6, 10);
			outputInstruction.rType.R2 = (short)readBitField(binInstruction,11, 15);
			outputInstruction.rType.R3 = (short)readBitField(binInstruction,16, 20);
			break;
		case INSTRUCTION_TYPE_I:
			outputInstruction.iType.R1 = (short)readBitField(binInstruction,6, 10);
			outputInstruction.iType.R2 = (short)readBitField(binInstruction,11, 15);
			outputInstruction.iType.immediateValue = (short)readBitField(binInstruction,16, 31);
			break;
		case INSTRUCTION_TYPE_J:
			outputInstruction.jType.address = (short)readBitField(binInstruction,6, 10);
			break;
			
	}
	long binInstructionAfterOpCode = (binInstruction<<5);
	memcpy(&(outputInstruction.rType), &binInstructionAfterOpCode, sizeof(long));
	return outputInstruction;
}


// Inclusive
long readBitField(binaryInstruction bin,unsigned char start, unsigned char end, long out)
{
	long mask = pow(2, end-start)-1>>start;
	return bin & mask;
}

void dump_state(state curState) {
	fprintf(stderr, "PROGRAM_HALTED\n--------------\n\
					ProgramCounter: %i\n", state.programCounter);
	for (int i = 0; i < MAX_REGISTERS; i++) {
		fprintf(stderr, "Register[%i]: ", i, state.reg[i]);
	}
}

//TODO: Implement a state and the map between instruction and function
void emulation_loop(const char * inputBuffer, int inputLength) {
	instruction instruction;
	binaryInstruction binaryInstruction;
	state state = initialise_state()
	state stateOld;
	// Whilst state is changing every iteration
	do {
		memcpy(&stateOld, &state, sizeof(state));
		memcpy(&binaryInstruction, inputBuffer+state.programCounter, 4);
		printBinaryInstruction(binaryInstruction);
		instruction = disassembleInstruction(binaryInstruction);
		opCodeDictionary[instruction.opCode](&instruction, &state);
	} while (memcmp(&state,&stateold,sizeof(state) != 0)
	// Dump PC and registers into stderr
	dump_state(state);
}

state initialise_state(void) { 
	state virtualState;
	memset(virtualState.reg, 0, sizeof(virtualState.reg));
	state.programCounter = 0;
	virtualState.MEMORY = calloc(MEMORY_SIZE, sizeof(long));
	// allocate 65535 addresses with 32 bit boundary.
	return virtualState;
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
