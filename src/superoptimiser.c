#define superoptimiser

// This program performs Levin Search on a given input file (up to a maximum level anyway), and attempts to find the superoptimised version. 
// The input file must be within in the IMPS format and run correctly with the emulator.
// This will take a *very* long time, as it works exponentially with program length.





#define ARGS 1
#define MAXLEVEL 4
#define nCARRYSOL
#define CONST0 0
#define CONST1 1
#define CONST2 255
#define RECURSION_LIMIT 100
#define START_ARG 0
#define END_ARG 255
#define TRY_NUMBER 5
#define RANDSAMP 16
#define MAX_PROG_LENGTH 100
#define MAX_OUTPUT 1000
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <time.h>
char progOut[MAX_OUTPUT];

#include "common.h"
#include "callbacks.h"
int bestMemoryActiveLoc[MEMORY_SIZE]; // don't read outside this

// #ifdef WIN32

// #include <windows.h>
// double get_time()
// {
    // LARGE_INTEGER t, f;
    // QueryPerformanceCounter(&t);
    // QueryPerformanceFrequency(&f);
    // return double(t.QuadPart)/double(f.QuadPart);
// }

// #else

// #include <sys/times.h>
// #include <sys/resource.h>
typedef unsigned long long ticks;

static __inline__ ticks getticks(void)
{
     unsigned a, d;
     asm("cpuid");
     asm volatile("rdtsc" : "=a" (a), "=d" (d));

     return (((ticks)a) | (((ticks)d) << 32));
}

unsigned long long get_time()
{
    //struct tms t;
    //times(&t);
    //return t.tms_stime;
	return getticks();
}
// #endif


int measure_type=2;

// For the goal to be changed, out must be equal, but time must be less.
typedef struct { 
	char * out;
	unsigned long long time;
}goal;

// contains the best program found for the current output.
typedef struct {
	goal goal;
	char * program;
	int programLength;
}bestProgram;

unsigned int randRegister() { 
	return rand() % 32;
}

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

state *initialise_state(state * old, const char * inputBuffer, const int inputLength) {
        state * virtualState;
	if (old == NULL) {
		virtualState = malloc(sizeof(state));
		virtualState->MEMORY = calloc(MEMORY_SIZE, sizeof(char));
		memset(&memoryActiveLoc, -1, sizeof(int) * MEMORY_SIZE);
	}
	else {
		virtualState = old;
		memset(virtualState->MEMORY, 0, MEMORY_SIZE);
		memcpy(&memoryActiveLoc, &bestActiveLoc, sizeof(int) * MEMORY_SIZE);
	}
	
	int i = 0;
	char so[kDisplayWidth+1];
	memset(virtualState->reg, 0, sizeof(virtualState->reg));
	virtualState->programCounter = 0;

	// allocate 65535 addresses with 32 bit boundary.	
	memcpy((virtualState->MEMORY), inputBuffer, inputLength);
	/*if (main_args.verbose) {
		printf("Moving program data to MEMORY: First 15 values are..\n");
		for (i=0; i <= 15; i++)
		{
			printf("  M[%i]=%s\n", 4*i, pBin(*(unsigned int *)&(virtualState->MEMORY[i*4]),so));
		}
	}	*/
	return virtualState;
}

/**********************************************************
 * This function runs through the program, disassembling the 
 * instructions, and then passing them to the appropriate 
 * callback, via a function pointer, to modify the state as in 
 * the specification for each opcode.
 */
