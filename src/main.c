#include <stdio.h>
#include <sodium.h>

#include "input.h"

#define MASTER_PASSWORD_MAX 256

int main(void)
{
    int option;
    int character;
    char master_password[MASTER_PASSWORD_MAX] = {0};

    if (sodium_init() < 0) {
        fprintf(stderr, "Failed to initialize the application.\n");
        return 1;
    }

    printf("Secure Password Manager\n");
    printf("Cryptographic library initialized.\n");
    printf("\n");
    printf("1. Create vault\n");
    printf("2. Unlock vault\n");
    printf("3. Exit\n");
    printf("Choose an option: ");

    if (scanf("%d", &option) != 1) {
        fprintf(stderr, "Invalid input.\n");
        return 1;
    }

    while ((character = getchar()) != '\n' && character != EOF) {
}

    switch (option) {
        case 1:
            if (get_hidden_input(
                    "Create master password: ",
                    master_password,
                    sizeof master_password) != 0) {
                fprintf(stderr, "Unable to read master password.\n");
                return 1;
            }

            printf("Master password received. Vault creation will be added next.\n");
            sodium_memzero(master_password, sizeof master_password);
            break;

        case 2:
            if (get_hidden_input(
                    "Enter master password: ",
                    master_password,
                    sizeof master_password) != 0) {
                fprintf(stderr, "Unable to read master password.\n");
                return 1;
            }

            printf("Master password received. Vault unlocking will be added next.\n");
            sodium_memzero(master_password, sizeof master_password);
            break;

        case 3:
            printf("Goodbye.\n");
            break;

        default:
            printf("Invalid option.\n");
            break;
    }

    sodium_memzero(master_password, sizeof master_password);
    return 0;
}