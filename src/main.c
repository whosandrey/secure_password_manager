#include <stdio.h>
#include <sodium.h>

int main(void)
{
    if (sodium_init() < 0) {
        fputs("Unable to initialize the application.\n", stderr);
        return 1;
    }

    puts("Secure Password Manager");
    puts("Cryptographic library initialized.");
    puts("Development version: vault functionality is not implemented.");

    return 0;
}