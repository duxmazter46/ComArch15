#include<stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

/*
 Rules for Simulator 
1) (simulator) อย่าแก้ printState หรือ stateStruct

2) (simulator) Call printState 1 ครั้ง ก่อน instruction executes และ 1 ครั้ง  ก่อน simulator exits.  อย่า call printState ที่อื่น.

3) (simulator) อย่า print "@@@" ที่ใดยกเว้นใน printState.

4) (simulator) state.numMemory ต้อง เท่ากับ จำนวนบรรทัด ใน machine-code file.

5) (simulator) Initialize ทุก registers ให้เป็น 0.
*/

/* instruction-level simulator */
#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;


void printState(stateType *);

 int convertNum(int num)
    {
          /* convert a 16-bit number into a 32-bit integer */
          if (num & (1<<15) ) {
              num -= (1<<16);
          }
          return(num);
    }

// Function to convert a binary string to an integer
int binaryStringToInt(const char* binary) {
    return (int)strtol(binary, NULL, 2);
}

// Function to extract the opcode from a binary string
int extractOpcode(const char* binaryValue) {
    char opcodeBinary[4];
    strncpy(opcodeBinary, binaryValue, 3);
    opcodeBinary[3] = '\0';
    return binaryStringToInt(opcodeBinary);
}

  void printState(stateType *statePtr)
    {
        int i;
        printf("\n@@@\nstate:\n");
        printf("\tpc %d\n", statePtr->pc);
        printf("\tmemory:\n");
        for (i=0; i<statePtr->numMemory; i++) {
            printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
        }
        printf("\tregisters:\n");
        for (i=0; i<NUMREGS; i++) {
            printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
        }
        printf("end state\n");
    }   


int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;
    int sum = 0;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");       //fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }
    
    // Initialize registers to 0
    for (int i = 0; i < NUMREGS; i++) {
        state.reg[i] = 0;
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
         state.numMemory++) {
        if (sscanf(line, "%d", state.mem + state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
         while (1) {
        printState(&state);

        int intValue = state.mem[state.pc];
        char binaryValue[17];
        itoa(intValue, binaryValue, 2);

        char opcodeBinary[4];
        strncpy(opcodeBinary, binaryValue, 3);
        opcodeBinary[3] = '\0';
        int opcode = binaryStringToInt(opcodeBinary);

        int regA, regB, destReg, offsetField, memoryAddress;

        switch (opcode) {
            case 0: // add
                regA = binaryStringToInt(&binaryValue[10]);
                regB = binaryStringToInt(&binaryValue[13]);
                destReg = binaryStringToInt(&binaryValue[29]);
                state.reg[destReg] = state.reg[regA] + state.reg[regB];
                state.pc++;
                sum++;
                break;

            case 1: // nand
                regA = binaryStringToInt(&binaryValue[10]);
                regB = binaryStringToInt(&binaryValue[13]);
                destReg = binaryStringToInt(&binaryValue[29]);
                state.reg[destReg] = ~(state.reg[regA] & state.reg[regB]);
                state.pc++;
                sum++;
                break;

            case 2: // lw
                regA = binaryStringToInt(&binaryValue[10]);
                regB = binaryStringToInt(&binaryValue[13]);
                offsetField = binaryStringToInt(&binaryValue[16]);
                offsetField = convertNum(offsetField);
                memoryAddress = state.reg[regA] + offsetField;
                if (memoryAddress < 0 || memoryAddress >= NUMMEMORY) {
                    printf("ERROR: Invalid memory address\n");
                    exit(1);
                }
                state.reg[regB] = state.mem[memoryAddress];
                state.pc++;
                sum++;
                break;

            case 3: // sw
                regA = binaryStringToInt(&binaryValue[10]);
                regB = binaryStringToInt(&binaryValue[13]);
                offsetField = binaryStringToInt(&binaryValue[16]);
                offsetField = convertNum(offsetField);
                memoryAddress = state.reg[regA] + offsetField;
                if (memoryAddress < 0 || memoryAddress >= NUMMEMORY) {
                    printf("ERROR: Invalid memory address\n");
                    exit(1);
                }
                state.mem[memoryAddress] = state.reg[regB];
                state.pc++;
                sum++;
                break;

            case 4: // beq
                regA = binaryStringToInt(&binaryValue[10]);
                regB = binaryStringToInt(&binaryValue[13]);
                offsetField = binaryStringToInt(&binaryValue[16]);
                offsetField = convertNum(offsetField);
                if (state.reg[regA] == state.reg[regB]) {
                    sum++;
                    state.pc = state.pc + 1 + offsetField;
                } else {
                    sum++;
                    state.pc++;
                }
                break;

            case 5: // jalr
                regA = binaryStringToInt(&binaryValue[10]);
                regB = binaryStringToInt(&binaryValue[13]);
                state.reg[regB] = state.pc + 1;
                if (regA == regB) {
                    state.pc++;
                } else {
                    state.pc = state.reg[regA];
                }
                sum++;
                break;

            case 6: // halt
                sum++;
                printf("machine halted\n");
                printf("total of %d instructions executed\n", sum);
                printf("final state of machine:\n");
                state.pc++;
                printState(&state);
                return 0;

            case 7: // noop
                sum++;
                state.pc++;
                break;

            default:
                printf("ERROR: Memory at location %d is illegible.\n", state.pc);
                exit(1);
        }
    }
        return 0;
    }

} 

