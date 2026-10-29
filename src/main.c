#include <stdio.h>
#include <sodium.h>

int main(void)
{
    int choice;

    if (sodium_init() < 0) {
        fprintf(stderr, "Failed to initialize the application.\n");
        return 1;
    }

    printf("Secure Password Manager");
    printf("Cryptographic library initialized.");
    printf("\n");
    printf("1. Create vault");
    printf("2. Unlock vault");
    printf("3. Exit");
    printf("Choose an option: ");

    if (scanf("%d", &choice) != 1) {
        fprintf(stderr, "Invalid input.\n");
        return 1;
    }

    switch (choice) {
        case 1:
            printf("Create vault is not implemented yet.");
            break;

        case 2:
            printf("Unlock vault is not implemented yet.");
            break;

        case 3:
            printf("Goodbye.");
            break;

        default:
            printf("Invalid option.");
            break;
    }

    return 0;
}