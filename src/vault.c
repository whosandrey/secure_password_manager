#define _POSIX_C_SOURCE 200809L

#include "vault.h"

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sodium.h>

#define VAULT_VERSION 2U
#define VAULT_MAGIC_SIZE 8U
#define STORED_U64_SIZE 8U

#define VAULT_PAYLOAD_SIZE             \
    (VAULT_SERVICE_SIZE              \
     + VAULT_USERNAME_SIZE           \
     + VAULT_PASSWORD_SIZE)

#define VAULT_HEADER_SIZE                                      \
    (VAULT_MAGIC_SIZE + 2U + STORED_U64_SIZE + STORED_U64_SIZE \
     + crypto_pwhash_SALTBYTES                                 \
     + crypto_aead_xchacha20poly1305_ietf_NPUBBYTES            \
     + STORED_U64_SIZE)

#define VAULT_CIPHERTEXT_SIZE \
    (VAULT_PAYLOAD_SIZE \
     + crypto_aead_xchacha20poly1305_ietf_ABYTES)

static const unsigned char VAULT_MAGIC[VAULT_MAGIC_SIZE] = {
    'S', 'P', 'M', 'V', 'A', 'U', 'L', 'T'
};

static int write_all(
    int file_descriptor,
    const unsigned char *data,
    size_t data_size)
{
    size_t written = 0;

    while (written < data_size) {
        ssize_t result = write(
            file_descriptor,
            data + written,
            data_size - written
        );

        if (result < 0) {
            if (errno == EINTR) {
                continue;
            }

            return -1;
        }

        if (result == 0) {
            return -1;
        }

        written += (size_t) result;
    }

    return 0;
}

static int read_all(
    int file_descriptor,
    unsigned char *data,
    size_t data_size)
{
    size_t received = 0;

    while (received < data_size) {
        ssize_t result = read(
            file_descriptor,
            data + received,
            data_size - received
        );

        if (result < 0) {
            if (errno == EINTR) {
                continue;
            }

            return -1;
        }

        if (result == 0) {
            return -1;
        }

        received += (size_t) result;
    }

    return 0;
}

static void store_u64_le(
    unsigned char output[STORED_U64_SIZE],
    uint64_t value)
{
    size_t index;

    for (index = 0; index < STORED_U64_SIZE; index++) {
        output[index] = (unsigned char) (value & 0xffU);
        value >>= 8U;
    }
}

static uint64_t load_u64_le(
    const unsigned char input[STORED_U64_SIZE])
{
    uint64_t value = 0;
    size_t index;

    for (index = 0; index < STORED_U64_SIZE; index++) {
        value |= ((uint64_t) input[index]) << (index * 8U);
    }

    return value;
}

static int field_is_valid(
    const char *field,
    size_t field_size)
{
    if (field == NULL || field[0] == '\0') {
        return 0;
    }

    return memchr(field, '\0', field_size) != NULL;
}

static int record_is_valid(
    const struct VaultRecord *record)
{
    if (record == NULL) {
        return 0;
    }

    return field_is_valid(
               record->service,
               sizeof record->service
           )
        && field_is_valid(
               record->username,
               sizeof record->username
           )
        && field_is_valid(
               record->password,
               sizeof record->password
           );
}

static void serialize_record(
    const struct VaultRecord *record,
    unsigned char payload[VAULT_PAYLOAD_SIZE])
{
    size_t offset = 0;
    size_t length;

    memset(payload, 0, VAULT_PAYLOAD_SIZE);

    length = strnlen(
        record->service,
        sizeof record->service
    );
    memcpy(payload + offset, record->service, length);
    offset += VAULT_SERVICE_SIZE;

    length = strnlen(
        record->username,
        sizeof record->username
    );
    memcpy(payload + offset, record->username, length);
    offset += VAULT_USERNAME_SIZE;

    length = strnlen(
        record->password,
        sizeof record->password
    );
    memcpy(payload + offset, record->password, length);
}

static int deserialize_record(
    const unsigned char payload[VAULT_PAYLOAD_SIZE],
    struct VaultRecord *record)
{
    size_t service_offset = 0;
    size_t username_offset = VAULT_SERVICE_SIZE;
    size_t password_offset =
        VAULT_SERVICE_SIZE + VAULT_USERNAME_SIZE;

    if (record == NULL) {
        return -1;
    }

    if (memchr(
            payload + service_offset,
            '\0',
            VAULT_SERVICE_SIZE) == NULL ||
        memchr(
            payload + username_offset,
            '\0',
            VAULT_USERNAME_SIZE) == NULL ||
        memchr(
            payload + password_offset,
            '\0',
            VAULT_PASSWORD_SIZE) == NULL) {
        return -1;
    }

