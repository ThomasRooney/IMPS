#define FATAL_ERROR 10

typedef unsigned char opCode;        // 8 bits to emulate 6 bit field
typedef unsigned char registerIndex;  // 4 bits
typedef unsigned long address;    // 32 bits to emulate 26 bit field



// A char is 8 bits
// a long is 32 bits


typedef struct {
	opCode opCode;              // opcode
	registerIndex dstRegIndex;  // destination register index
	registerIndex src1RegIndex; // first source register index
	registerIndex src2RegIndex; // second source register index
	                            // 14 bits unused caused by the union
}RTypeInstruction;

typedef struct {
	opCode opCode; 					 // opcode
	registerIndex dstRegIndex;       // destination register index
	registerIndex srcRegIndex;       // source register index
	unsigned long intermidiateValue; // 16 bits for the intermediate value
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

// Every instruction is 32 bits long, or 8 bytes
typedef struct {
	char w0;
	char w1;
	char w2;
	char w3;
}binaryInstruction;

arguments main_args;
