#pragma once

#include <QString>

struct FileInfo{
    enum State{
        UNKNOWN_NONE,
        UNKNOWN_SIGNATURENOTMATCH,
        PLAIN_GOOD,
        CIPHER_GOOD,
        CIPHER_HEADERNOTMATCH
    };
    struct FilePath{
        QString absolutepath;
        QString displayPath;
    };

    State state = UNKNOWN_NONE;
    FilePath path;
};
