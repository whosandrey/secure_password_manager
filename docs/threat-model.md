# Threat Model

## Status

Initial design exists for Checkpoint 1.
The controls are not implemented yet.

## Scope

A password manager for an user on Linux, written in C.
The user unlocks the encrypted vault using a master password.
Credentials are managed through a command line interface.

## Assets

- Stored. Username.
- The master. Its derived encryption keys.
- The integrity and availability of vault records.

## Attacker capabilities

An attacker may:
- Obtain a copy of the vault and the source code.
- Attempt to guess the master password offline.
- Supply input or a crafted vault file.
- Modify, replace or truncate a vault file that they can access.
- Attempt record operations without unlocking the vault.
- Interrupt the application during an operation.

## Threats and planned mitigations

| ID | Threat | Planned mitigation |
| --- | --- | --- |
| T1 | Disclosure of credentials from a stolen vault | Encrypt all credential fields using authenticated encryption. |
| T2 | Guessing of the master password | Use Argon2id resource costs and encourage a strong master password. |
| T3 | Access to records without authentication | Require successful vault login before every record operation. Clear unlocked state when locking or exiting. |
| T4 | Vault tampering or truncation | Validate the file structure and verify authentication during decryption. Reject invalid data without exposing decrypted content. |
| T5 | Memory corruption due to invalid input data | Limit the length of the input data. |
| T6 | Secrets exposed through output or logs | Use hidden interactive password input. Never place secrets in command line, logs or error messages. |
| T7 | Secrets remaining in process memory | Minimise secret lifetimes and copies. Wipe sensitive buffers using libsodium utilities on normal and handled error paths. |
| T8 | Vault corruption during an interrupted save | Write to a securely created temporary file and use an atomic replacement procedure. Test interruption and write-failure paths. |
| T9 | Access by another unprivileged Linux user | Restrict vault-directory and file permissions. Do not require root privileges. |
| T10 | File operations redirected through symbolic links | Use controlled storage paths and file-opening procedures that reject unsafe symbolic-link targets. |

## Trust boundaries

- Terminal input is untrusted until it is validated.
- Vault files are untrusted including their headers and lengths.
- Decrypted content must not be used before successful authentication.
- The operating system and cryptographic library are trusted dependencies.

## Security limitations

- A compromised operating system, a keylogger or an administrator who controls the running process is outside the protection zone.
- Encryption cannot prevent the storage from being deleted.
- An attacker can restore a valid version of the storage.
- Memory wiping does not guarantee the removal of all copies from the operating system’s registers or buffers.
- Weak master passwords remain vulnerable to offline brute‑force attacks.

## Planned verification

- Tests for incorrect master password entries.
- Logging of access attempts when the vault is locked.
- Vault files showing signs of hacking, leakage or exceeding the allowed size.
- Review of logs for random leaks of secrets.
- Tests for save interruptions and write failures.
- Tests of access permissions, for files and symbolic links.
