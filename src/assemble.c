#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  return EXIT_SUCCESS;
}




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

  
















