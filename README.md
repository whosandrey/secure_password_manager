# secure_password_manager

Semester project for ICS0022 Secure Programming at TalTech,
Autumn 2026.

## Project status

It can create an encrypted vault locally, which can have one credential.
The encrypted vault can only be accessed with the right master password.

The service name and username are displayed after unlocking.
The password stored will not be displayed in clear text.

This project is for educational purposes only, and should not be used
to store any real passwords.

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

The project was built and tested on Ubuntu using WSL 2.

### Dependencies

```bash
sudo apt update
sudo apt install build-essential git gdb libsodium-dev pkg-config
```

### Build

Run from the repository root:

```bash
make
```

### Run

```bash
make run
```

The vault file is created as `passwords.vault` in the current directory.
It is excluded from Git.

### Sanitizer build

```bash
make sanitize
./build/password-manager-sanitize
```

### Clean generated files

```bash
make clean
```
## Current limitations

- The prototype stores one credential.
- Credentials cannot yet be updated or deleted.
- The stored password is always masked after unlocking.
- The vault location is currently fixed to `passwords.vault`.
- Only the Linux command-line environment is supported.


## Design documents

- [Threat model](docs/threat-model.md)
- [Architecture](docs/architecture.md)
- [Checkpoint 2 progress](docs/checkpoint-2-progress.md)

*ChatGPT by OpenAI helped with unfamiliar terms, assist with English language, and improved Markdown formatting.  I verified the build instructions by running them on Ubuntu using WSL 2*
