#include <assert.h>
#include <sodium.h>
#include <string.h>

#include "vault.h"

int main(void)
{
    if (sodium_init() < 0) {
        return 1;
    }

    const char *path = "passwords.vault";
    const char *master_password = "test-master-password";

    struct VaultData initial = {
        .count = 1,
        .records = {
            {
                .service = "demo",
                .username = "alice",
                .password = "fake-pass-a"
            }
        }
    };

    const struct VaultRecord second = {
        .service = "mail",
        .username = "bob",
        .password = "fake-pass-b"
    };

    struct VaultData reopened = {0};

    int status = vault_create(path, master_password, &initial);
    assert(status == VAULT_SUCCESS);

    status = vault_add_entry(path, master_password, &second);
    assert(status == VAULT_SUCCESS);

    status = vault_unlock(path, master_password, &reopened);
    assert(status == VAULT_SUCCESS);

    assert(reopened.count == 2);
    assert(strcmp(reopened.records[0].username, "alice") == 0);
    assert(strcmp(reopened.records[1].username, "bob") == 0);

    status = vault_add_entry(path, "wrong-master-password", &second);
    assert(status == VAULT_AUTH_FAILED);

    vault_data_clear(&reopened);
    status = vault_unlock(path, master_password, &reopened);
    assert(status == VAULT_SUCCESS);
    assert(reopened.count == 2);
    assert(strcmp(reopened.records[0].username, "alice") == 0);
    assert(strcmp(reopened.records[1].username, "bob") == 0);

    status = vault_unlock(path, "wrong-master-password", &reopened);
    assert(status == VAULT_AUTH_FAILED);
    assert(sodium_is_zero(
        (const unsigned char *)&reopened, sizeof reopened
    ) == 1);

    struct VaultRecord invalid = second;

    invalid.service[0] = '\0';
    status = vault_add_entry(path, master_password, &invalid);
    assert(status == VAULT_FAILURE);

    memset(invalid.service, 'A', sizeof invalid.service);
    status = vault_add_entry(path, master_password, &invalid);
    assert(status == VAULT_FAILURE);

    vault_record_clear(&invalid);

    vault_data_clear(&initial);
    vault_data_clear(&reopened);

    return 0;
}