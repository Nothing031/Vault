#ifndef FILE_T_H
#define FILE_T_H

#include <QString>


typedef struct __file_t__{
    enum state{
        PlainData,
        CipherData
    };

    int index = -1;
    QString absolutepath;
    QString relativePath;
    QString displayPath;
    state state = PlainData;
} file_t, *pfile_t;


#endif // FILE_T_H
