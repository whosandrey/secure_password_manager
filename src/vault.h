#ifndef VAULT_H
#define VAULT_H

#define DEFAULT_VAULT_FILE "passwords.vault"

#define VAULT_SERVICE_SIZE 64
#define VAULT_USERNAME_SIZE 64
#define VAULT_PASSWORD_SIZE 128

struct VaultRecord {
    char service[VAULT_SERVICE_SIZE];
    char username[VAULT_USERNAME_SIZE];
    char password[VAULT_PASSWORD_SIZE];
};

enum VaultStatus {
    VAULT_SUCCESS = 0,
    VAULT_FAILURE = -1,
    VAULT_ALREADY_EXISTS = -2,
    VAULT_DOES_NOT_EXIST = -3,
    VAULT_AUTH_FAILED = -4
};

int vault_create(
    const char *path,
    const char *master_password,
    const struct VaultRecord *record
);

int vault_unlock(
    const char *path,
    const char *master_password,
    struct VaultRecord *record
);

void vault_record_clear(struct VaultRecord *record);

#endif