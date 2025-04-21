#ifndef FILEMETADATA_H
#define FILEMETADATA_H

#include <QString>
#include <QByteArray>
#include <QDebug>

struct FileMetadata{
    enum State{
        PlainData,
        CipherData
    };
    struct Header{
        struct sizes{
            static constexpr int signature = 0x16;
            static constexpr int version = 0x10;
            static constexpr int salt = 0x20;
            static constexpr int iteration = 0x04;
            static constexpr int hmac = 0x20;
            static constexpr int iv = 0x10;
            static constexpr int total = signature + version + salt + iteration + hmac + iv;
        };
        struct offsets{
            static constexpr int signature = 0;
            static constexpr int version = signature + sizes::signature;
            static constexpr int salt = version + sizes::version;
            static constexpr int iteration = salt + sizes::salt;
            static constexpr int hmac = iteration + sizes::iteration;
            static constexpr int iv = hmac + sizes::hmac;
        };

        static const inline QByteArray signature = QByteArray("VAULT BY NOTHING031", sizes::signature);
        QByteArray version = "V0.3.0";
        QByteArray salt = {};
        int iteration = 0;
        QByteArray hmac = {};
        QByteArray iv = {};

        bool Set(const QByteArray& data){
            if (data.size() < sizes::total){
                qDebug() << "size does not matching";
                return false;
            }
            if (memcmp(signature.constData(), data.mid(offsets::signature, sizes::signature).constData(), sizes::signature)){
                qDebug() << "signature does not matching";
                return false;
            }

            version = data.mid(offsets::version, sizes::version);
            salt = data.mid(offsets::salt, sizes::salt);
            memcpy(&iteration, data.constData() + offsets::iteration, sizes::iteration);
            hmac = data.mid(offsets::hmac, sizes::hmac);
            iv = data.mid(offsets::iv, sizes::iv);
#ifdef QT_DEBUG
            qDebug() << "signature :" << signature;
            qDebug() << "version   :" << version;
            qDebug() << "salt      :" << salt;
            qDebug() << "iteration :" << iteration;
            qDebug() << "hmac      :" << hmac;
            qDebug() << "iv        :" << iv;
#endif
            return true;
        };

        QByteArray Get() const {
            QByteArray header(sizes::total, 0);
            memcpy(header.data() + offsets::signature, signature, sizes::signature);
            memcpy(header.data() + offsets::version, version.constData(), sizes::version);
            memcpy(header.data() + offsets::salt, salt.constData(), sizes::salt);
            memcpy(header.data() + offsets::iteration, &iteration, sizes::iteration);
            memcpy(header.data() + offsets::hmac, hmac.constData(), sizes::hmac);
            memcpy(header.data() + offsets::iv, iv.constData(), sizes::iv);
            return header;
        }
    };
    struct Path{
        QString absolutepath;
        QString relativePath;
        QString displayPath;
        State state = PlainData;
    };

    Header header;
    Path path;
};


#endif // FILEMETADATA_H
