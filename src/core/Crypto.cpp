#include "Crypto.hpp"


#include <QAtomicInt>
#include <QMutexLocker>

Crypto::Crypto(){

}

Crypto::~Crypto(){
    if (thread){
        delete thread;
    }
}



bool Crypto::ReadFile(const QString &path, QByteArray &out)
{
    QFile f(path);
    if (f.open(QFile::ReadOnly)){
        out = f.readAll();
        f.close();
        return true;
    }else{
        return false;
    }
}

bool Crypto::WriteFile(const QString &path, const QByteArray &bytes)
{
    QFile f(path);
    if (f.open(QFile::WriteOnly)){
        f.write(bytes);
        f.close();
        return true;
    }else{
        return false;
    }
}

bool Crypto::__AES256_Encrypt__(const QByteArray &in, const QByteArray &key, const QByteArray &iv, QByteArray &out, Error* error)
{
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    // init
    if (1 != EVP_EncryptInit_ex(ctx,
                                EVP_aes_256_cbc(),
                                NULL,
                                reinterpret_cast<const unsigned char*>(key.constData()),
                                reinterpret_cast<const unsigned char*>(iv.constData()))){
        EVP_CIPHER_CTX_free(ctx);
        if (error) error->Set(Error::Encryption, "Failed to initialize EVP");
        return false;
    }

    // encrypt
    out.resize(in.size() + AES_BLOCK_SIZE);
    int outLen;
    if (1 != EVP_EncryptUpdate(ctx,
                               reinterpret_cast<unsigned char*>(out.data()),
                               &outLen,
                               reinterpret_cast<const unsigned char*>(in.data()),
                               in.size())) {
        EVP_CIPHER_CTX_free(ctx);
        if (error) error->Set(Error::Encryption, "Failed to encrypt");
        return false;
    }
    int finalLen = outLen;

    // finalize
    if (1 != EVP_EncryptFinal_ex(ctx,
                                 reinterpret_cast<unsigned char*>(out.data()) + outLen,
                                 &outLen)) {
        EVP_CIPHER_CTX_free(ctx);
        if (error) error->Set(Error::Encryption, "Failed to finalize EVP");
        return false;
    }
    finalLen += outLen;
    out.resize(finalLen);
    EVP_CIPHER_CTX_free(ctx);
    return true;
}

bool Crypto::__AES256_Decrypt__(const QByteArray &in, const QByteArray &key, const QByteArray &iv, QByteArray &out, Error* error)
{
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    // init
    if (1 != EVP_DecryptInit_ex(ctx,
                                EVP_aes_256_cbc(),
                                NULL,
                                reinterpret_cast<const unsigned char*>(key.constData()),
                                reinterpret_cast<const unsigned char*>(iv.constData()))) {
        EVP_CIPHER_CTX_free(ctx);
        if (error) error->Set(Error::Decryption, "Failed to initialize EVP");
        return false;
    }
    out.resize(in.size());

    // decrypt
    int outLen;
    if (1 != EVP_DecryptUpdate(ctx,
                               reinterpret_cast<unsigned char*>(out.data()),
                               &outLen,
                               reinterpret_cast<const unsigned char*>(in.constData()),
                               in.size())) {
        EVP_CIPHER_CTX_free(ctx);
        if (error) error->Set(Error::Decryption, "Failed to decrypt");
        return false;
    }

    // finalize
    int finalLen = outLen;
    if (1 != EVP_DecryptFinal_ex(ctx,
                                 reinterpret_cast<unsigned char*>(out.data()) + outLen,
                                 &outLen)) {
        EVP_CIPHER_CTX_free(ctx);
        if (error) error->Set(Error::Decryption, "Failed to finalize EVP");
        return false;
    }
    finalLen += outLen;
    out.resize(finalLen);
    EVP_CIPHER_CTX_free(ctx);
    return true;
}

