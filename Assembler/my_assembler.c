/* Assembler code fragment */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000
#define MAXLABELS 100

typedef struct {
    char name[MAXLINELENGTH];
    int value;
    int add;
} LabelEntry;

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
void firstPass(FILE *, LabelEntry[], int *);
void secondPass(FILE *, LabelEntry[], int*);

int main(int argc, char *argv[])
{
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
            argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];

    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }
    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }

/*
    //here is an example for how to use readAndParse to read a line from inFilePtr //

    if (! readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {

        // reached end of file //

    }

    //this is how to rewind the file ptr so that you start reading from the beginning of the file //
    rewind(inFilePtr);

    //after doing a readAndParse, you may want to do the following to test the opcode //

    if (!strcmp(opcode, "add")) {

        // do whatever you need to do for opcode "add" //

    }
*/

    // Label table to store labels and their addresses/values
    LabelEntry labelTable[MAXLABELS];
    int labelCount = 0;

        // First pass to compute label addresses/values
    firstPass(inFilePtr, labelTable, &labelCount);

    // Rewind the input file for the second pass
    rewind(inFilePtr);


    return(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0, char *arg1, char *arg2) {
    char line[MAXLINELENGTH];
    char *ptr = line;

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
        /* reached end of file */
        return(0);
    }

    /* check for line too long (by looking for a \n) */
    if (strchr(line, '\n') == NULL) {
        /* line too long */
        printf("error: line too long\n");
        exit(1);
    }

    /* Check if the first string cluster is a label */
    ptr = line;
    while (*ptr != '\0' && (*ptr == ' ' || *ptr == '\t')) {
        ptr++; // Skip leading whitespace
    }
    
     /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n ]", label)) {
	/* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]", opcode, arg0, arg1, arg2);
    return(1);
}



int isNumber(char *string)
{
    /* return 1 if string is a number */
    int i;
    return( (sscanf(string, "%d", &i)) == 1);
}

void firstPass(FILE *inFilePtr, LabelEntry labelTable[], int *labelCount) {
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
        arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

    int lineCounter = 0;

    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        lineCounter++;

        // Check if the opcode belongs to a specific group
        if (strcmp(opcode, "add") == 0 || strcmp(opcode, "nand") == 0) {
            // Instructions in the "add" and "nand" group (R-type)
            printf("R-type: label: %-10s opcode: %-10s arg0: %-10s arg1: %-10s arg2: %-10s\n",
                   label, opcode, arg0, arg1, arg2);
        } else if (strcmp(opcode, "lw") == 0 || strcmp(opcode, "sw") == 0 || strcmp(opcode, "beq") == 0) {
            // Instructions in the "lw," "sw," and "beq" group (I-type)
            printf("I-type: label: %-10s opcode: %-10s arg0: %-10s arg1: %-10s arg2: %-10s\n",
                   label, opcode, arg0, arg1, arg2);
        } else if (strcmp(opcode, "jalr") == 0) {
            // Instruction "jalr" (J-type)
            printf("J-type: label: %-10s opcode: %-10s arg0: %-10s arg1: %-10s\n",
                   label, opcode, arg0, arg1);         
        } else if (strcmp(opcode, ".fill") == 0) {
            // Instruction ".fill" 
            printf("O-type: label: %-10s opcode: %-10s arg0: %-10s \n",
                   label, opcode, arg0);
        }else {
            // O,halt,.fill instructions 
            printf("O-type: label: %-10s opcode: %-10s \n",
                   label, opcode);
        }

        // Check if the label is empty or too long
        if (strlen(label) > 0) {
            // Add the label to the labelTable
            if (*labelCount < MAXLABELS) {
                strcpy(labelTable[*labelCount].name, label);
                labelTable[*labelCount].add = lineCounter-1; 
                labelTable[*labelCount].value = -1;// Initialize value to -1
                (*labelCount)++;
            } else {
                fprintf(stderr, "Error: Maximum number of labels (%d) exceeded at line %d\n", MAXLABELS, lineCounter);
                exit(1);
            }
        }

        // Check if the label is too long
        if (strlen(label) > 6) {
            fprintf(stderr, "Error: Label '%s' exceeds the maximum length of 6 characters at line %d\n", label, lineCounter);
            exit(1);
        }

    }

    // Print all labels and their values
    printf("\nLabels and Values after First Pass:\n");
    for (int i = 0; i < *labelCount; i++) {
        printf("Label: %-10s Value: %d Address: %d\n", labelTable[i].name, labelTable[i].value,labelTable[i].add);
    }

}





