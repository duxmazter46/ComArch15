/* Assembler code fragment */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000
#define MAXLABELS 100
#define MAXLINES 100


typedef struct {
    char name[MAXLINELENGTH];
    int value;
    int add;
} LabelEntry;

int code[MAXLINES] = {0};

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
void firstPass(FILE *, LabelEntry[], int *);
int secondPass(FILE *, LabelEntry[], int*);
int thirdPass(FILE *, LabelEntry[], int*);
int* decimalToBinary3Array(int );
int* decimalToBinary16Array(int );
char* intArrayToCharArray(const int intArray[32]);
char* binaryToHex(const char* binary);
int hexCharToDecimal(char hexChar);
int hexToDecimal(const char* hexString);


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

    secondPass(inFilePtr, labelTable, &labelCount);

    // Rewind the input file for the third pass
    rewind(inFilePtr);

    int lines = thirdPass(inFilePtr, labelTable, &labelCount);


    printf("\nWriting %d lines to output\n\n",lines);
    for(int i = 0 ; i < lines;i++){
        printf("%d\n",code[i]);
        // Print each line to the output file
        fprintf(outFilePtr, "%d\n", code[i]);
    }

    // Close the output file
    fclose(outFilePtr);

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

/*
 * Perform the first pass of assembly code parsing. This pass reads lines from
 * the input assembly-language file, identifies labels, opcodes, and arguments,
 * and populates the label table with label names and their corresponding
 * addresses.
 *
 * Parameters:
 *   inFilePtr   - Pointer to the input assembly-language file.
 *   labelTable  - An array of LabelEntry structures to store label information.
 *   labelCount  - Pointer to an integer to track the count of labels.
 *
 * Return values:
 *   None. The labelTable and labelCount parameters are updated with label
 *   information.
 *
 * Exit(1) if a line is too long or if the maximum number of labels is exceeded.
 */
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
                labelTable[*labelCount].value = 0;// Initialize value to 0
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

/*
 * Perform the second pass of assembly code parsing. This pass updates the values
 * of labels in the labelTable based on .fill instructions.
 *
 * Parameters:
 *   inFilePtr   - Pointer to the input assembly-language file.
 *   labelTable  - An array of LabelEntry structures storing label information.
 *   labelCount  - Pointer to an integer indicating the count of labels.
 *
 * Return value:
 *   1 if the second pass completes successfully.
 */
int secondPass(FILE *inFilePtr, LabelEntry labelTable[], int *labelCount) {
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
        arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

    int lineCounter = 0;

    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        lineCounter++;

        if (strcmp(opcode, ".fill") == 0) {
            int value;

            if (isNumber(arg0)) {
                // If arg0 is a number, use it as the value
                value = atoi(arg0);
            } else {
                // Find the label's corresponding value
                for (int i = 0; i < *labelCount; i++) {
                    if (strcmp(labelTable[i].name, arg0) == 0) {
                        labelTable[i].value = labelTable[i].add;
                        break;
                    }
                }
            }

            // Update the value in the labelTable
            for (int i = 0; i < *labelCount; i++) {
                if (strcmp(labelTable[i].name, label) == 0) {
                    labelTable[i].value = value;
                    break;
                }
            }
        }
    }


    // Print all labels and their updated values
    printf("\nLabels and Updated Values after Second Pass:\n");
    for (int i = 0; i < *labelCount; i++) {
        printf("Label: %-10s Value: %d Address: %d\n", labelTable[i].name, labelTable[i].value, labelTable[i].add);
    }

    return 1;
}

/*
 * Perform the third pass of assembly code parsing. This pass generates machine
 * code for the instructions and fills the 'code' array with the corresponding values.
 *
 * Parameters:
 *   inFilePtr   - Pointer to the input assembly-language file.
 *   labelTable  - An array of LabelEntry structures storing label information.
 *   labelCount  - Pointer to an integer indicating the count of labels.
 *   code        - An integer array to store the generated machine code values.
 *
 * Return value:
 *   The number of lines processed during the third pass.
 */
