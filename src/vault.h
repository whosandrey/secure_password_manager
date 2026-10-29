#ifndef VAULT_H
#define VAULT_H

#define DEFAULT_VAULT_FILE "passwords.vault"

enum VaultStatus {
    VAULT_SUCCESS = 0,
    VAULT_FAILURE = -1,
    VAULT_ALREADY_EXISTS = -2,
    VAULT_DOES_NOT_EXIST = -3,
    VAULT_AUTH_FAILED = -4
};

int vault_create(const char *path, const char *master_password);
int vault_unlock(const char *path, const char *master_password);

#endif