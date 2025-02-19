#ifndef CRYPTO_HPP
#define CRYPTO_HPP

#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <thread>
#include <ctime>
#include <QObject>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#include "vault.h"

using namespace std;
namespace fs = std::filesystem;

enum class eCryptoState{
    SUCCESS,
    FAIL,
    ABORT,
    END
};
enum class eCryptoMode{
    ENCRYPTION,
    DECRYPTION
};

class Crypto : public QObject{
    Q_OBJECT

private:
    std::thread *thread;
    bool run = false;
    bool working = false;

    void _encrypt_file(const wstring& inPath,const wstring& outPath, const unsigned char* key){
        vector<unsigned char> plainBytes = ReadFile(inPath);
        // create iv
        unsigned char iv[16];
        RAND_bytes(iv, 16);
        // encrypt
        vector<unsigned char> cipherBytes = aes256_encrypt(plainBytes, key, iv);
        // insert iv
        cipherBytes.insert(cipherBytes.begin(), iv, iv + 16);
        WriteFile(outPath, cipherBytes);
    }
    void _decrypt_file(const wstring& inPath,const wstring& outPath, const unsigned char* key){
        vector<unsigned char> cipherBytes = ReadFile(inPath);
        // read iv
        unsigned char iv[16];
        copy(cipherBytes.begin(), cipherBytes.begin() + 16, iv);
        // erase iv
        cipherBytes.erase(cipherBytes.begin(), cipherBytes.begin() + 16);
        // decrypt
        vector<unsigned char> plainBytes = aes256_decrypt(cipherBytes, key, iv);
        WriteFile(outPath, plainBytes);
    }

    void __en_decrypt_files(vector<FILE_STRUCT>& fullFiles, const unsigned char* key, eCryptoMode cMode){
        QStringList successList;
        QStringList failList;
        QStringList tempList;
        clock_t start = clock();

        for (auto& file : fullFiles){
            // check flag
            if (!run){
                // finalize work
                working = false;
                emit signal_outputMessage(successList, eCryptoState::SUCCESS, cMode);
                emit signal_outputMessage(failList, eCryptoState::FAIL, cMode);
                tempList.clear();
                emit signal_outputMessage(tempList, eCryptoState::ABORT, cMode);
                return;
            }
            //encrypt
            if (cMode == eCryptoMode::ENCRYPTION && !file.encrypted){
                if (!fs::exists(file.path)){
                    failList.push_back(file.relativePath);
                    tempList.clear();
                    tempList.push_back(QString::fromStdString("File not exists: ") + file.relativePath);
                    emit signal_outputMessage(tempList, eCryptoState::FAIL, cMode);
                }
                wstring inPath = file.path.generic_wstring();
                wstring outPath = inPath + L".enc";
                try{
                    _encrypt_file(inPath, outPath, key);
                    fs::remove(inPath);
                    file.encrypted = true;
                    file.path = fs::path(outPath);
                    successList.push_back(file.relativePath);
                }catch (exception& e){
                    if (fs::exists(outPath)) fs::remove(outPath);
                    failList.push_back(file.relativePath);
                    tempList.clear();
                    tempList.push_back(QString::fromStdString(e.what()) + ": " + file.relativePath);
                    emit signal_outputMessage(tempList, eCryptoState::FAIL, cMode);
                }
            }
            // decrypt
            else if (cMode == eCryptoMode::DECRYPTION && file.encrypted){
                if (!fs::exists(file.path)){
                    failList.push_back(file.relativePath);
                    tempList.clear();
                    tempList.push_back(QString::fromStdString("File not exists: ") + file.relativePath);
                    emit signal_outputMessage(tempList, eCryptoState::FAIL, cMode);
                }
                wstring inPath = file.path.generic_wstring();
                wstring outPath = inPath.substr(0, inPath.size() - 4);
                try{
                    _decrypt_file(inPath, outPath, key);
                    fs::remove(inPath);
                    file.encrypted = false;
                    file.path = fs::path(outPath);
                    successList.push_back(file.relativePath);
                }catch (exception& e){
                    if (fs::exists(outPath)) fs::remove(outPath);
                    failList.push_back(file.relativePath);
                    tempList.clear();
                    tempList.push_back(QString::fromStdString(e.what()) + ": " + file.relativePath);
                    emit signal_outputMessage(tempList, eCryptoState::FAIL, cMode);
                }

            }
            // emit successList every 100ms
            if (clock() - start >= 100){
                emit signal_outputMessage(successList, eCryptoState::SUCCESS, cMode);
                successList.clear();
                start = clock();
            }
        } // end file loop

        // finalize
        run = false;
        working = false;
        emit signal_outputMessage(successList, eCryptoState::SUCCESS, cMode);
        tempList.clear();
        emit signal_outputMessage(tempList, eCryptoState::END, cMode);
        emit signal_outputMessage(failList, eCryptoState::FAIL, cMode);
    }


public:
    static vector<unsigned char> ReadFile(const wstring& path){
        std::ifstream f(path, std::ios::binary);
        if (!f.good()) {
            return vector<unsigned char>();
        }
        f.seekg(0, std::ios::end);
        size_t size = f.tellg();
        f.seekg(0, std::ios::beg);
        std::vector<unsigned char> buffer(size);
        f.read(reinterpret_cast<char*>(buffer.data()), size);
        return buffer;
    }
    static void WriteFile(const wstring& path, const vector<unsigned char>& bytes){
        ofstream of(path, std::ios::binary | std::ios::trunc);
        of.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        of.close();
    }

