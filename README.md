# secure_password_manager

Semester project for ICS0022 Secure Programming at TalTech,
Autumn 2026.

## Project status

An initial C prototype builds and initializes libsodium.
Vault storage, authentication and credential management are not implemented yet.
This is an educational project and must not be used to store real credentials.

## Objective

Develop a local password manager that securely stores credentials
and demonstrates secure programming practices.

## Planned features

- Master-password authentication.
- Encrypted storage of service names, usernames and passwords.
- Adding, retrieving, updating and deleting credentials.
- Per-user access control for every record operation.
- A command-line interface that does not display plaintext passwords.
- Input validation and safe handling of malformed data.
- Explicit cleanup of sensitive memory buffers.
- Structured logging without passwords, keys or other secrets.
- Security and functionality tests.

The credential retrieval mechanism and user-isolation model
will be clarified with the lecturer before implementation.

## Proposed technologies

- C17 as the implementation language.
- libsodium for cryptography and sensitive-memory utilities.
- Linux as the target environment.

The vault format, cryptographic scheme and build system
will be documented during the architecture stage.

## Planned architecture

- Interface: command handling and input validation.
- Authentication and access control: vault unlocking and record ownership.
- Cryptography: key derivation, encryption and decryption.
- Storage: reading and safely updating encrypted vault data.

## Build and run

## Build and run

The current prototype initializes libsodium and prints a startup message.
Vault storage and password management are not implemented yet.

### Dependencies

On Ubuntu:

```bash
sudo apt update
sudo apt install build-essential git gdb libsodium-dev pkg-config
```

### Build

Run from the repository root:

```bash
mkdir -p build
gcc -std=c17 -Wall -Wextra -Wpedantic -g src/main.c -o build/password-manager $(pkg-config --cflags --libs libsodium)
```

### Run

```bash
./build/password-manager
```

## Design documents

- [Threat model](docs/threat-model.md)
- [Architecture](docs/architecture.md)
