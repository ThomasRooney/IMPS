#ifndef COMMON_HEADER_PARSED
#include "common.h"
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
  state->reg[args->iType.R1] =
  state->MEMORY[state->reg[args->iType.R2] + args->iType.immediateValue];
  return STATE_INCREMENTPC;
}

int doOpCode_SW  (instruction * args, state * state) {
  state->MEMORY[state->reg[args->iType.R2] + args->iType.immediateValue] =
  state->reg[args->iType.R1];
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
  printf("%c", state->reg[args->rType.R1]);
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
	&doOpCode_OUT 
};