    static string sha256(const string& str){
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<const unsigned char*>(str.c_str()), str.size(), hash);
        stringstream ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            ss << hex << setw(2) << setfill('0') << (int)hash[i];
        }
        return ss.str();
    }

    static vector<unsigned char> aes256_encrypt(const vector<unsigned char>& input, const unsigned char* key, const unsigned char* iv) {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        std::vector<unsigned char> ciphertext(input.size() + EVP_MAX_BLOCK_LENGTH);

        int len;
        int ciphertext_len;

        if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
            throw std::runtime_error("Encryption initialization failed");
        }

        if (1 != EVP_EncryptUpdate(ctx, ciphertext.data(), &len, input.data(), input.size())) {
            throw std::runtime_error("Encryption failed");
        }
        ciphertext_len = len;

        if (1 != EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len)) {
            throw std::runtime_error("Final encryption failed");
        }
        ciphertext_len += len;

        ciphertext.resize(ciphertext_len);

        EVP_CIPHER_CTX_free(ctx);
        return ciphertext;
    }
    static vector<unsigned char> aes256_decrypt(const vector<unsigned char>& input, const unsigned char* key, const unsigned char* iv) {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        std::vector<unsigned char> plaintext(input.size());

        int len;
        int plaintext_len;

        // AES 256 CBC 모드로 복호화
        if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
            throw std::runtime_error("Decryption initialization failed");
        }

        if (1 != EVP_DecryptUpdate(ctx, plaintext.data(), &len, input.data(), input.size())) {
            throw std::runtime_error("Decryption failed");
        }
        plaintext_len = len;

        if (1 != EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len)) {
            throw std::runtime_error("Final decryption failed");
        }
        plaintext_len += len;

        plaintext.resize(plaintext_len);

        EVP_CIPHER_CTX_free(ctx);
        return plaintext;
    }

    bool start_encrypt(vector<FILE_STRUCT>& fullFiles, const unsigned char* key){
        if (working){
            return false;
        }
        run = true;
        working = true;
        thread = new std::thread(&Crypto::__en_decrypt_files, this, ref(fullFiles), key, eCryptoMode::ENCRYPTION);
        return true;
    }
    bool start_decrypt(vector<FILE_STRUCT>& fullFiles, const unsigned char* key){
        if (working){
            return false;
        }
        run = true;
        working = true;
        thread = new std::thread(&Crypto::__en_decrypt_files, this, ref(fullFiles), key, eCryptoMode::DECRYPTION);
        return true;
    }
    void suspend(){
        run = false;
    }
public slots:


signals:
    void signal_outputMessage(QStringList messages, eCryptoState cState, eCryptoMode cMode);

    void signal_suspended(QStringList QStrList);
    void signal_decrypted(QStringList relativePaths);
    void signal_encrypted(QStringList relativePaths);
    void signal_exception(QString exception);
    void signal_finalExceptions(QStringList exceptions);
    void signal_work_encryption_done();
    void signal_work_decryption_done();
};


#endif // CRYPTO_HPP