int thirdPass(FILE *inFilePtr, LabelEntry labelTable[], int *labelCount) {
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
        arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

    int machineCode[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
    int value;

    int lineCounter = 0;
    printf("\n");

    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        lineCounter++;

        for(int i = 0; i < 32; i++ ){
            machineCode[i] = 0;
        }


        // Map the opcode to machine code based on the instruction
        if (strcmp(opcode, "add") == 0) {
        // Map "add" instruction to machine code

            machineCode[24] = 0;machineCode[23] = 0;machineCode[22] = 0;
            printf("opcode:%d%d%d ",machineCode[24],machineCode[23],machineCode[22]);

            int* regA = decimalToBinary3Array(atoi(arg0));
            int* regB = decimalToBinary3Array(atoi(arg1));
            int* dest = decimalToBinary3Array(atoi(arg2));

            machineCode[21] = regA[2];machineCode[20] = regA[1];machineCode[19] = regA[0];
            printf("regA:%d%d%d ",machineCode[21],machineCode[20],machineCode[19]);

            machineCode[18] = regB[2];machineCode[17] = regB[1];machineCode[16] = regB[0];
            printf("regB:%d%d%d ",machineCode[18],machineCode[17],machineCode[16]);

            machineCode[2] = dest[2];machineCode[1] = dest[1];machineCode[0] = dest[0];
            printf("dest:%d%d%d ",machineCode[2],machineCode[1],machineCode[0]);

            free(regA);
            free(regB);
            free(dest);


        } else if (strcmp(opcode, "nand") == 0) {
        // Map "nand" instruction to machine code

            machineCode[24] = 0;machineCode[23] = 0;machineCode[22] = 1;

            int* regA = decimalToBinary3Array(atoi(arg0));
            int* regB = decimalToBinary3Array(atoi(arg1));
            int* dest = decimalToBinary3Array(atoi(arg2));

            machineCode[21] = regA[2];machineCode[20] = regA[1];machineCode[19] = regA[0];

            machineCode[18] = regB[2];machineCode[17] = regB[1];machineCode[16] = regB[0];

            machineCode[2] = dest[2];machineCode[1] = dest[1];machineCode[0] = dest[0];

            free(regA);
            free(regB);
            free(dest);


        }else if (strcmp(opcode, "lw") == 0) {
        // Map "lw" instruction to machine code

            machineCode[24] = 0; machineCode[23] = 1; machineCode[22] = 0;

            int* regA = decimalToBinary3Array(atoi(arg0));
            int* regB = decimalToBinary3Array(atoi(arg1));

            machineCode[21] = regA[2]; machineCode[20] = regA[1]; machineCode[19] = regA[0];

            machineCode[18] = regB[2]; machineCode[17] = regB[1]; machineCode[16] = regB[0];

            int offset;
            int* offsetBinary = NULL;

            if (isNumber(arg2)) {
                // If arg2 is a number, use it directly
                offset = atoi(arg2);
            } else {
                // Find the label's corresponding value
                offset = 32768; // Initialize to an invalid value
                for (int i = 0; i < *labelCount; i++) {
                    if (strcmp(labelTable[i].name, arg2) == 0) {
                        offset = labelTable[i].add;
                        break;
                    }
                }

                if (offset == 32768) {
                    fprintf(stderr, "Error: Label '%s' not found at line %d\n", arg2, lineCounter);
                    exit(1);
                }
            }

            // Convert the offset value to binary and copy it to machineCode[15]->[0]
            offsetBinary = decimalToBinary16Array(offset);
            for (int i = 0; i < 15 ; i++) {
                machineCode[i] = offsetBinary[15-i];
            }

            printf("opcode:%d%d%d ",machineCode[24],machineCode[23],machineCode[22]);
            printf("regA:%d%d%d ",machineCode[21],machineCode[20],machineCode[19]);
            printf("regB:%d%d%d ",machineCode[18],machineCode[17],machineCode[16]);
            printf("offset:");
            for (int i = 15; i >= 0; i--) {
                printf("%d",machineCode[i]);
            }
            printf(" ");

        

            free(regA);
            free(regB);
            free(offsetBinary);
        }else if (strcmp(opcode, "sw") == 0) {
        // Map "sw" instruction to  machine code

            machineCode[24] = 0;machineCode[23] = 1;machineCode[22] = 1;

            int* regA = decimalToBinary3Array(atoi(arg0));
            int* regB = decimalToBinary3Array(atoi(arg1));

            machineCode[21] = regA[2]; machineCode[20] = regA[1]; machineCode[19] = regA[0];

            machineCode[18] = regB[2]; machineCode[17] = regB[1]; machineCode[16] = regB[0];

            int offset;
            int* offsetBinary = NULL;

            if (isNumber(arg2)) {
                // If arg2 is a number, use it directly
                offset = atoi(arg2);
            } else {
                // Find the label's corresponding value
                offset = 32768; // Initialize to an invalid value
                for (int i = 0; i < *labelCount; i++) {
                    if (strcmp(labelTable[i].name, arg2) == 0) {
                        offset = labelTable[i].add;
                        break;
                    }
                }

                if (offset == 32768) {
                    fprintf(stderr, "Error: Label '%s' not found at line %d\n", arg2, lineCounter);
                    exit(1);
                }
            }

            // Convert the offset value to binary and copy it to machineCode[15]->[0]
            offsetBinary = decimalToBinary16Array(offset);
            for (int i = 0; i < 15 ; i++) {
                machineCode[i] = offsetBinary[15-i];
            }

            printf("opcode:%d%d%d ",machineCode[24],machineCode[23],machineCode[22]);
            printf("regA:%d%d%d ",machineCode[21],machineCode[20],machineCode[19]);
            printf("regB:%d%d%d ",machineCode[18],machineCode[17],machineCode[16]);
            printf("offset:");
            for (int i = 15; i >= 0; i--) {
                printf("%d",machineCode[i]);
            }
            printf(" ");

        

            free(regA);
            free(regB);
            free(offsetBinary);


        }else if (strcmp(opcode, "beq") == 0) {
        // Map "beq" instruction to machine code

            machineCode[24] = 1;machineCode[23] = 0;machineCode[22] = 0;

            int* regA = decimalToBinary3Array(atoi(arg0));
            int* regB = decimalToBinary3Array(atoi(arg1));

            machineCode[21] = regA[2]; machineCode[20] = regA[1]; machineCode[19] = regA[0];

            machineCode[18] = regB[2]; machineCode[17] = regB[1]; machineCode[16] = regB[0];

            int offset;
            int* offsetBinary = NULL;

            if (isNumber(arg2)) {
                // If arg2 is a number, use it directly
                offset = atoi(arg2);
            } else {
                // Find the label's corresponding value
                offset = 32768; // Initialize to an invalid value
                for (int i = 0; i < *labelCount; i++) {
                    if (strcmp(labelTable[i].name, arg2) == 0) {
                        offset = labelTable[i].add - (lineCounter);
                        break;
                    }
                }

                if (offset == 32768) {
                    fprintf(stderr, "Error: Label '%s' not found at line %d\n", arg2, lineCounter);
                    exit(1);
                }
            }

            // Convert the offset value to binary and copy it to machineCode[15]->[0]
            offsetBinary = decimalToBinary16Array(offset);
            for (int i = 0; i < 15 ; i++) {
                machineCode[i] = offsetBinary[15-i];
            }
            if(offset < 0)  machineCode[15] = 1 ;

            printf("opcode:%d%d%d ",machineCode[24],machineCode[23],machineCode[22]);
            printf("regA:%d%d%d ",machineCode[21],machineCode[20],machineCode[19]);
            printf("regB:%d%d%d ",machineCode[18],machineCode[17],machineCode[16]);
            printf("offset:");
            for (int i = 15; i >= 0; i--) {
                printf("%d",machineCode[i]);
            }
            printf(" ");

        

            free(regA);
            free(regB);
            free(offsetBinary);


        }else if (strcmp(opcode, "jalr") == 0) {
        // Map "jalr" instruction to machine code

            machineCode[24] = 1;machineCode[23] = 0;machineCode[22] = 1;

            int* regA = decimalToBinary3Array(atoi(arg0));
            int* regB = decimalToBinary3Array(atoi(arg1));

            machineCode[21] = regA[2];machineCode[20] = regA[1];machineCode[19] = regA[0];

            machineCode[18] = regB[2];machineCode[17] = regB[1];machineCode[16] = regB[0];

            printf("opcode:%d%d%d ",machineCode[24],machineCode[23],machineCode[22]);
            printf("regA:%d%d%d ",machineCode[21],machineCode[20],machineCode[19]);
            printf("regB:%d%d%d ",machineCode[18],machineCode[17],machineCode[16]);


            free(regA);
            free(regB);

        }else if (strcmp(opcode, "halt") == 0) {
        // Map "halt" instruction to machine code

            machineCode[24] = 1;machineCode[23] = 1;machineCode[22] = 0;

            printf("opcode:%d%d%d ",machineCode[24],machineCode[23],machineCode[22]);

        }else if (strcmp(opcode, "noop") == 0) {
        // Map "noop" instruction to machine code

            machineCode[24] = 1;machineCode[23] = 1;machineCode[22] = 1;

            printf("opcode:%d%d%d ",machineCode[24],machineCode[23],machineCode[22]);

        }else if (strcmp(opcode, ".fill") == 0) {
        // Map ".fill" instruction to machine code
            int offset;
            int* offsetBinary = NULL;

            if (isNumber(arg0)) {
                // If arg2 is a number, use it directly
                offset = atoi(arg0);
            } else {
                // Find the label's corresponding value
                offset = 32768; // Initialize to an invalid value
                for (int i = 0; i < *labelCount; i++) {
                    if (strcmp(labelTable[i].name, arg0) == 0) {
                        offset = labelTable[i].value;
                        break;
                    }
                }

                if (offset == 32768) {
                    fprintf(stderr, "Error: Label '%s' not found at line %d\n", arg0, lineCounter);
                    exit(1);
                }
            }

            value = offset;

            free(offsetBinary);

            

        }

        // Print the address "i: 000..."
        printf("(address %d): ", lineCounter-1);
        

        if(strcmp(opcode,".fill") != 0){
            // Print the machineCode from bit 32 to 1
            char* charCode = intArrayToCharArray(machineCode);
            printf("%s",charCode);

            char* hexArray = binaryToHex(charCode);
            printf(" (hex 0x%s)", hexArray);

            // Convert the hexArray back to decimal
            int decimalValueFromArray = hexToDecimal(hexArray);
            printf("%d\n", decimalValueFromArray);
            code[lineCounter-1] = decimalValueFromArray;

            free(hexArray);
            
        }else{
            printf("%d\n",value);
            code[lineCounter-1] = value;
        }
        

    }
    
    return lineCounter;

}