bool Crypto::Aes256EncryptFile(FileInfo *file, const QByteArray &key, const QByteArray& hmac, Error* error){
    // read file
    QByteArray plainData;
    bool result;
    result = ReadFile(file->path.absolutepath, plainData);
    if (!result){
        if (error) error->Set(Error::FailedToRead, "Failed to read : " + file->path.relativePath);
        return false;
    }


    // set header
    file->header.Set(vaultHeader);
    file->header.ResetIv();

    // encrypt
    QByteArray cipherData;
    result = __AES256_Encrypt__(plainData, key, file->header.iv, cipherData, error);
    if (!result){
        if (error) error->Set(error->type(), error->what() + " : " + file->path.relativePath);
        return false;
    }


    // insert header
    cipherData.insert(0, file->header.Get());

    // save
    result = WriteFile(file->path.absolutepath + ".enc", cipherData);
    if(!result){
        if (error) error->Set(Error::FailedToSave, "Failed to save : " + file->path.relativePath);
        return false;
    }

    // delete
    QFile::remove(file->path.absolutepath);

    // set data
    file->path.absolutepath+= ".enc";
    file->path.relativePath+= ".enc";
    file->state = FileInfo::CipherData;
    return true;
}

bool Crypto::Aes256DecryptFile(FileInfo *file, const QByteArray &key, const QByteArray& hmac, Error* error){
    // read file
    QByteArray cipherData;
    bool result;
    result = ReadFile(file->path.absolutepath, cipherData);
    if (!result){
        if (error) error->Set(Error::FailedToRead, "Failed to read : " + file->path.absolutepath);
        return false;
    }

    // read header
    result = file->header.Set(cipherData);
    if (!result){
        if (error) error->Set(Error::HeaderSignatureNotMatch, "Header signature does not match : " + file->path.absolutepath);
        return false;
    }

    // hmac check
    if (file->header.hmac != hmac){
        if (error) error->Set(Error::HeaderHmacNotMatch, "Header hmac does not match : " + file->path.absolutepath);
        return false;
    }
    cipherData.erase(cipherData.begin(), cipherData.begin() + FileInfo::Header::sizes::total);

    // decrypt
    QByteArray plainData;
    result = __AES256_Decrypt__(cipherData, key, file->header.iv, plainData, error);
    if (!result) return false;

    // save
    result = WriteFile(file->path.absolutepath.left(file->path.absolutepath.size() - 4), plainData);

    if (!result){
        if (error) error->Set(Error::FailedToSave, "Failed to save : " + file->path.absolutepath);
        return false;
    }

    // delete
    QFile::remove(file->path.absolutepath);

    // set data
    file->path.absolutepath = file->path.absolutepath.left(file->path.absolutepath.size() - 4);
    file->path.relativePath = file->path.relativePath.left(file->path.relativePath.size() - 4);
    file->state = FileInfo::PlainData;
    return true;
}

void Crypto::AES256_Encrypt_All(Vault *vault){
    QMutexLocker<QMutex> lock(&vault->mutex);

    // get target files
    QQueue<FileInfo*> queue;
    QMutex queueMutex;
    for (auto& file : vault->files){
        if (file->state == FileInfo::PlainData)
            queue.append(file);
    }

    emit onEvent(EVENT_START, ENCRYPTION, queue.size());

    QAtomicInt queueSize = queue.size();
    QAtomicInt success = 0;
    QAtomicInt failed = 0;


    QStringList flushMessages;

    // vault header for
    FileInfo::Header vaultHeader;
    vaultHeader = {ENV_VERSION, vault->globalSalt, vault->iteration, vault->hmac, {}};
    // thread
    Enviroment& env = Enviroment::GetInstance();
    QVector<QThread*> threads;
    for (int i = 0; i < env.ThreadCount(); i++){
        QThread* thread = QThread::create([&](){
            Error error;
            bool result;
            while(true){
                // dequeue file
                QMutexLocker<QMutex> lock(&queueMutex);
                if (queue.isEmpty()) return;
                FileInfo* file = queue.dequeue();
                lock.unlock();


                result = Aes256EncryptFile(file, vault->aesKey, vault->hmac, &error);

                if (result)
                    ++success;
                else{




                }






            }
        });
    }












}

void Crypto::AES256_Decrypt_All(Vault *vault){


}















