#include <stdlib.h>
#include <string.h>
#include "assemble.h"

#define ASSEMBLE

#define MAX_INT 32767

/***
A basic idea of how I think this should probably be done efficiently

Naiive Assembling on a function sense requires two things. The first is knowledge of where to assign registers and labels to in code. i.e. When we call JMP on a label, we should replace each label with the appropriate program counter based number. We would also need to associate each *special* command with an appropriate assembler directive. i.e. .fill wwill be mapped to a memory writing call, and assignation etc.

Therefore the first thing that we need to do is preparse the assembler code to work out the label tokens and their relative positions in the code. 

To take assembler code and tokenise it we will need to break it between the '\n' and then the ' ' into a multidimensional array. We look for ':' and on each occurence we get the index of the array its on and add it to the map which will include both name of label and program location.

Then, for each beginning of the line index, if it is a token line, then go to that line token++, lookup the assembler directive 'addi' etc, in our map, and get the opcode index back. put it in the instruction structure. Add in argument details if necessary, removing the '$' or index[0] from the (char *) and call atoi on the result. This gets placed into an appropriate bitfield. Then assemble it with a memcpy into the binaryInstruction typedefintion. Then add it to the memory where we're holding instructions. At the end of this process, write the block of memory to the file.

Optimisation is going to be more difficult. The naiive implementation will probably only take a few hours to implement, but what I'd really like to do is see if I can implement MOSES - meta optimising evolutionary search, to the assembler. This hasn't been done before, but would be a novel way of optimising assembler programs if I can get a logical map between the reduced instruction set and the atomspace that MOSES will work with. A naiive way of doing this will be running the assembler program with all the arguments in a fuzzy way, and then producing test data that MOSES can work with. The program tree generated 'may' be able to map to assembler code in a nice way, as I believe it is axiomatic. This may be an absoloutely useless endevour, but its a pretty cool experiment to get A.I. algorithms that are been attempted with openCog for AGI (Artificial General Intelligence) work.

***/




lineLL *tokAssemblerCode(int inputLength, char * inputBuffer)
{ 
	symbolsLL *symbolsTokCur;
	lineLL *lineTokHead;
	symbolsLL *symbolsTokHEAD;
	lineLL *lineTokCur;
	char * str = inputBuffer;
	char * plh;
	char * plhr;
	char * psh;
	char * pshr;
	if (main_args.verbose) 
		printf ("Splitting string \"%s\" into tokens:\n",str);
	plh = strtok_r (str,"\n", &plhr);
	if (plh != NULL)
		lineTokCur = calloc(1, sizeof(lineLL));
		lineTokHead = lineTokCur; 
	// parse Lines, add to assemblerTokLL 
	while (plh != NULL)
	{
		str = plhr;
		lineTokCur->line = plh;
		if (main_args.verbose)
			printf ("%s\n",lineTokCur->line);
		symbolsTokCur = calloc(1, sizeof(symbolsLL));
		psh = strtok_r(lineTokCur->line, " ", &pshr);
		if (psh != NULL) 
			symbolsTokCur = calloc(1, sizeof(symbolsLL));
			symbolsTokHEAD = symbolsTokCur;
		while (psh != NULL) {
			symbolsTokCur->symbol = psh;
			psh = pshr;
			psh = strtok_r(psh, " ", &pshr);
			if (psh != NULL) {
				symbolsTokCur->next = calloc(1, sizeof(symbolsLL));
				symbolsTokCur = symbolsTokCur->next;
			}
		}
		lineTokCur->symbolsHEAD = symbolsTokHEAD;
		plh = strtok_r (str, "\n", &plhr);
		if (plh != NULL) {
			lineTokCur->next = calloc(1, sizeof(lineLL));
			lineTokCur = lineTokCur->next;
		}
	} 
	return lineTokHead;
}




// PRE: symbols are in form:
//      <opcodeSTR> <argument1> <argument2> ...
// Without labels and .skip statements, but with .fill

