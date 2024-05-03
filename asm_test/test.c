#include <stdio.h>

int main() {
    int myInt = 42; // Define an integer and initialize it with a value (e.g., 42)
    int myInt2 = 77; // Define an integer and initialize it with a value (e.g., 42)
    int myInt3 = myInt/myInt2; // Define an integer and initialize it with a value (e.g., 42)
    int myInt4 = 44; // Define an integer and initialize it with a value (e.g., 42)
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

    if (myInt==4) myInt4=2;

    printf("myInt: %d\n", myInt); // Print the integer
    printf("myInt2: %d\n", myInt2); // Print the integer
    printf("myInt3: %d\n", myInt3); // Print the integer
    printf("myInt4: %d\n", myInt4); // Print the integer
    return 0;
}
