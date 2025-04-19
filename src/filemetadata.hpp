#ifndef FILEMETADATA_H
#define FILEMETADATA_H

#include <QString>
#include <QByteArray>


struct __FileMetadata__{
    enum State{
        PlainData,
        CipherData
    };
    struct Header{
        const char* signature = "VAULT BY Nothing031";
        QByteArray salt;
        int iteration;
        QByteArray hmac;
        QByteArray iv;

        struct sizes{
            static constexpr int signature = 20;
            static constexpr int salt = 32;
            static constexpr int iteration = 4;
            static constexpr int hmac = 32;
            static constexpr int iv = 16;
            static constexpr int total = signature + salt + iteration + hmac + iv;
        };
        struct offsets{
            static constexpr int signature = 0;
            static constexpr int salt = 0x14;
            static constexpr int iteration = 0x34;
            static constexpr int hmac = 0x38;
            static constexpr int iv = 0x58;
        };
        bool Set(const QByteArray& data){
            if (data.size() < signature + salt + iteration + hmac + iv)
                return false;

            if (memcmp(signature, data.mid(offsets::signature, sizes::signature).constData(), sizes::signature))
                return false;

            salt = data.mid(offsets::salt, sizes::salt);
            memcpy(&iteration, data.data() + offsets::iteration, sizes::iteration);
            hmac = data.mid(offsets::hmac, sizes::hmac);
            iv = data.mid(offsets::iv, sizes::iv);
            return true;
        };
        QByteArray Get(){
            QByteArray header(sizes::total, 0);
            memcpy(header.data() + offsets::signature, signature, sizes::signature);
            memcpy(header.data() + offsets::salt, salt, sizes::salt);
            memcpy(header.data() + offsets::iteration, &iteration, sizes::iteration);
            memcpy(header.data() + offsets::hmac, hmac, sizes::hmac);
            memcpy(header.data() + offsets::iv, iv, sizes::iv);
            return header;
        }
    };
    struct Info{
        QString absolutepath;
        QString relativePath;
        QString displayPath;
        State state = PlainData;
    };

    Header header;
    Info info;
};
typedef struct __FileMetadata__ FileMetadata;


#endif // FILEMETADATA_H