// Post: Will return pointer to "output" which now has its fields changed to appropriate binary values.
instruction symbolsToInstruction(symbolsLL * symbols, labelLL *labels) {
	instruction output;
	int opCode;
	int valueBuffer;
	symbolsLL * curSymbol;
	int i = 0;
	
	memset(&output, 0, sizeof(instruction));
	// Deal with .fill statements
	if (strcmp(symbols->symbol, ".fill") == 0) {
		// memcpy the (integer) second argument into the instrction, or it's a null instruction
		if (symbols->next != NULL) {
			valueBuffer = atoi(symbols->next->symbol);
			memcpy(&output, &valueBuffer, sizeof(instruction));
		}
		return output;
	}
	
	// lookup opCode
	opCode = strToOpCode(symbols->symbol);
	if (opCode == GENERIC_ERROR) {
		printf("FATAL ERROR: opCode not found (%s)\n", symbols->symbol);
		exit(EXIT_FAILURE);
	}
	output.raw.opCode = opCode;
	for (curSymbol = symbols->next; curSymbol != NULL; curSymbol = curSymbol->next)
	{			
		// Deal with labels and Registers. Extract the value.
		valueBuffer = extractValue(curSymbol->symbol, labels,
					  (output.raw.opCode == INSTRUCTION_TYPE_I && i++ == 2)// case for I-Type immediate value 
					  || output.raw.opCode == INSTRUCTION_TYPE_J);  // Case for J-Type address
		
		
		// Place this value into the relevant bitfields.
		switch (instructionFromOpcode(output.raw.opCode)) {
			case INSTRUCTION_TYPE_NA:
				// empty instruction, No values
				break;
			case INSTRUCTION_TYPE_R:
				// <opCode> <R1> <R2> <R3>
				switch (i) {
					case 0:
						output.rType.R1 = valueBuffer;
						break;
					case 1:
						output.rType.R2 = valueBuffer;
						break;
					case 2:
						output.rType.R3 = valueBuffer;
						break;
				}
				
				break;
			case INSTRUCTION_TYPE_I:
				switch (i++) {
					case 0:
						output.iType.R1 = valueBuffer;
						break;
					case 1:
						output.iType.R2 = valueBuffer;
						break;
					case 2:
						output.iType.immediateValue = valueBuffer;
						break;
				}

				break;
			case INSTRUCTION_TYPE_J:
				output.jType.address = atoi(curSymbol->symbol);
				break;
		}
	}
		
	return output;
}

 
/********************************************************************
 * This function collects labels for use in symbolsToInstruction
 * It also changes '.skip n' statements into n '.fill' statements
 **/
labelLL preParse(lineLL * lineHEAD) {
	lineLL *lCur;
	lineLL *lBuf;
	symbolsLL *sCur;
	int programCounter;
	int iter;
	int iter2;
	int sizeBuffer;
	char newLine[250];
	int skip;
	labelLL labelHEAD;
	labelLL *labelCur = &labelHEAD;
	symbolsLL * sCurAddress;
	
	for (lCur = lineHEAD; lCur != NULL; lCur = lCur->next)
	{
		programCounter += sizeof(instruction);
		iter = 0;
		for (sCur = lCur->symbolsHEAD; sCur != NULL && iter++; sCur = sCur->next)
		{
			sizeBuffer = strlen(sCur->symbol);
			// Only requirement for label is, "is the last letter a ':'?"
			if(sCur->symbol[sizeBuffer - 1] == ':' )
			{
				// we have a label. Add it to labels LL
				labelCur->next = calloc(1, sizeof(labelLL));
				labelCur = labelCur->next;
				labelCur->labelKey.position = programCounter + iter;
				labelCur->labelKey.keyName = sCur->symbol;
				labelCur->labelKey.keyName[sizeBuffer - 1] = '\0'; // Cut off ':'
				// take down the address of sCur
				sCurAddress = sCur;
				// now remove the label from the symbols list
				sCur = lCur->symbolsHEAD;
				if (sCur == sCurAddress) {
					lCur->symbolsHEAD = sCur->next;
				}
				else {
					while (sCur->next != sCurAddress) {
						sCur = sCur->next;
					}
					sCur->next = sCur->next->next;
				}
			} else {
			if (strcmp(sCur->symbol, ".skip") ==0)
			{
				// Store current lCur->next
				lBuf = lCur->next;
				// Does .skip have an argument?
				if (sCur->next == NULL) {
					// if not, then we skip 1 line
					skip = 1;
					// Free the memory to avoid the memory leak.
					free(sCur);
				}
				else {
					// else we skip 'n' from ".skip n" lines
					skip = atoi(sCur->next->symbol);
					free(sCur);
					free(sCur->next);
				}
				// Overwrite this lCur
				strcpy(newLine, "");
				for (iter2 = 0; iter2 < skip; iter2++)
				{
					strcat(newLine,".fill 0\n");
				}
				lCur = tokAssemblerCode(strlen(newLine), newLine);
				while (lCur->next!=NULL)
					lCur = lCur->next;
				lCur->next = lBuf;
			}}
		}
	}
	return labelHEAD;
}

