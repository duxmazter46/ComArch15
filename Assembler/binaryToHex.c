#include <stdio.h>
#include <stdlib.h>

// Function to cast a binary 32-bit integer array to a character array
char* intArrayToCharArray(const int intArray[32]) {
    char* charArray = (char*)malloc(33); // 32 bits + null terminator
    if (charArray == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }

    for (int i = 0; i < 32; i++) {
        charArray[i] = intArray[i] ? '1' : '0';
    }

    charArray[32] = '\0'; // Null-terminate the string

    return charArray;
}

// Function to convert binary representation to hexadecimal and print
void convertAndPrint(int binaryArray[32]) {
    char* charArray = intArrayToCharArray(binaryArray);

    printf("Binary: %s\n", charArray);

    // Convert binary string to hexadecimal
    unsigned int hexValue = (unsigned int)strtoul(charArray, NULL, 2);
    sprintf(charArray, "%08X", hexValue);

    printf("Hexadecimal: %s\n", charArray);

    // Convert hexadecimal string back to binary
    unsigned int newBinaryValue = (unsigned int)strtoul(charArray, NULL, 16);
    for (int i = 31; i >= 0; i--) {
        binaryArray[i] = (newBinaryValue >> (31 - i)) & 1;
    }

    // Print the converted binary value
    char* newCharArray = intArrayToCharArray(binaryArray);
    printf("Binary (Converted Back): %s\n", newCharArray);

    free(charArray);
    free(newCharArray);
}

int main() {
    int binaryArray[32] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1};

    convertAndPrint(binaryArray);

    return 0;
}
