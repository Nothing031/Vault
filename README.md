
### Encryption
Create 64 bytes length key with PBKDF2 HMAC SHA256. Using password and randomly generated 256 bits length salt and 100000 iterations for key.
First 32 bytes of key is for verification and last 32 bytes for AES-256 Encryption/Decryption.
#### Cipher data structor
\<Signature (20 bytes)\>\<Version (16 bytes)\>\<Salt (32 bytes)\>\<Iteration (4 bytes)\>\<Hmac (32 bytes)\>\<Iv (16 bytes)\>\<Cipher data\>
