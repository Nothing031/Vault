# Vault
File based encryption program
### Encryption
Generate a 64 bytes key with PBKDF2 HMAC SHA256. Use password and randomly generated 32 bytes salt with 100000 iterations for key.
The fist 32 bytes of the key are for verification and the last 32 bytes are used for AES-256 Encryption/Decryption.
### Cipher data structor
| Field       | offset |  Size (bytes) | Description            |
|-------------|--------|-------|------------------------|
| Signature   | 0x00   | 20    | Format signature |
| Version     | 0x16   | 4     | Format version info    |
| Salt        | 0x1A   | 32    | Random salt used for PBKDF2 |
| Iteration   | 0x3A   | 4     | PBKDF2 iteration count |
| HMAC        | 0x3E   | 32    | integrity check vaule |
| IV          | 0x5E   | 16    | Initialize vector |
| Cipher Data | 0x6E   | -     | Encrypted data      |
