#include "mainwindow.h"

#include <QApplication>
#include <Windows.h>
#include <dwmapi.h>
#include <QMessageBox>

#include <QDebug>

#include <openssl/aes.h>
#include <openssl/sha.h>


int main(int argc, char *argv[])
{

    // load json
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