/*
 * Convert a decimal value (0 to 7) to a 3-bit binary integer array.
 *
 * Parameters:
 *   decimalValue: The decimal value to convert (0 to 7).
 *
 * Returns:
 *   An integer array representing the binary value (3 bits).
 */
int* decimalToBinary3Array(int decimalValue) {
    if(decimalValue< 0 || decimalValue >7) exit(0);


    int* binaryArray = (int*)malloc(3 * sizeof(int));

    binaryArray[2] = (decimalValue & 4) ? 1 : 0;
    binaryArray[1] = (decimalValue & 2) ? 1 : 0;
    binaryArray[0] = (decimalValue & 1) ? 1 : 0;

    return binaryArray;
}

/*
 * Convert a decimal value (-32768 to 32767) to a 16-bit binary integer array.
 * Handles two's complement representation for negative values.
 *
 * Parameters:
 *   decimalValue: The decimal value to convert (-32768 to 32767).
 *
 * Returns:
 *   An integer array representing the binary value (16 bits).
 */
int* decimalToBinary16Array(int decimalValue) {
    if (decimalValue < -32768 || decimalValue > 32767) {
        // Handle out-of-range values or any other error condition
        // You can choose to return an error code or exit the program
        // Here, I'm exiting with an error message
        fprintf(stderr, "Error: Decimal value out of range.\n");
        exit(1);
    }

    int* binaryArray = (int*)malloc(16 * sizeof(int));

    // Check if the value is negative
    int isNegative = (decimalValue < 0) ? 1 : 0;

    // Convert to positive if negative
    if (isNegative) {
        decimalValue = -decimalValue;
    }

    // Initialize all bits to 0
    for (int i = 0; i < 16; i++) {
        binaryArray[i] = 0;
    }

    // Convert to binary and store in the array
    for (int i = 15; i >= 0; i--) {
        binaryArray[i] = decimalValue % 2;
        decimalValue /= 2;
    }

    // Apply 2's complement if the value was negative
    if (isNegative) {
        // Invert all bits
        for (int i = 0; i < 16 ; i++) {
            binaryArray[i] = (binaryArray[i] == 0) ? 1 : 0;
        }

        // Add 1 to the binary representation
        int carry = 1;
        for (int i = 15; i >= 0; i--) {
            int sum = binaryArray[i] + carry;
            binaryArray[i] = sum % 2;
            carry = sum / 2;
        }

    }

    return binaryArray;
}

