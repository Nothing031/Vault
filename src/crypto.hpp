#ifndef CRYPTO_HPP
#define CRYPTO_HPP

#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <thread>

#include <QObject>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#include "vault.h"

using namespace std;
namespace fs = std::filesystem;

class Crypto : public QObject{
    Q_OBJECT

private:
    std::thread *thread;
    bool run = false;
    bool working = false;

    void _encrypt_files(vector<FILE_STRUCT>& fullFiles, const unsigned char* key){
        QStringList exceptionList;
        for (auto& file : fullFiles){
            // break
            if (!run){
                working = false;
                QMetaObject::invokeMethod(this, [this](){
                    emit signal_decrypted("encryption suspended");
                }, Qt::QueuedConnection);
                return;
            }
            // encrypt
            if (!file.encrypted){
                wstring inPath = file.path.generic_wstring();
                wstring outPath = inPath + L".enc";
                try{
                    vector<unsigned char> plainBytes = ReadFile(inPath);
                    // generate random iv
                    unsigned char iv[16];
                    RAND_bytes(iv, sizeof(iv));
                    // encrypt
                    vector<unsigned char> cipherBytes = aes256_encrypt(plainBytes, key, iv);
                    // write iv
                    cipherBytes.insert(cipherBytes.begin(), iv, iv + 16);
                    WriteFile(outPath, cipherBytes);
                    // remove plain
                    fs::remove(inPath);
                    // set file
                    file.encrypted = true;
                    file.path = fs::path(outPath);
                    // emit signal
                    QString qOutPath = QString::fromStdWString(outPath);
                    QMetaObject::invokeMethod(this, [this, qOutPath](){
                        emit signal_encrypted(qOutPath);
                    }, Qt::QueuedConnection);
                }catch (wstring& e){
                    exceptionList.push_back(QString::fromStdWString(e));
                    if (fs::exists(outPath)) fs::remove(outPath);
                    QMetaObject::invokeMethod(this, [this, e]() {
                        emit signal_exception(QString::fromStdWString(e));
                    }, Qt::QueuedConnection);
                }
            }
        }
        run = false;
        working = false;
        // emit signal
        QMetaObject::invokeMethod(this, [this]() {
            emit signal_work_encryption_done();
        }, Qt::QueuedConnection);
        QMetaObject::invokeMethod(this, [this, exceptionList](){
            emit signal_finalExceptions(exceptionList);
        }, Qt::QueuedConnection);
    }
    void _decrypt_files(vector<FILE_STRUCT>& fullFiles, const unsigned char* key){
        QStringList exceptionList;
        for (auto& file : fullFiles){
            // break
            if (!run){
                working = false;
                QMetaObject::invokeMethod(this, [this](){
                    emit signal_decrypted("decryption suspended");
                }, Qt::QueuedConnection);
                return;
            }
            // decrypt
            if (file.encrypted){
                wstring inPath = file.path.generic_wstring();
                wstring outPath = inPath.substr(0, inPath.size() - 4);
                try{
                    vector<unsigned char> cipherBytes = ReadFile(inPath);
                    // read iv
                    unsigned char iv[16];
                    copy(cipherBytes.begin(), cipherBytes.begin() + 16, iv);
                    // remove iv
                    cipherBytes.erase(cipherBytes.begin(), cipherBytes.begin() + 16);
                    // decrypt
                    vector<unsigned char> plainBytes = aes256_decrypt(cipherBytes, key, iv);
                    WriteFile(outPath, plainBytes);
                    // remove cipher
                    fs::remove(inPath);
                    // set file
                    file.encrypted = false;
                    file.path = fs::path(outPath);
                    // emit signal
                    QString qOutPath = QString::fromStdWString(outPath);
                    QMetaObject::invokeMethod(this, [this, qOutPath](){
                        emit signal_decrypted(qOutPath);
                    }, Qt::QueuedConnection);
                }catch (wstring& e){
                    if (fs::exists(outPath)) fs::remove(outPath);
                    exceptionList.push_back(QString::fromStdWString(e));
                    QMetaObject::invokeMethod(this, [this, e]() {
                        emit signal_exception(QString::fromStdWString(e));
                    }, Qt::QueuedConnection);
                }
            }
        }
        run = false;
        working = false;
        // emit signal
        QMetaObject::invokeMethod(this, [this]() {
            emit signal_work_decryption_done();
        }, Qt::QueuedConnection);
        QMetaObject::invokeMethod(this, [this, exceptionList]{
            emit signal_finalExceptions(exceptionList);
        }, Qt::QueuedConnection);
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
        ofstream of(path, std::ios::binary);
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

        ciphertext.resize(ciphertext_len);  // 암호화된 데이터 크기에 맞게 크기 조정

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

        plaintext.resize(plaintext_len);  // 복호화된 데이터 크기에 맞게 크기 조정

        EVP_CIPHER_CTX_free(ctx);
        return plaintext;
    }

    bool start_encrypt(vector<FILE_STRUCT>& fullFiles, const unsigned char* key){
        if (working){
            return false;
        }
        run = true;
        working = true;
        thread = new std::thread(&Crypto::_encrypt_files, this, ref(fullFiles), key);
        return true;
    }
    bool start_decrypt(vector<FILE_STRUCT>& fullFiles, const unsigned char* key){
        if (working){
            return false;
        }
        run = true;
        working = true;
        thread = new std::thread(&Crypto::_decrypt_files, this, ref(fullFiles), key);
        return true;
    }
    void suspend(){
        run = false;
    }
public slots:


signals:
    void signal_suspended(QString text);
    void signal_decrypted(QString relativePath);
    void signal_encrypted(QString relativePath);
    void signal_exception(QString exception);
    void signal_finalExceptions(QStringList exceptions);
    void signal_work_encryption_done();
    void signal_work_decryption_done();
};


#endif // CRYPTO_HPP
