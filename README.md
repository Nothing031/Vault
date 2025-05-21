# Vault
File based AES-256 encryption gui program

## Supported OS
| OS       |  support  |
|----------|-----------|
| Windows  | ✅       |
| Linux    | ❌       |
| Mac      | ❌       |

## Beware
* does not support .enc extension
* does not support over MAX_PATH(260 char) length path
* does not support encrypted file via v0.2.1 and lower versions. decrypt before reinstall Vault

## Encryption
Generate a 64 bytes key with PBKDF2 HMAC SHA256. Use password and randomly generated 32 bytes salt with 100000 iterations for key.
The fist 32 bytes of the key are for verification and the last 32 bytes are used for AES-256 Encryption/Decryption.

## Encrypted File Structor
| Field          | offset |  Size (bytes) | Description            |
|----------------|--------|---------------|------------------------|
| Signature      | 0x00   | 20            | Format signature       |
| Version        | 0x16   | 4             | Format version info    |
| Salt           | 0x1A   | 32            | Global salt for each vault |
| Iteration      | 0x3A   | 4             | PBKDF2 iteration count |
| HMAC           | 0x3E   | 32            | HMAC check vaule |
| IV             | 0x5E   | 16            | Initialization  vector |
| Encrypted Data | 0x6E   | -             | Encrypted data      |
