# Threat Model

## Status

Initial design for Checkpoint 1.
The controls below are planned, not yet implemented or verified.

## Scope

A local, single-user password manager written in C for Linux.
The user unlocks an encrypted vault using a master password
and manages credentials through a command-line interface.

The source code is public. Security must not depend on
keeping the implementation secret.

The mechanism for retrieving passwords without displaying
plaintext remains subject to clarification with the lecturer.

## Assets

- Stored passwords, usernames and service names.
- The master password and derived encryption keys.
- The integrity and availability of vault records.

## Attacker capabilities

An attacker may:

- Obtain a copy of the locked vault and the source code.
- Attempt to guess the master password offline.
- Supply malformed input or a crafted vault file.
- Modify, replace or truncate a vault file they can access.
- Attempt record operations without unlocking the vault.
- Interrupt the application during an operation.

## Threats and planned mitigations

| ID | Threat | Planned mitigation |
| --- | --- | --- |
| T1 | Disclosure of credentials from a stolen vault | Encrypt all credential fields using authenticated encryption. Derive the encryption key from the master password using Argon2id with a random salt. |
| T2 | Offline guessing of the master password | Use appropriate Argon2id resource costs and encourage a strong master password. Weak passwords remain vulnerable to guessing. |
| T3 | Access to records without authentication | Require successful vault unlocking before every record operation. Clear unlocked state when locking or exiting. |
| T4 | Vault tampering or truncation | Validate the file structure and verify authentication during decryption. Reject invalid data without exposing decrypted content. |
| T5 | Memory corruption from malformed input | Bound input lengths and file sizes, validate parsed lengths and integer arithmetic, and check allocation and function results. |
| T6 | Secrets exposed through output or logs | Use hidden interactive password input. Never place secrets in command-line arguments, logs or error messages. |
| T7 | Secrets remaining in process memory | Minimise secret lifetimes and copies. Explicitly wipe sensitive buffers using libsodium utilities on normal and handled error paths. |
| T8 | Vault corruption during an interrupted save | Write to a securely created temporary file and use an atomic replacement procedure. Test interruption and write-failure paths. |
| T9 | Access by another unprivileged Linux user | Restrict vault-directory and file permissions. Do not require root privileges. |
| T10 | File operations redirected through symbolic links | Use controlled storage paths and file-opening procedures that reject unsafe symbolic-link targets. |

## Trust boundaries

- Terminal input is untrusted until validated.
- Vault files are untrusted, including their headers and lengths.
- Decrypted content must not be used before successful authentication.
- The operating system and cryptographic library are trusted dependencies.

## Security limitations

- A compromised operating system, keylogger or administrator
  controlling the running process is outside the protection scope.
- Encryption cannot prevent deletion of the vault.
- An attacker may restore an older valid vault. Authenticated
  encryption alone does not detect this rollback.
- Explicit memory wiping does not guarantee removal of every
  copy from registers, operating-system buffers or crash artifacts.
- Weak master passwords remain susceptible to offline guessing.
- This educational application must use fictional test credentials.

## Planned verification

- Correct and incorrect master-password tests.
- Record-operation attempts while the vault is locked.
- Tampered, truncated and oversized vault files.
- Empty, oversized and malformed user input.
- Inspection of logs for accidental secret disclosure.
- Interrupted-save and write-failure tests.
- File-permission and symbolic-link tests.
- Compiler warnings, static analysis, sanitizers and memory checks.

Test results and remaining limitations will be documented
as implementation progresses.
