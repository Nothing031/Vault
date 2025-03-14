#ifndef FILE_T_H
#define FILE_T_H

#include <QString>

enum file_t_state{
    PlainData,
    CipherData,
};

typedef struct __file_t__{
    int index = -1;
    QString absolutepath;
    QString relativePath;
    QString displayPath;
    file_t_state state = PlainData;
} file_t;

#endif // FILE_T_H