assembledProgram assembleProgram(preAssemblyProgram * preAssemblyHead) { 
	preAssemblyProgram *preAssemblyCur;
	assembledProgram output;
	instruction * memPointer;
	// First work out length of program
	for (preAssemblyCur = preAssemblyHead;
		 preAssemblyCur != NULL;
		 preAssemblyCur=preAssemblyCur->next)
	{
		output.length += sizeof(instruction);		
	}
	// malloc the length
	output.memory = malloc(output.length);
	memPointer = output.memory;
	// place the instructions onto the output
	for (preAssemblyCur = preAssemblyHead;
		 preAssemblyCur != NULL;
		 preAssemblyCur=preAssemblyCur->next)
	{
		memcpy(memPointer++, preAssemblyCur, sizeof(instruction));
	}	
	return output;
}



int main(int argc, char **argv) {
	char * inputBuffer;
	int  * inputLength = malloc(sizeof(int));
	lineLL * tokenisedFile;
	lineLL * lineCur;
	assembledProgram assembled;
	labelLL labels;
	preAssemblyProgram preAssemblyHEAD;
	preAssemblyProgram *preAssemblyCur = &preAssemblyHEAD;
	
	// Get arguments in terms of .s file (prob)
	parseArguments(argc, argv);
	if (readFile(main_args.file_name, inputLength, &inputBuffer)>EXIT_SUCCESS)
		return FATAL_ERROR;
	
	if (main_args.verbose)
		printf("File Read Success\n");
	// Tokenisation
	tokenisedFile = tokAssemblerCode(*inputLength, inputBuffer);
	// PreParser
	labels = preParse(tokenisedFile);
	  
	// Parsing Loop
	for (lineCur = tokenisedFile;
	   lineCur != NULL;
	   lineCur = lineCur->next)
	{
		preAssemblyCur->next = calloc(1, sizeof(preAssemblyProgram));
		preAssemblyCur = preAssemblyCur->next;
		preAssemblyCur->curInstruction = symbolsToInstruction(lineCur->symbolsHEAD, &labels);
	}
	// Assemble Program
	assembled = assembleProgram(&preAssemblyHEAD);
	// Save the assembled program to the file specified
  // Move the parsed results to binary
  return EXIT_SUCCESS;
}


/*



FILE *file;
//open file
file = fopen(const char *filename, "rt");

//check for errors
  if (file == NULL) {
    perror("Unable to open the file: %s", filename);
    exit(FATAL_ERROR);
  }




close(file);


File *binFile;
binFile = fopen("binaryRep.txt","w");

  if (binFile == null) {
    perror("unable to open the file: %s", filename);





// Will do first pass over code to create symbol table and match labels to memory
void findLabels (
  int labels;
  labels = numLabels(file);            // gets the number of labels
//int *labelsTable = malloc (labels * (15 * sizeof(char))); // assume that the longest label will be 15 chars long
  char labelTable[labels];

  for (int i = 0; i < labels; i++) {
    labelTable[i] = getLabel


  



// counts number of labels in the file.
int numLabels (File *file) {
  int labels = 0;
  char *currentChar;
    while (!feof(file) == 0 ) {
      fgets(currentChar, 1, file);
        if (currentChar == ":") 
          labels ++;
    }
  return labels;
  } 



// Shall perform the second pass over the code to change all the OpCodes and operands into binary.
void convertOps (char line[]
char *currentChar = line;
char *tokens;

// checks the whole string after the Opcode
  while (*currentChar != "\n") {
    tokens = strtok(line, " ");
    currentChar = tokens


    if (*currentChar == "$") {
      while (*currentChar != "\0") {
      
      currentChar++;
      pBin(currentChar); // change if two digit register address
    
  }

  
















*/
