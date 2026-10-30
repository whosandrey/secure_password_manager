# Checkpoint 2 Progress Note

## Functionality Implemented

The current prototype can generate a local vault consisting of one credential.

The credential has:

- Service name
- Username
- Password

The user generates a master password while generating the vault.
The master password should be entered correctly for unlocking.

On the successful unlock process, the application shows the service name
and the username. The password is shown only as `********`.

## Implementation of cryptographic primitives

The encryption key is generated from the master password by means of Argon2id
using `crypto_pwhash` interface provided by libsodium library.

The data of the vault is encrypted/authenticated by XChaCha20-Poly1305.

Header fields in the vault include:

- Version format
- Identifier for algorithm
- Argon2id parameters
- Randomly generated salt
- Randomly generated nonce
- Length of ciphertext

The header does not include any plaintext credentials.
The header is authenticated as additional data during encryption.

## Security controls

- Entered master passwords and stored passwords are non-echoing.
- Secret buffers are zeroed out using `sodium_memzero`.
- Input sizes are bounded.
- Vault header and file sizes are checked before decryption.
- File permissions of `0600` are used when creating the vault.
- Vault files are never overwritten.
- Vault files are ignored by Git.
- Incorrect passwords and changed vault data do not authenticate.
- Plaintext stored passwords are never displayed.

### Tests executed

The following tests were executed:

- Clean compilation using Makefile
- Compilation with enabled GCC warnings
- Compilation with AddressSanitizer and UndefinedBehaviorSanitizer
- Vault creation
- Unlock with the master password
- Unlock with an incorrect master password
- Duplicate vault creation refusal
- Check permissions on the vault file
- Verify that test credentials are not included in the vault file

### Limitations of the prototype

At the moment, the prototype stores only one credential.

Future implementation will include updating, deleting and storing more than
one credential.