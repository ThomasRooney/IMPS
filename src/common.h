#define FATAL_ERROR 10
#define MEMORY_SIZE 65535
#define MAX_REGISTERS 32
#define PC_BOUNDARY 4

typedef unsigned char opCode;
typedef unsigned int registerIndex;
typedef unsigned int address;

/**************************************************************
 * Utility
 */
char* pBin(long int x, char *so);                   // version without fill
#define kDisplayWidth 36
char* pBin(long x,char *so)
{
 char s[kDisplayWidth+1];
 int i=kDisplayWidth;
 s[i--]=0x00;   // terminate string
 do
 { // fill in array from right to left
  s[i--]=!(i%9)?' ':(x>>=1)&&(x&1) ? '1':'0';  // determine bit, shift, add in spaceing
 } while( i > 0); 
 i++;   // point to last valid character
 sprintf(so,"%s",s+i); // stick it in the temp string string
 return so;
}

/**************************************************************
 * Instructions
 */

#pragma pack(1)
typedef struct {
	opCode opCode:6;
	registerIndex R1:5;  // destination register index
	registerIndex R2:5; // first source register index
	registerIndex R3:5; // second source register index
}RTypeInstruction;


#pragma pack(1)
typedef struct {
	opCode opCode:6;
	registerIndex R1:5;       // destination register index
	registerIndex R2:5;       // source register index
	unsigned short immediateValue:16; // 16 bits for the intermediate value
}ITypeInstruction;


#pragma pack(1)
typedef struct {;
	opCode opCode:6;
	 address address:26;
}JTypeInstruction;

#pragma pack(1)
typedef struct {

	// The union causes padding between bitfields
	// This causes sizeof(instruction) == 5
	union {
		RTypeInstruction rType;
		ITypeInstruction iType;
		JTypeInstruction jType;
		/*struct {
			registerIndex R1:5;  // destination register index
			registerIndex R2:5; // first source register index
			registerIndex R3:5; // second source register index
			unsigned :11; // 11 bytes of padding to make the struct 26 bits
		}rType;
		struct {
			registerIndex R1:5;       // destination register index
			registerIndex R2:5;       // source register index
			unsigned short immediateValue:16; // 16 bits for the intermediate value
		}iType;
		struct {;
			 address address:26;
		}jType;*/
		opCode opCode : 6;
	};
}instruction;

typedef struct {
	char * file_name;
	int verbose;
}arguments;

// Every instruction is 32 bits long, or 4 bytes
typedef unsigned int binaryInstruction;

arguments main_args;


// A char is 8 bits
// a int is 32 bits

/**************************************************************
 * OpCode Definitions
 */
typedef enum opCodeDef {
	OPCODE_HALT = 0,
	OPCODE_ADD = 1,
	OPCODE_ADDI = 2,
	OPCODE_SUB = 3,
	OPCODE_SUBI = 4,
	OPCODE_MUL = 5,
	OPCODE_MULI = 6,
	OPCODE_LW = 7,
	OPCODE_SW = 8,
	OPCODE_BEQ = 9,
	OPCODE_BNE = 10,
	OPCODE_BLT = 11,
	OPCODE_BGT = 12,
	OPCODE_BLE = 13,
	OPCODE_BGE = 14,
	OPCODE_JMP = 15,
	OPCODE_JR = 16,
	OPCODE_JAL = 17,
	OPCODE_OUT = 18,
	NUMBER_OF_OPCODES = 19
}opCodeDef;


/**************************************************************
 * Instruction Type Definitions
 */
#define INSTRUCTION_TYPE_UNKNOWN   0
#define INSTRUCTION_TYPE_R         1
#define INSTRUCTION_TYPE_I         2
#define INSTRUCTION_TYPE_J         3
#define INSTRUCTION_TYPE_NA        4
 
 
const int instructionType [NUMBER_OF_OPCODES] = {
	INSTRUCTION_TYPE_NA, 
	INSTRUCTION_TYPE_R,
	INSTRUCTION_TYPE_I,
	INSTRUCTION_TYPE_R,
	INSTRUCTION_TYPE_I,
	INSTRUCTION_TYPE_R,
	INSTRUCTION_TYPE_I,
	INSTRUCTION_TYPE_I,
	INSTRUCTION_TYPE_I,
	INSTRUCTION_TYPE_I,
	INSTRUCTION_TYPE_I,
	INSTRUCTION_TYPE_I,
	INSTRUCTION_TYPE_I,
	INSTRUCTION_TYPE_I,
	INSTRUCTION_TYPE_I,
	INSTRUCTION_TYPE_J,
	INSTRUCTION_TYPE_R,
	INSTRUCTION_TYPE_J,
	INSTRUCTION_TYPE_R
};


/**************************************************************
 * State information containing registers for emulation
 */


typedef struct { 
  unsigned int reg [32];
  int * MEMORY;
  unsigned int programCounter;
}state;
 
 



/**************************************************************
 * Function Pointers for Opcode emulation
 */

// TODO: Implement all
// TODO: work out a decent state structure, and change "state"
void doOpCode_HALT(instruction * args, state * state);
void doOpCode_ADD (instruction * args, state * state);
void doOpCode_ADDI(instruction * args, state * state);
void doOpCode_SUB (instruction * args, state * state);
void doOpCode_SUBI(instruction * args, state * state);
void doOpCode_MUL (instruction * args, state * state);
void doOpCode_MULI(instruction * args, state * state);
void doOpCode_LW  (instruction * args, state * state);
void doOpCode_SW  (instruction * args, state * state);
void doOpCode_BEQ (instruction * args, state * state);
void doOpCode_BNE (instruction * args, state * state);
void doOpCode_BLT (instruction * args, state * state);
void doOpCode_BGT (instruction * args, state * state);
void doOpCode_BLE (instruction * args, state * state);
void doOpCode_BGE (instruction * args, state * state);
void doOpCode_JMP (instruction * args, state * state);
void doOpCode_JR  (instruction * args, state * state);
void doOpCode_JAL (instruction * args, state * state);
void doOpCode_OUT (instruction * args, state * state);

/**************************************************************
 * Map between Opcode, Instruction type, and a function pointer
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



