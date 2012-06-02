#include "common.h"


typedef struct instructionLL{
	instruction cur;
	struct instructionLL *next;
}instructionLL;

typedef struct {
	unsigned int position;
	char * keyName;
}label;

typedef struct next{
	label cur;
	struct labelLL * next;
}labelLL;

typedef struct { 
	instructionLL instructions;
	labelLL labels;
}tokAssembler;

typedef struct symbolsLL{
	char * symbol;
	struct symbolsLL * next;
}symbolsLL;

typedef struct lineLL{
	char * line;
	symbolsLL * symbolsHEAD;
	struct lineLL * next;
}lineLL;

const char * opCodeMapStr[NUMBER_OF_OPCODES] = {
	"halt",
	"add",
	"addi",
	"sub",
	"subi",
	"mul",
	"muli",
	"lw",
	"sw",
	"beq",
	"bne",
	"blt",
	"bgt",
	"ble",
	"bge",
	"jmp",
	"jr",	
	"jal",
	"out"
};

int strToOpCode(char * strOpCode) {
	int i;
	for (i=0; i < NUMBER_OF_OPCODES; i++) {
		if (strcmp(opCodeMapStr[i], strOpCode) == 0) {
			return i;
		}
	}
	return GENERIC_ERROR;
}

int extractRegister(char * registerToken) {
	int out = -1;
	if (registerToken[0] == '$') 
	{
		// Chop off first character
		registerToken = &registerToken[1];
		out = atoi(registerToken);
	}
	if (out > 31 || out < 0)
	{
		printf("FATAL ERROR. Register Token: \"%s\" wasn't as expected\n", registerToken);
		exit(EXIT_FAILURE);
	}
	return out;
}


