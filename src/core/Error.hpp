#pragma once

#include <QString>

class Error{
public:
    enum eError{
        Unknown,
        FileNotExists,
        HeaderSignatureNotMatch,
        HeaderHmacNotMatch,
        FailedToRead,
        FailedToSave,
        Encryption,
        Decryption,
    };

    Error(const eError type = eError::Unknown, const QString what = "Unknown Error") : e_type(type), e_what(what)
    {
    }

    void Set(const eError type = eError::Unknown, const QString what = "Unknown Error")
    {
        e_type = type;
        e_what = what;
    }

    eError &type() { return e_type; }

    QString& what() { return e_what; }
private:
    eError e_type;
    QString e_what;
};

