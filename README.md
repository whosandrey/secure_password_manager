# secure_password_manager

Semester project for ICS0022 Secure Programming at TalTech,
Autumn 2026.

## Project status

This C-language project prototype allows you to initialize the libsodium libraries.
Data storage in Vault, authentication, and account management.
This project is intended for academic purposes and should not be used to store real passwords.

## Objective

Create a local password manager that allows you to securely store usernames and passwords

## Planned features

- Authentication using a master password.
- Encrypted storage of usernames and passwords.
- User and password management.
- A CLI that does not display passwords in plain text.
- Validation of input data and safe handling of erroneous data.
- Explicit clearing of memory buffers containing secrets.
- Structured logging without revealing passwords, keys, or other sensitive data.
- Security and functionality tests.

## Planned CLI menu

The application will use an interactive CLI.

In lock mode:
- Create a vault
- Unlock an existing vault using the master password.
- Log out.

In unlock mode:
- List users with passwords masked.
- Add a credential.
- Get a credential.
- Update a credential.
- Delete a credential.
- Lock the vault and return to the locked menu.

## Proposed technologies

- C17 as the coding language.
- libsodium for cryptography and sensitive memory utilities.
- Linux as the target environment.

## Planned architecture

- Interface: command handling and input validation.
- Authentication and access control.
- Cryptography: key displaying, encryption and decryption.
- Storage: reading and safely updating encrypted vault data.

## Build and run

Project was built and tested on Ubuntu WSL 2

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

*ChatGPT by OpenAI helped with unfamiliar terms, assist with English language, and improved Markdown formatting.  I verified the build instructions by running them on Ubuntu using WSL 2*
