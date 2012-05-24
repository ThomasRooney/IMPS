#define FATAL_ERROR 10

typedef unsigned char opCode;        // 8 bits to emulate 6 bit field
typedef unsigned char registerIndex;  // 8 bits to emulate 4 bit field
typedef unsigned long address;    // 32 bits to emulate 26 bit field


/**************************************************************
 * Instructions
 */
typedef struct {
	opCode opCode;              // opcode
	registerIndex dstRegIndex;  // destination register index
	registerIndex src1RegIndex; // first source register index
	registerIndex src2RegIndex; // second source register index
	char padding[1];            // 14 bits unused (16 emulated)
}RTypeInstruction;

typedef struct {
	opCode opCode; 					 // opcode
	registerIndex dstRegIndex;       // destination register index
	registerIndex srcRegIndex;       // source register index
	unsigned short intermediateValue; // 16 bits for the intermediate value
}ITypeInstruction;

typedef struct {
	 opCode opCode;
	 address immediateAddress;
}JTypeInstruction;

typedef union {
	RTypeInstruction rType;
	ITypeInstruction iType;
	JTypeInstruction jType;
}instruction;

typedef struct {
	char * file_name;
	int verbose;
}arguments;

// Every instruction is 32 bits long, or 4 bytes
typedef struct {
	char w0;
	char w1;
	char w2;
	char w3;
}binaryInstruction;

arguments main_args;


// A char is 8 bits
// a long is 32 bits


/**************************************************************
 * Instruction Type Definitions
 */
typedef enum instructionType {
	INSTRUCTION_TYPE_UNKNOWN  = 0,
	INSTRUCTION_TYPE_R        = 1,
	INSTRUCTION_TYPE_I        = 2,
	INSTRUCTION_TYPE_J        = 3,
	INSTRUCTION_TYPE_NA       = 4
}instructionType;

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
 * State information containing registers for emulation
 */

typedef struct { 
	unsigned long reg [32]
}state;



/**************************************************************
 * Function Pointers for Opcode emulation
 */

// TODO: Implement all
// TODO: work out a decent state structure, and change "state"
void doOpCode_HALT(char * args, state * state);
void doOpCode_ADD (char * args, state * state);
void doOpCode_ADDI(char * args, state * state);
void doOpCode_SUB (char * args, state * state);
void doOpCode_SUBI(char * args, state * state);
void doOpCode_MUL (char * args, state * state);
void doOpCode_MULI(char * args, state * state);
void doOpCode_LW  (char * args, state * state);
void doOpCode_SW  (char * args, state * state);
void doOpCode_BEQ (char * args, state * state);
void doOpCode_BNE (char * args, state * state);
void doOpCode_BLT (char * args, state * state);
void doOpCode_BGT (char * args, state * state);
void doOpCode_BLE (char * args, state * state);
void doOpCode_BGE (char * args, state * state);
void doOpCode_JMP (char * args, state * state);
void doOpCode_JR  (char * args, state * state);
void doOpCode_JAL (char * args, state * state);
void doOpCode_OUT (char * args, state * state);

/**************************************************************
 * Map between Opcode, Instruction type, and a function pointer
 */
const opCodeDef       opCodeDefinition;
const instructionType instructionTypeDefinition;
// TODO: fill in
void * opCodeDictionary [opCodeDefinition.NUMBER_OF_OPCODES] = 
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



