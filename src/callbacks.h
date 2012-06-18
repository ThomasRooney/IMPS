#ifndef COMMON_HEADER_PARSED
#include "common.h"
#endif

#ifdef superoptimiser
int memoryActiveLoc[MEMORY_SIZE];
int bestActiveLoc[MEMORY_SIZE];
void addMemoryActiveLoc(unsigned int memLoc)
{
	int i;
	for (i = 0;memoryActiveLoc[i] != -1; i++)
	{
		if (memoryActiveLoc[i] == memLoc)
		{
			return;
		}
	}
	memoryActiveLoc[i] = memLoc;
}
int getSizeMemLoc(){
	int i;
	for (i=0;memoryActiveLoc[i]!=-1;i++);
	return (i>=MEMORY_SIZE-1?MEMORY_SIZE-1:i);
}

short getRandMemoryLoc(char opCode)
{
	int newI;
	int size = getSizeMemLoc();
	if (opCode == 7) // return an active Loc
		return memoryActiveLoc[rand() % (size + 1)];
	else
	{
		newI = rand() % MEMORY_SIZE;
		memoryActiveLoc[size] = newI;
		return newI;
	}
}
#endif


/**************************************************************************
* Callback functions for opcode operations
*/


// no state change will call the termination in the emulation loop
int doOpCode_HALT(instruction * args, state * state) 
{
	return STATE_HALT;
}

int doOpCode_ADD (instruction * args, state * state) {
  state->reg[args->rType.R1] =
  state->reg[args->rType.R2] + state->reg[args->rType.R3];
  return STATE_INCREMENTPC;
}

int doOpCode_ADDI(instruction * args, state * state) {
  state->reg[args->iType.R1] =
  state->reg[args->iType.R2] + args->iType.immediateValue; 
  return STATE_INCREMENTPC;
}

int doOpCode_SUB (instruction * args, state * state) {
  state->reg[args->rType.R1] =
  state->reg[args->rType.R2] - state->reg[args->rType.R3];
  return STATE_INCREMENTPC;
}

int doOpCode_SUBI(instruction * args, state * state) {
  state->reg[args->iType.R1] =
  state->reg[args->iType.R2] - args->iType.immediateValue; 
  return STATE_INCREMENTPC;
}

int doOpCode_MUL (instruction * args, state * state) {
  state->reg[args->rType.R1] =
  state->reg[args->rType.R2] * state->reg[args->rType.R3];
  return STATE_INCREMENTPC;
}

int doOpCode_MULI(instruction * args, state * state) {
  state->reg[args->iType.R1] =
  state->reg[args->iType.R2] * args->iType.immediateValue; 
  return STATE_INCREMENTPC;
}

int doOpCode_LW  (instruction * args, state * state) {
  // As below
  char so[kDisplayWidth+1];
  unsigned calc = state->reg[args->iType.R2] + args->iType.immediateValue;
  if (calc > MEMORY_SIZE || calc < 0)
    return STATE_ERROR;
  else { 
#ifdef superoptimiser
	addMemoryActiveLoc(calc);
#endif
    memcpy(&state->reg[args->iType.R1], &state->MEMORY[calc], sizeof(instruction));
  }

  if (main_args.verbose) 
  {
    printf("  M[%i]=%s\n", calc, pBin(*(unsigned int *)&(state->MEMORY[calc]),so));
    printf("  LW: Memory[%i]: %i to Register[%i]\n", calc, *(unsigned int *)&state->MEMORY[calc], args->iType.R1);
  }
  return STATE_INCREMENTPC;
}

int doOpCode_SW  (instruction * args, state * state) {
  // To make this bulletproof, we need to make sure that the memory accessed exists. 
  // If not, we're returning a STATE_ERROR
  unsigned calc =  state->reg[args->iType.R2] + args->iType.immediateValue;
  if (calc > MEMORY_SIZE || calc < 0)
    return STATE_ERROR;
  else {
    memcpy(&state->MEMORY[calc], &state->reg[args->iType.R1], sizeof(instruction));
    state->MEMORY[calc] = state->reg[args->iType.R1];
  }
  return STATE_INCREMENTPC;
}

