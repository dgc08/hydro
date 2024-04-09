#include <stdio.h>

int main() {
    int myInt = 42; // Define an integer and initialize it with a value (e.g., 42)

    // Inline assembly to move 0 to all registers
    asm volatile (
        "xor %%eax, %%eax \n\t"
        "xor %%ebx, %%ebx \n\t"
        "xor %%ecx, %%ecx \n\t"
        "xor %%edx, %%edx \n\t"
        :
        :
        : "eax", "ebx", "ecx", "edx"
    );

    printf("myInt: %d\n", myInt); // Print the integer
    return 0;
}