goal *emulation_loop(state *programState, goal *curGoal, goal *progScore) {
	instruction parsedInstruction;
	binaryInstruction binaryInstruction;
	stateSignal (*opCodeFunction)(instruction *, state *);
	unsigned long long iTime;
	int iter;
	// start timer
	iter = 0;
	memset(&progOut, 0, sizeof(progOut));
	iTime = get_time(); 
	progScore->time = (iTime - get_time());
	while ( programState->programCounter >= 0 && programState->programCounter <= MAX_PROG_LENGTH)
	{
		iter++;
		if (iter % 100 == 0)
		{
			// add the difference between progScore->time and get_time
			progScore->time = abs(iTime - get_time());
			if (curGoal != NULL && curGoal->time < progScore->time)
			{
				return NULL; // We haven't met requirements
			}
		}
		memcpy(&binaryInstruction, programState->MEMORY+programState->programCounter, 
				sizeof(binaryInstruction));

		parsedInstruction = disassembleInstruction(binaryInstruction);
		if (parsedInstruction.raw.opCode >= 19 || parsedInstruction.raw.opCode < 0 )
			return NULL;
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
			default:
				return NULL;
		}		
	} 
	// return our score to update the bestProgram
	progScore->time = abs(iTime - get_time());
	memcpy(progScore->out, progOut, sizeof(progOut));
	
	return progScore;
}


typedef struct stack {
  instruction * prog;
  struct stack *next;
  int progLength;
}stack;

unsigned char regs[8],carry;
stack *base;
int current;


stack * breakOutFuncs(instruction * fst, int level, int max)
{
	printf("Breaking out insn: ");
	dump_instruction(*fst);
	stack * col;
	instruction *cur;
	if (level > max)
		return NULL;
	for (cur = fst; 
	cur != NULL &&
	(!(instructionFromOpcode(fst->raw.opCode) == INSTRUCTION_TYPE_J || (fst->raw.opCode <= 14 && fst->raw.opCode >= 9)));
	cur = (instruction *)((void *)fst + (sizeof(instruction) * level++))
	);	
	if (fst == cur || fst == NULL)
		// There is no function we can find
		return breakOutFuncs(fst, level++, max);
	//fst - cur is now size of function
	col = malloc(sizeof(stack));
	col->prog = cur;
	col->progLength += level*sizeof(instruction);
	
	printf("col Length = %i\n", col->progLength);
	return col;
}

instruction rand_valid_instruct(stack * p)
{
	instruction out;
	memset(&out, 0, sizeof(out));
	// rand opcode
	out.raw.opCode = rand() % NUMBER_OF_OPCODES;
	
	switch (instructionFromOpcode(out.raw.opCode))
	{
		case INSTRUCTION_TYPE_NA: 
			return out;
		case INSTRUCTION_TYPE_R:
			out.rType.R1 = randRegister();
			out.rType.R2 = randRegister();
			out.rType.R3 = randRegister();
			return out;
		case INSTRUCTION_TYPE_I:
			out.iType.R1 = randRegister();
			out.iType.R2 = randRegister();
			out.iType.immediateValue = getRandMemoryLoc(out.raw.opCode);
			return out;
		case INSTRUCTION_TYPE_J:
			out.jType.address = rand() % p->progLength;
			return out;
	}
	return out;
}

void random_evolve_prog(stack * p)
{
	srand ( time(NULL) ); // Initialise Random
	int i;
	int r;
	instruction randInstruction;
	// rand = how many instructions to modify.
	r = (rand() + 1) % p->progLength;
	
	for (i = 0; i < r; i++)
	{
		randInstruction = rand_valid_instruct(p);
		memcpy(p->prog+(rand()*4)%p->progLength - 1, &randInstruction, sizeof(instruction));
	}
}

stack * clever_evolve_prog(bestProgram *p)
{
	stack *newP = NULL;
	srand ( time(NULL) ); // Initialise Random	
	
	// Get a function
	stack * func = breakOutFuncs((instruction *)p->program, 0, p->programLength);
	if (func == NULL)
	{
		return NULL;
	}
	// Find a faster version
	// ???
	// Profit
	// get in memory loc for p's out
	return newP;
}
/*
void enumerate_prog(bestProgram * p)
{	
	int i;
	int r;
	instruction randInstruction;
	// rand = how many instructions to modify.
	r = (rand() + 1) % p->progLength;
	
	for (i = 0; i < r; i++)
	{
		randInstruction = next_instruction(p, 0);
		memcpy(p->prog+rand()%p->progLength - 1, &randInstruction, sizeof(instruction));
	}
}
*/

