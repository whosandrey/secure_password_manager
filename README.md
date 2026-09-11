# secure_password_manager

Semester project for ICS0022 Secure Programming at TalTech,
Autumn 2026.

## Project status

Planning and architecture stage. The application is not implemented yet.
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

Not available yet. Reproducible build and usage instructions
will be added when the initial implementation is ready.
