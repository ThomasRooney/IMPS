#ifndef COMMON_HEADER_PARSED
#include "common.h"
#endif


/**************************************************************
 * Function Pointers for Opcode emulation
 */

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

