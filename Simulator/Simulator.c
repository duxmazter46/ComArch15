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

// Define the number of registers
#define NUM_REGISTERS 32

// Define the size of the memory
#define MEMORY_SIZE 1024

// Define the instruction set opcodes
#define ADD 0x20
#define SUB 0x22
#define LW 0x23
#define SW 0x2B
#define BEQ 0x04
#define HALT 0x3F

// Define the machine's registers and memory
uint32_t registers[NUM_REGISTERS];
uint32_t memory[MEMORY_SIZE];

// Sign extension function for 16-bit 2's complement numbers
int32_t signExtend(uint16_t value) {
    if (value & 0x8000) {
        return (int32_t)value | 0xFFFF0000;
    }
    return value;
}

// Function to load machine code into memory
void loadMachineCode(uint32_t* machineCode, int codeSize) {
    for (int i = 0; i < codeSize; i++) {
        memory[i] = machineCode[i];
    }
}

// Function to simulate machine code execution
void executeMachineCode() {
    uint32_t programCounter = 0;
    
    while (1) {
        uint32_t instruction = memory[programCounter];
        uint32_t opcode = instruction >> 26;
        
        // Extract fields from the instruction
        uint32_t rs = (instruction >> 21) & 0x1F;
        uint32_t rt = (instruction >> 16) & 0x1F;
        uint32_t rd = (instruction >> 11) & 0x1F;
        uint32_t immediate = instruction & 0xFFFF;
        
        switch (opcode) {
            case ADD:
                registers[rd] = registers[rs] + registers[rt];
                break;
            case SUB:
                registers[rd] = registers[rs] - registers[rt];
                break;
            case LW:
                registers[rt] = memory[registers[rs] + signExtend(immediate) / 4];
                break;
            case SW:
                memory[registers[rs] + signExtend(immediate) / 4] = registers[rt];
                break;
            case BEQ:
                if (registers[rs] == registers[rt]) {
                    programCounter += signExtend(immediate) << 2;
                }
                break;
            case HALT:
                return;
        }
        
        // Increment the program counter
        programCounter++;
    }
}

// Function to print the current state of the machine
void printState() {
    // Print register values, memory contents, and program counter
    // You can implement this part according to your requirements
}

int main() {
    // Example machine code (you need to replace this with your actual code)
    uint32_t machineCode[] = { 0x22090001, 0x024B4820, 0x8C4A0000, 0xAC4A0000, 0x106A0001, 0x0000003F };
    
    // Initialize registers to zero
    for (int i = 0; i < NUM_REGISTERS; i++) {
        registers[i] = 0;
    }
    
    // Load machine code into memory
    int codeSize = sizeof(machineCode) / sizeof(machineCode[0]);
    loadMachineCode(machineCode, codeSize);
    
    // Execute the machine code
    executeMachineCode();
    
    // Print the final state of the machine
    printState();
    
    return 0;
}
