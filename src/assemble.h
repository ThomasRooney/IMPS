#include "common.h"




typedef struct instructionLL{
	instruction cur;
	struct instructionLL *next;
}instructionLL;

typedef struct {
	unsigned int position;
	char * keyName;
}label;

typedef struct labelLL{
	label labelKey;
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

typedef struct preAssemblyProgram {
	instruction curInstruction;
	struct preAssemblyProgram * next;
}preAssemblyProgram;

typedef struct { 
	instruction * memory;
	int length;
}assembledProgram;

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

/********************************************************************
 * Utility Function for extractValue
 **/
 // PRE: Definitely in the correct form: "0x..."
int strHexToDec(char * hex)
{
	// blah
	char * p;
	return strtol(hex,  &p, 16);
}

label *inLabelLL(char * symbol, labelLL * labelsHEAD) 
{
	labelLL *cur;
	for (cur = labelsHEAD; cur != NULL; cur = cur->next)
	{
		if (strcmp(symbol, cur->labelKey.keyName) == 0) {
			return &cur->labelKey;
		}
	}
	return NULL;
}

/********************************************************************
 * Utility Functions for symbolsToInstruction
 **/

int strToOpCode(char * strOpCode) {
	int i;
	for (i=0; i < NUMBER_OF_OPCODES; i++) {
		if (strcmp(opCodeMapStr[i], strOpCode) == 0) {
			return i;
		}
	}
	return GENERIC_ERROR;
}

 
int extractValue(char * symbol, labelLL *labels , int immediateValue) {
	int out = -1;
	label * labelBuf;
	labelBuf = inLabelLL(symbol, labels);
	if (labelBuf!=NULL) {
		out =  labelBuf->position;
		return out;
	}
	if (immediateValue) { // Case I immediateValue or J-Type Address
		// Make sure its not hex
		if (symbol[0] == '0' && symbol[1] == 'x') {
			out = strHexToDec(symbol);
		}
		else {
			out = atoi(symbol);
		}
	} else {
	if (symbol[0] == '$') //Case Register
	{
		// Chop off first character
		symbol = &symbol[1];
		out = atoi(symbol);
		if (out > 31 || out < 0)
		{
			printf("FATAL ERROR. Register Token: \"%s\" wasn't as expected\n", symbol);
			exit(EXIT_FAILURE);
		}
	} else { // Case: Label
		printf("FATAL ERROR. Label expected.. \"%s\" wasn't as expected\n", symbol);
		exit(EXIT_FAILURE);
	}}
	
	return out;
}

char * strtok2(const char * str, char *twochardelim)
{
	int working = 0;
	char * workingStr = malloc(255);
	strcpy(workingStr, str);
	int len = strlen(str);
	while (working++ < len)
	{
		if (workingStr[working - 1] == twochardelim[0] && workingStr[working] == twochardelim[1])
		{
			workingStr[working - 1] = '\0';
			break;
		}
	}
	return workingStr;
}
