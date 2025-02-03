#ifndef CRYPTO_HPP
#define CRYPTO_HPP

#include <string>
#include <sstream>
#include <iomanip>
#include <filesystem>

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



}


#endif // CRYPTO_HPP