    memset(record, 0, sizeof *record);

    memcpy(
        record->service,
        payload + service_offset,
        VAULT_SERVICE_SIZE
    );

    memcpy(
        record->username,
        payload + username_offset,
        VAULT_USERNAME_SIZE
    );

    memcpy(
        record->password,
        payload + password_offset,
        VAULT_PASSWORD_SIZE
    );

    if (!record_is_valid(record)) {
        vault_record_clear(record);
        return -1;
    }

    return 0;
}

void vault_record_clear(struct VaultRecord *record)
{
    if (record != NULL) {
        sodium_memzero(record, sizeof *record);
    }
}

int vault_create(
    const char *path,
    const char *master_password,
    const struct VaultRecord *record)
{
    unsigned char header[VAULT_HEADER_SIZE];
    unsigned char payload[VAULT_PAYLOAD_SIZE];
    unsigned char salt[crypto_pwhash_SALTBYTES];
    unsigned char nonce[
        crypto_aead_xchacha20poly1305_ietf_NPUBBYTES
    ];
    unsigned char key[
        crypto_aead_xchacha20poly1305_ietf_KEYBYTES
    ] = {0};
    unsigned char ciphertext[VAULT_CIPHERTEXT_SIZE];

    unsigned long long ciphertext_size = 0;
    unsigned long long operations =
        crypto_pwhash_OPSLIMIT_INTERACTIVE;
    size_t memory = crypto_pwhash_MEMLIMIT_INTERACTIVE;

    size_t offset = 0;
    int file_descriptor = -1;
    int status = VAULT_FAILURE;

    if (path == NULL ||
        master_password == NULL ||
        master_password[0] == '\0' ||
        !record_is_valid(record)) {
        return VAULT_FAILURE;
    }

    serialize_record(record, payload);

    randombytes_buf(salt, sizeof salt);
    randombytes_buf(nonce, sizeof nonce);

    memcpy(header + offset, VAULT_MAGIC, VAULT_MAGIC_SIZE);
    offset += VAULT_MAGIC_SIZE;

    header[offset++] = VAULT_VERSION;
    header[offset++] =
        (unsigned char) crypto_pwhash_ALG_ARGON2ID13;

    store_u64_le(header + offset, (uint64_t) operations);
    offset += STORED_U64_SIZE;

    store_u64_le(header + offset, (uint64_t) memory);
    offset += STORED_U64_SIZE;

    memcpy(header + offset, salt, sizeof salt);
    offset += sizeof salt;

    memcpy(header + offset, nonce, sizeof nonce);
    offset += sizeof nonce;

    store_u64_le(
        header + offset,
        (uint64_t) VAULT_CIPHERTEXT_SIZE
    );

    if (crypto_pwhash(
            key,
            sizeof key,
            master_password,
            strlen(master_password),
            salt,
            operations,
            memory,
            crypto_pwhash_ALG_ARGON2ID13) != 0) {
        sodium_memzero(payload, sizeof payload);
        sodium_memzero(key, sizeof key);
        return VAULT_FAILURE;
    }

    if (crypto_aead_xchacha20poly1305_ietf_encrypt(
            ciphertext,
            &ciphertext_size,
            payload,
            sizeof payload,
            header,
            sizeof header,
            NULL,
            nonce,
            key) != 0 ||
        ciphertext_size != VAULT_CIPHERTEXT_SIZE) {
        sodium_memzero(payload, sizeof payload);
        sodium_memzero(key, sizeof key);
        return VAULT_FAILURE;
    }

    file_descriptor = open(
        path,
        O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC,
        S_IRUSR | S_IWUSR
    );

    if (file_descriptor < 0) {
        sodium_memzero(payload, sizeof payload);
        sodium_memzero(key, sizeof key);

        if (errno == EEXIST) {
            return VAULT_ALREADY_EXISTS;
        }

        return VAULT_FAILURE;
    }

    if (write_all(
            file_descriptor,
            header,
            sizeof header) != 0 ||
        write_all(
            file_descriptor,
            ciphertext,
            (size_t) ciphertext_size) != 0 ||
        fsync(file_descriptor) != 0) {
        goto cleanup;
    }

    if (close(file_descriptor) != 0) {
        file_descriptor = -1;
        goto cleanup;
    }

    file_descriptor = -1;
    status = VAULT_SUCCESS;

cleanup:
    if (file_descriptor >= 0) {
        close(file_descriptor);
    }

    if (status != VAULT_SUCCESS) {
        unlink(path);
    }

    sodium_memzero(payload, sizeof payload);
    sodium_memzero(key, sizeof key);
    return status;
}

