#include <stdio.h>
#include <sodium.h>

#include "input.h"
#include "vault.h"

#define MASTER_PASSWORD_MAX 256

int main(void)
{
    int option;
    int character;
    int vault_status;

    char master_password[MASTER_PASSWORD_MAX] = {0};
    char password_confirmation[MASTER_PASSWORD_MAX] = {0};
    struct VaultRecord record = {0};

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

            if (get_hidden_input(
                    "Confirm master password: ",
                    password_confirmation,
                    sizeof password_confirmation) != 0) {
                sodium_memzero(
                    master_password,
                    sizeof master_password
                );
                fprintf(
                    stderr,
                    "Unable to read password confirmation.\n"
                );
                return 1;
            }

            if (sodium_memcmp(
                    master_password,
                    password_confirmation,
                    sizeof master_password) != 0) {
                fprintf(stderr, "Master passwords do not match.\n");

                sodium_memzero(
                    master_password,
                    sizeof master_password
                );
                sodium_memzero(
                    password_confirmation,
                    sizeof password_confirmation
                );

                break;
            }

            if (get_text_input(
                    "Service: ",
                    record.service,
                    sizeof record.service) != 0) {
                fprintf(stderr, "Invalid service name.\n");

                sodium_memzero(
                    master_password,
                    sizeof master_password
                );
                sodium_memzero(
                    password_confirmation,
                    sizeof password_confirmation
                );
                vault_record_clear(&record);

                return 1;
            }

            if (get_text_input(
                    "Username: ",
                    record.username,
                    sizeof record.username) != 0) {
                fprintf(stderr, "Invalid username.\n");

                sodium_memzero(
                    master_password,
                    sizeof master_password
                );
                sodium_memzero(
                    password_confirmation,
                    sizeof password_confirmation
                );
                vault_record_clear(&record);

                return 1;
            }

            if (get_hidden_input(
                    "Password to store: ",
                    record.password,
                    sizeof record.password) != 0) {
                fprintf(stderr, "Invalid credential password.\n");

                sodium_memzero(
                    master_password,
                    sizeof master_password
                );
                sodium_memzero(
                    password_confirmation,
                    sizeof password_confirmation
                );
                vault_record_clear(&record);

                return 1;
            }

            vault_status = vault_create(
                DEFAULT_VAULT_FILE,
                master_password,
                &record
            );

            sodium_memzero(
                master_password,
                sizeof master_password
            );
            sodium_memzero(
                password_confirmation,
                sizeof password_confirmation
            );
            vault_record_clear(&record);

            if (vault_status == VAULT_SUCCESS) {
                printf("Encrypted vault created successfully.\n");
            } else if (vault_status == VAULT_ALREADY_EXISTS) {
                fprintf(stderr, "Vault already exists.\n");
            } else {
                fprintf(stderr, "Unable to create vault.\n");
            }

            break;

        case 2:
            if (get_hidden_input(
                    "Enter master password: ",
                    master_password,
                    sizeof master_password) != 0) {
                fprintf(stderr, "Unable to read master password.\n");
                return 1;
            }

            vault_status = vault_unlock(
                DEFAULT_VAULT_FILE,
                master_password,
                &record
            );

            sodium_memzero(
                master_password,
                sizeof master_password
            );

            if (vault_status == VAULT_SUCCESS) {
                printf("Vault unlocked successfully.\n");
                printf("Service: %s\n", record.service);
                printf("Username: %s\n", record.username);
                printf("Password: ********\n");
            } else if (vault_status == VAULT_DOES_NOT_EXIST) {
                fprintf(stderr, "Vault does not exist.\n");
            } else if (vault_status == VAULT_AUTH_FAILED) {
                fprintf(
                    stderr,
                    "Incorrect master password or damaged vault.\n"
                );
            } else {
                fprintf(stderr, "Unable to unlock vault.\n");
            }

            vault_record_clear(&record);
            break;

        case 3:
            printf("Goodbye.\n");
            break;

        default:
            printf("Invalid option.\n");
            break;
    }

    sodium_memzero(master_password, sizeof master_password);
    sodium_memzero(
        password_confirmation,
        sizeof password_confirmation
    );
    vault_record_clear(&record);

    return 0;
}