int main (int argc, char **argv) {

	stack p;
	char * inputBuffer;
	int improv = 0;
	int failures = 0;
	int  * inputLength = malloc(sizeof(int)); // Kept on heap due to non-local use
	int iter;
	int i2, i3;
	unsigned long long average;
	unsigned long long t1, t2;
	// This is where the opcodeOUT output gets channelled
	memset(&progOut, 0, sizeof(MAX_OUTPUT));
	// TEMP: test tick function
	t1 = get_time();
	t2 = get_time();
	printf("Two insns, one after another has time %llu\n", t2-t1);
	// populate main_args
	parseArguments(argc, argv);
	goal *score;
	bestProgram best;
	// Set up heap for scoring
	score = malloc(sizeof(goal));
	score->out = malloc(sizeof(progOut));
	best.goal.out = malloc(sizeof(progOut));

	if (readFile(main_args.file_name, inputLength, &inputBuffer)>EXIT_SUCCESS) {	
		return FATAL_ERROR;
	}
	if (main_args.verbose)
		printf("File Read Success. Length is %i bits\n", (*inputLength)*32);
		
	// This virtualState, when run by emulation_loop, gives us our      scoring function, alongside time taken
	state *virtualState = initialise_state(NULL, inputBuffer, *inputLength);
	emulation_loop(virtualState, NULL, score);
		printf("scoring function: \n");
		printf("goal(time) = %llu\n", score->time);
		printf("goal(out) = %s\n", score->out);
	if (score == NULL)
	{
		printf("Scoring Function Flawed\n");
	}
	if (main_args.step)
		waitUntilEnter();
	
	best.goal.time = score->time;
	memcpy(best.goal.out, score->out, sizeof(progOut));
	best.program = inputBuffer;
	best.programLength = *inputLength;

	p.prog = malloc(best.programLength);
	memcpy(p.prog, best.program, best.programLength);
	p.progLength = best.programLength;
	// try not to use more memory
	memcpy(&bestActiveLoc, &memoryActiveLoc, MEMORY_SIZE * sizeof(int));
	// Use best as breeding stock
	unsigned int i = 0;
	// if we've done this maybe a few hundred times then give up:
	for (iter = 0;iter <= 1; iter++)
	{
		// iterate function (defined as blocks of non-jumping  consequitive code) from best
		while ( 1 )
		{
			// evolve p->prog randomly
			random_evolve_prog(&p);
			virtualState = initialise_state(virtualState, (char *)p.prog, p.progLength);
 			if (emulation_loop (virtualState, &(best.goal), score) == NULL)
				continue;
			printf("out = %s; time = %llu  %s", score->out, score->time,(!(i++ % 9) ?"\n":""));

			// CASE:  True IMPROVEMENT!
			if (score != NULL && (strcmp(score->out, best.goal.out) == 0) && (score->time < best.goal.time)){
				// Test to make sure its not a statistical fluke
				for (i3 = 0; i3 < 100; i3++)
				{
					if (emulation_loop (virtualState, &(best.goal), score) == NULL)
						continue;
					average+=score->time;
				}
				average = average / 100;
				if (average <= best.goal.time)
					continue; // Fluke, continue
				// else its better
				best.goal.time = score->time;
				memcpy(best.goal.out, score->out, sizeof(progOut));
				best.program = (char *)p.prog;
				best.programLength = p.progLength;
				improv = 1;
				printf("\nImprovement Found\n");
				printf("    New time = %llu", score->time);
				printf("    New Out = %s", score->out);
				printf("\n New Program: \n");
				for (i2 = 0; i2 < p.progLength; i2++)
				{
					dump_instruction(*(instruction *)(best.program+i2*4));
				}
				failures = 0;
				break;
			}
			else {
 			} 
		}
	}
	
//	free(inputLength);
//	free(virtualState);
//	free(inputBuffer);
    return EXIT_SUCCESS;

}

	
	