int doOpCode_BEQ (instruction * args, state * state) {
  if (state->reg[args->iType.R1] == state->reg[args->iType.R2]) {
    state->programCounter += (args->iType.immediateValue * PC_BOUNDARY);
	return STATE_CONTINUE;
  }
  return STATE_INCREMENTPC;
}

int doOpCode_BNE (instruction * args, state * state) {
  if (state->reg[args->iType.R1] != state->reg[args->iType.R2]) {
    state->programCounter += (args->iType.immediateValue * PC_BOUNDARY);
	return STATE_CONTINUE;
  }
  return STATE_INCREMENTPC;
}

int doOpCode_BLT (instruction * args, state * state) {
  if (state->reg[args->iType.R1] < state->reg[args->iType.R2]) {
    state->programCounter += (args->iType.immediateValue * PC_BOUNDARY);
    return STATE_CONTINUE;
  }
  return STATE_INCREMENTPC;
}

int doOpCode_BGT (instruction * args, state * state) {
  if (state->reg[args->iType.R1] > state->reg[args->iType.R2]) {
    state->programCounter += (args->iType.immediateValue * PC_BOUNDARY);
    return STATE_CONTINUE;
  }
  return STATE_INCREMENTPC;
}

int doOpCode_BLE (instruction * args, state * state) {
  if (state->reg[args->iType.R1] <= state->reg[args->iType.R2]) {
    state->programCounter += (args->iType.immediateValue * PC_BOUNDARY);
    return STATE_CONTINUE;
  }
  return STATE_INCREMENTPC;
}

int doOpCode_BGE (instruction * args, state * state) {
  if (state->reg[args->iType.R1] >= state->reg[args->iType.R2]) {
    state->programCounter += (args->iType.immediateValue * PC_BOUNDARY);
    return STATE_CONTINUE;
  }
  return STATE_INCREMENTPC;
}

int doOpCode_JMP (instruction * args, state * state) {
  state->programCounter = args->jType.address;
  return STATE_CONTINUE;
}

int doOpCode_JR  (instruction * args, state * state) {
  state->programCounter = state->reg[args->rType.R1];
  return STATE_CONTINUE;
}

int doOpCode_JAL (instruction * args, state * state) {
  state->reg[31] = state->programCounter + PC_BOUNDARY;
  state->programCounter = args->jType.address;
  return STATE_CONTINUE;
}

int doOpCode_OUT (instruction * args, state * state) {
#ifndef superoptimiser
  printf("%c", state->reg[args->rType.R1]);
#else
  sprintf(progOut, "%c", state->reg[args->rType.R1]);
#endif
  return STATE_INCREMENTPC;
}
int doOpCode_NOP(instruction * args, state * state)
{
	return STATE_INCREMENTPC;
}

/**************************************************************
 * Map between Opcode and a function pointer for that operation
 */
const opCodeDef       opCodeDefinition;
void * opCodeDictionary [NUMBER_OF_OPCODES] =
{
	&doOpCode_HALT,
	&doOpCode_ADD ,
	&doOpCode_ADDI,
	&doOpCode_SUB ,
	&doOpCode_SUBI,
	&doOpCode_MUL ,
	&doOpCode_MULI,
	&doOpCode_LW  ,
	&doOpCode_SW  ,
	&doOpCode_BEQ ,
	&doOpCode_BNE ,
	&doOpCode_BLT ,
	&doOpCode_BGT ,
	&doOpCode_BLE ,
	&doOpCode_BGE ,
	&doOpCode_JMP ,
	&doOpCode_JR  ,
	&doOpCode_JAL ,
	&doOpCode_OUT ,
	&doOpCode_NOP
};

