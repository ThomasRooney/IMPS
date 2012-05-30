#include <stdlib.h>
#include <string.h>
#include "common.h"

/***
A basic idea of how I think this should probably be done efficiently

Naiive Assembling on a function sense requires two things. The first is knowledge of where to assign registers and labels to in code. i.e. When we call JMP on a label, we should replace each label with the appropriate program counter based number. We would also need to associate each *special* command with an appropriate assembler directive. i.e. .fill wwil lbe mapped to a memory writing call, and assignation etc.

Therefore the first thing that we need to do is preparse the assembler code to work out the label tokens and their relative positions in the code. 

To take assembler code and tokenise it we will need to break it between the '\n' and then the ' ' into a multidimensional array. We look for ':' and on each occurence we get the index of the array its on and add it to the map which will include both name of label and program location.

Then, for each beginning of the line index, if it is a token line, then go to that's line token++, lookup the assembler directive 'addi' etc, in our map, and get the opcode index back. put it in the instruction structure. Add in argument details if necessary, removing the '$' or index[0] from the (char *) and call atoi on the result. This gets placed into an appropriate bitfield. Then assemble it with a memcpy into the binaryInstruction typedefintion. Then add it to the memory where we're holding instructions. At the end of this process, write the block of memory to the file.

Optimisation is going to be more difficult. The naiive implementation will probably only take a few hours to implement, but what I'd really like to do is see if I can implement MOSES - meta optimising evolutionary search, to the assembler. This hasn't been done before, but would be a novel way of optimising assembler programs if I can get a logical map between the reduced instruction set and the atomspace that MOSES will work with. A naiive way of doing this will be running the assembler program with all the arguments in a fuzzy way, and then producing test data that MOSES can work with. The program tree generated 'may' be able to map to assembler code in a nice way, as I believe it is axiomatic. This may be an absoloutely useless endevour, but its a pretty cool experiment to get A.I. algorithms that are been attempted with openCog for AGI (Artificial General Intelligence) work.

***/


int main(int argc, char **argv) {
  
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