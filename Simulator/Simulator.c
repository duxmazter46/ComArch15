#include <stdio.h>
#include <stdint.h>
#include <string.h>

/*
 Rules for Simulator 
1) (simulator) อย่าแก้ printState หรือ stateStruct

2) (simulator) Call printState 1 ครั้ง ก่อน instruction executes และ 1 ครั้ง  ก่อน simulator exits.  อย่า call printState ที่อื่น.

3) (simulator) อย่า print "@@@" ที่ใดยกเว้นใน printState.

4) (simulator) state.numMemory ต้อง เท่ากับ จำนวนบรรทัด ใน machine-code file.

5) (simulator) Initialize ทุก registers ให้เป็น 0.
*/


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

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }

    state.pc = 0; // Initialize the program counter
    state.numMemory = 0; // Initialize numMemory

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
    }
        // Print the state before executing the command and exit
        printState(&state);
        return 0;
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

