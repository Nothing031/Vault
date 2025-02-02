#include "mainwindow.h"

#include <QApplication>
#include <Windows.h>
#include <dwmapi.h>

#include <QDebug>
#include <filesystem>
#include <fstream>
#include <Windows.h>
#include <iostream>
int main(int argc, char *argv[])
{

    // load json





    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
