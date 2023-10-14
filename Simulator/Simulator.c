#include <stdio.h>
#include <stdint.h>

/*
 Rules for Simulator 
1) (simulator) อย่าแก้ printState หรือ stateStruct

2) (simulator) Call printState 1 ครั้ง ก่อน instruction executes และ 1 ครั้ง  ก่อน simulator exits.  อย่า call printState ที่อื่น.

3) (simulator) อย่า print "@@@" ที่ใดยกเว้นใน printState.

4) (simulator) state.numMemory ต้อง เท่ากับ จำนวนบรรทัด ใน machine-code file.

5) (simulator) Initialize ทุก registers ให้เป็น 0.
*/

// Define the maximum number of memory locations
#define MAX_MEMORY 1024

// Define the state structure
struct stateStruct {
    uint32_t registers[32];
    uint32_t memory[MAX_MEMORY];
    uint32_t numMemory;
    uint32_t programCounter;
};

// Function to initialize the state structure
void initializeState(struct stateStruct *state) {
    for (int i = 0; i < 32; i++) {
        state->registers[i] = 0;
    }
    for (int i = 0; i < MAX_MEMORY; i++) {
        state->memory[i] = 0;
    }
    state->numMemory = 0;
    state->programCounter = 0;
}

// Function to print the current state of the machine
void printState(struct stateStruct *state) {
    printf("@@@\nstate:\n");
    printf("    pc %d\n", state->programCounter);
    for (int i = 0; i < 32; i++) {
        printf("    reg[%d] %d\n", i, state->registers[i]);
    }
    for (int i = 0; i < state->numMemory; i++) {
        printf("    mem[%d] %d\n", i, state->memory[i]);
    }
}

// Function to execute machine code
void executeMachineCode(struct stateStruct *state) {
    while (state->programCounter < state->numMemory) {
        // Call printState before executing the command
        printState(state);

        // Fetch the instruction from memory
        uint32_t instruction = state->memory[state->programCounter];
        uint32_t opcode = (instruction >> 26) & 0x3F;
        uint32_t rs = (instruction >> 21) & 0x1F;
        uint32_t rt = (instruction >> 16) & 0x1F;
        uint32_t rd = (instruction >> 11) & 0x1F;
        int32_t immediate = instruction & 0xFFFF;

        // Execute the instruction (simplified for illustration)
        switch (opcode) {
            case 0x20: // ADD
                state->registers[rd] = state->registers[rs] + state->registers[rt];
                break;
            case 0x23: // LW
                // Simplified memory access (assuming byte addressable memory)
                state->registers[rt] = state->memory[state->registers[rs] + immediate];
                break;
            // Add cases for other instructions
            // ...
            default:
                // Handle unknown instruction
                // ...

        }

        // Increment the program counter
        state->programCounter++;
    }

    // Call printState before exiting
    printState(state);
}

int main() {
    struct stateStruct state;
    initializeState(&state);

    // Example: Load machine code into memory (replace this with actual machine code)
    state.memory[0] = 0x20090001; // Example LW instruction
    state.memory[1] = 0x0000000A; // Memory value at address 10

    state.numMemory = 2;

    // Execute machine code
    executeMachineCode(&state);

    return 0;
}