/*
 * Convert a binary integer array (32 bits) to a character array (string).
 *
 * Parameters:
 *   intArray: An array of 32 integers representing the binary value.
 *
 * Returns:
 *   A character array (string) representing the binary value.
 */
char* intArrayToCharArray(const int intArray[32]) {
    char* charArray = (char*)malloc(33); // 32 bits + null terminator
    if (charArray == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }

    for (int i = 0; i < 32; i++) {
        charArray[i] = intArray[31-i] ? '1' : '0';
    }

    charArray[32] = '\0'; // Null-terminate the string

    return charArray;
}

/*
 * Convert a binary character array to a hexadecimal character array.
 * Assumes the binary string length is a multiple of 4.
 *
 * Parameters:
 *   binary: A binary character array (string).
 *
 * Returns:
 *   A hexadecimal character array (string).
 */
char* binaryToHex(const char* binary) {
    int binaryLength = strlen(binary);
    
    // Ensure that the length of the binary string is a multiple of 4
    if (binaryLength % 4 != 0) {
        fprintf(stderr, "Error: Binary string length must be a multiple of 4.\n");
        exit(1);
    }
    
    int hexLength = binaryLength / 4;
    char* hexString = (char*)malloc(hexLength + 1); // +1 for null terminator
    
    if (hexString == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    
    for (int i = 0; i < hexLength; i++) {
        char binaryChunk[5]; // 4 bits + null terminator
        strncpy(binaryChunk, binary + i * 4, 4);
        binaryChunk[4] = '\0';
        int decimalValue = strtol(binaryChunk, NULL, 2);
        sprintf(hexString + i, "%x", decimalValue);
    }
    
    hexString[hexLength] = '\0'; // Null-terminate the string
    
    return hexString;
}

/*
 * Convert a hexadecimal character to its decimal value.
 *
 * Parameters:
 *   hexChar: A single hexadecimal character ('0' to '9', 'A' to 'F', or 'a' to 'f').
 *
 * Returns:
 *   The decimal value corresponding to the hexadecimal character.
 */

int hexCharToDecimal(char hexChar) {
    if (hexChar >= '0' && hexChar <= '9') {
        return hexChar - '0';
    } else if (hexChar >= 'A' && hexChar <= 'F') {
        return hexChar - 'A' + 10;
    } else if (hexChar >= 'a' && hexChar <= 'f') {
        return hexChar - 'a' + 10;
    }
    // Handle invalid characters
    fprintf(stderr, "Error: Invalid hexadecimal character '%c'\n", hexChar);
    exit(1);
}

/*
 * Convert a hexadecimal string to a decimal integer.
 *
 * Parameters:
 *   hexString: A hexadecimal string.
 *
 * Returns:
 *   The decimal integer value corresponding to the hexadecimal string.
 */
int hexToDecimal(const char* hexString) {
    int length = strlen(hexString);
    int result = 0;

    for (int i = 0; i < length; i++) {
        result = result * 16 + hexCharToDecimal(hexString[i]);
    }

    return result;
}


