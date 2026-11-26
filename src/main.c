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
                case 1: {
            struct VaultData data = {0};
            const size_t max_entries =
                sizeof data.records / sizeof data.records[0];
            int input_ok = 1;

            if (get_hidden_input(
                    "Create master password: ",
                    master_password,
                    sizeof master_password) != 0) {
                sodium_memzero(master_password, sizeof master_password);
                fprintf(stderr, "Unable to read master password.\n");
                return 1;
            }

            if (get_hidden_input(
                    "Confirm master password: ",
                    password_confirmation,
                    sizeof password_confirmation) != 0) {
                sodium_memzero(master_password, sizeof master_password);
                sodium_memzero(password_confirmation, sizeof password_confirmation);
                fprintf(stderr, "Unable to read password confirmation.\n");
                return 1;
            }

            int passwords_match = sodium_memcmp(
                master_password,
                password_confirmation,
                sizeof master_password
            ) == 0;
            sodium_memzero(password_confirmation, sizeof password_confirmation);

            if (!passwords_match) {
                sodium_memzero(master_password, sizeof master_password);
                fprintf(stderr, "Master passwords do not match.\n");
                break;
            }

            while (data.count < max_entries) {
                struct VaultRecord *entry = &data.records[data.count];
                printf("\nEntry %u\n", data.count + 1);

                if (get_text_input("Service: ", entry->service,
                                   sizeof entry->service) != 0 ||
                    get_text_input("Username: ", entry->username,
                                   sizeof entry->username) != 0 ||
                    get_hidden_input("Password to store: ", entry->password,
                                     sizeof entry->password) != 0) {
                    fprintf(stderr, "Invalid or incomplete entry.\n");
                    input_ok = 0;
                    break;
                }

                ++data.count;

                if (data.count == max_entries) {
                    printf("Entry limit reached.\n");
                    break;
                }

                char answer[8] = {0};
                if (get_text_input("Add another entry? (y/n): ",
                                   answer, sizeof answer) != 0 ||
                    answer[1] != '\0') {
                    fprintf(stderr, "Invalid answer. Vault was not created.\n");
                    input_ok = 0;
                    break;
                }

                if (answer[0] == 'n' || answer[0] == 'N') {
                    break;
                }
                if (answer[0] != 'y' && answer[0] != 'Y') {
                    fprintf(stderr, "Invalid answer. Vault was not created.\n");
                    input_ok = 0;
                    break;
                }
            }

            if (input_ok) {
                vault_status = vault_create(
                    DEFAULT_VAULT_FILE, master_password, &data
                );
            }

            vault_data_clear(&data);
            sodium_memzero(master_password, sizeof master_password);

            if (!input_ok) {
                return 1;
            }

            if (vault_status == VAULT_SUCCESS) {
                printf("Encrypted vault created successfully.\n");
            } else if (vault_status == VAULT_ALREADY_EXISTS) {
                fprintf(stderr, "Vault already exists.\n");
            } else {
                fprintf(stderr, "Unable to create vault.\n");
            }

            break;
        }


                case 2: {
            if (get_hidden_input(
                    "Enter master password: ",
                    master_password,
                    sizeof master_password) != 0) {
                sodium_memzero(master_password, sizeof master_password);
                fprintf(stderr, "Unable to read master password.\n");
                return 1;
            }

            struct VaultData unlocked_data = {0};
            vault_status = vault_unlock(
                DEFAULT_VAULT_FILE,
                master_password,
                &unlocked_data
            );

            sodium_memzero(master_password, sizeof master_password);

            if (vault_status == VAULT_SUCCESS) {
                printf("Vault unlocked successfully.\n");

                if (unlocked_data.count == 0) {
                    printf("Vault is empty.\n");
                }

                for (unsigned int i = 0; i < unlocked_data.count; ++i) {
                    printf("\nEntry %u\n", i + 1);
                    printf("Service: %s\n", unlocked_data.records[i].service);
                    printf("Username: %s\n", unlocked_data.records[i].username);
                    printf("Password: ********\n");
                }
            } else if (vault_status == VAULT_DOES_NOT_EXIST) {
                fprintf(stderr, "Vault does not exist.\n");
            } else if (vault_status == VAULT_AUTH_FAILED) {
                fprintf(stderr, "Incorrect master password or damaged vault.\n");
            } else {
                fprintf(stderr, "Unable to unlock vault.\n");
            }

            vault_data_clear(&unlocked_data);
            break;
        }


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