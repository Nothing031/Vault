#include "../ui/mainwindow.h"

#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <openssl/aes.h>
#include <openssl/sha.h>

#include "enviroment.hpp"

int main(int argc, char *argv[])
{
    // load json
    Enviroment::GetInstance();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
