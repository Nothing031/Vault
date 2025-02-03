#ifndef CRYPTO_HPP
#define CRYPTO_HPP

#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <fstream>

#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

using namespace std;
namespace fs = std::filesystem;

namespace crypto{

static string sha256(const string& str){
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(str.c_str()), str.size(), hash);
    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}

bool aes256_encrypt(const wstring& inputFile, const wstring& outputFile, const unsigned char* key) {
    ifstream in(inputFile, ios::binary);
    ofstream out(outputFile, ios::binary);

    if (!in.is_open()) {
        throw wstring(L"Failed to open file :" + inputFile);
    }
    if (!out.is_open()) {
        throw wstring(L"Failed to create file :" + outputFile);
    }

    unsigned char iv[16];
    RAND_bytes(iv, sizeof(iv));
    out.write(reinterpret_cast<char*>(iv), sizeof(iv));

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv);

    vector<unsigned char> buffer(1024);
    int len;
    in.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
    do {
        len = in.gcount();
        if (len > 0) {
            unsigned char out_buf[1024 + AES_BLOCK_SIZE];
            int out_len;
            if (1 != EVP_CipherUpdate(ctx, out_buf, &out_len, buffer.data(), len)) {
                EVP_CIPHER_CTX_free(ctx);
                throw string("Error during encryption");
            }
            out.write(reinterpret_cast<char*>(out_buf), out_len);
        }
    } while (in.read(reinterpret_cast<char*>(buffer.data()), buffer.size()));

    unsigned char final_buf[AES_BLOCK_SIZE];
    int final_len;
    if (1 != EVP_CipherFinal_ex(ctx, final_buf, &final_len)) {
        EVP_CIPHER_CTX_free(ctx);
        throw string("Error finalizing encryption");
    }
    EVP_CIPHER_CTX_free(ctx);
    out.write(reinterpret_cast<char*>(final_buf), final_len);
    in.close();
    out.close();
    return true;
}

bool aes256_decrypt(const wstring& inputFile, const wstring& outputFile, const unsigned char* key) {
    ifstream in(inputFile, ios::binary);
    ofstream out(outputFile, ios::binary);
    if (!in.is_open()) {
        throw wstring(L"Failed to open file :" + inputFile);
    }
    if (!out.is_open()) {
        throw wstring(L"Failed to create file :" + outputFile);
    }

    unsigned char iv[16];
    in.read(reinterpret_cast<char*>(iv), sizeof(iv));

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv);

    vector<unsigned char> buffer(1024);
    int len;
    in.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
    do {
        len = in.gcount();
        if (len > 0) {
            unsigned char out_buf[1024 + AES_BLOCK_SIZE];
            int out_len;
            if (1 != EVP_CipherUpdate(ctx, out_buf, &out_len, buffer.data(), len)) {
                EVP_CIPHER_CTX_free(ctx);
                throw string("Error during decryption");
            }
            out.write(reinterpret_cast<char*>(out_buf), out_len);
        }
    } while (in.read(reinterpret_cast<char*>(buffer.data()), buffer.size()));

    unsigned char final_buf[AES_BLOCK_SIZE];
    int final_len;
    if (1 != EVP_CipherFinal_ex(ctx, final_buf, &final_len)) {
        EVP_CIPHER_CTX_free(ctx);
        throw string("Error finalizing decryption");
    }
    EVP_CIPHER_CTX_free(ctx);
    out.write(reinterpret_cast<char*>(final_buf), final_len);
    in.close();
    out.close();
    return true;
}

}


#endif // CRYPTO_HPP