int vault_unlock(
    const char *path,
    const char *master_password,
    struct VaultRecord *record)
{
    unsigned char header[VAULT_HEADER_SIZE];
    unsigned char ciphertext[VAULT_CIPHERTEXT_SIZE];
    unsigned char payload[VAULT_PAYLOAD_SIZE];
    unsigned char salt[crypto_pwhash_SALTBYTES];
    unsigned char nonce[
        crypto_aead_xchacha20poly1305_ietf_NPUBBYTES
    ];
    unsigned char key[
        crypto_aead_xchacha20poly1305_ietf_KEYBYTES
    ] = {0};

    unsigned long long payload_size = 0;
    uint64_t stored_operations;
    uint64_t stored_memory;
    uint64_t stored_ciphertext_size;

    size_t offset = 0;
    int file_descriptor;
    int status = VAULT_FAILURE;
    struct stat file_information;

    if (record != NULL) {
        vault_record_clear(record);
    }

    if (path == NULL ||
        master_password == NULL ||
        master_password[0] == '\0' ||
        record == NULL) {
        return VAULT_FAILURE;
    }

    file_descriptor = open(
        path,
        O_RDONLY | O_NOFOLLOW | O_CLOEXEC
    );

    if (file_descriptor < 0) {
        if (errno == ENOENT) {
            return VAULT_DOES_NOT_EXIST;
        }

        return VAULT_FAILURE;
    }

    if (fstat(file_descriptor, &file_information) != 0 ||
        !S_ISREG(file_information.st_mode) ||
        file_information.st_size !=
            (off_t) (
                VAULT_HEADER_SIZE + VAULT_CIPHERTEXT_SIZE
            ) ||
        read_all(
            file_descriptor,
            header,
            sizeof header) != 0 ||
        read_all(
            file_descriptor,
            ciphertext,
            sizeof ciphertext) != 0) {
        close(file_descriptor);
        return VAULT_FAILURE;
    }

    if (close(file_descriptor) != 0) {
        return VAULT_FAILURE;
    }

    if (memcmp(
            header + offset,
            VAULT_MAGIC,
            VAULT_MAGIC_SIZE) != 0) {
        return VAULT_FAILURE;
    }

    offset += VAULT_MAGIC_SIZE;

    if (header[offset++] != VAULT_VERSION) {
        return VAULT_FAILURE;
    }

    if (header[offset++] !=
        (unsigned char) crypto_pwhash_ALG_ARGON2ID13) {
        return VAULT_FAILURE;
    }

    stored_operations = load_u64_le(header + offset);
    offset += STORED_U64_SIZE;

    stored_memory = load_u64_le(header + offset);
    offset += STORED_U64_SIZE;

    memcpy(salt, header + offset, sizeof salt);
    offset += sizeof salt;

    memcpy(nonce, header + offset, sizeof nonce);
    offset += sizeof nonce;

    stored_ciphertext_size = load_u64_le(header + offset);

    if (stored_operations < crypto_pwhash_OPSLIMIT_MIN ||
        stored_operations > crypto_pwhash_OPSLIMIT_SENSITIVE ||
        stored_memory < crypto_pwhash_MEMLIMIT_MIN ||
        stored_memory > crypto_pwhash_MEMLIMIT_SENSITIVE ||
        stored_ciphertext_size != VAULT_CIPHERTEXT_SIZE) {
        return VAULT_FAILURE;
    }

    if (crypto_pwhash(
            key,
            sizeof key,
            master_password,
            strlen(master_password),
            salt,
            (unsigned long long) stored_operations,
            (size_t) stored_memory,
            crypto_pwhash_ALG_ARGON2ID13) != 0) {
        sodium_memzero(key, sizeof key);
        return VAULT_FAILURE;
    }

    if (crypto_aead_xchacha20poly1305_ietf_decrypt(
            payload,
            &payload_size,
            NULL,
            ciphertext,
            sizeof ciphertext,
            header,
            sizeof header,
            nonce,
            key) != 0) {
        status = VAULT_AUTH_FAILED;
        goto cleanup;
    }

    if (payload_size != VAULT_PAYLOAD_SIZE ||
        deserialize_record(payload, record) != 0) {
        status = VAULT_FAILURE;
        goto cleanup;
    }

    status = VAULT_SUCCESS;

cleanup:
    sodium_memzero(payload, sizeof payload);
    sodium_memzero(key, sizeof key);

    if (status != VAULT_SUCCESS) {
        vault_record_clear(record);
    }

    return status;
}