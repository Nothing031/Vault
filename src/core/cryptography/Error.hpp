#pragma once

#include <QString>

class Error
{
public:
    enum ErrorCode{
        NO_ERROR,
        ERROR_UNKNOWN,
        CRYPTO_EVP_INIT_FAILURE,
        CRYPTO_EVP_UPDATE_FAILURE,
        CRYPTO_EVP_FINALIZE_FAILURE,
        HEADER_HMAC_FAILURE,
        HEADER_SIGNATURE_FAILURE, // file has been corrupted
        IO_WRITE_FAILURE,
        IO_READ_FAILURE,
        IO_NOT_EXISTS,
    };

    Error();
    Error(const ErrorCode, const QString, const QString);
    void Set(const ErrorCode, const QString, const QString);

    const ErrorCode& code();
    const QString& what();
    const QString& path();

private:
    ErrorCode m_code = NO_ERROR;
    QString m_what;
    QString m_path;
};

