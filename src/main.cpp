#include <QApplication>
#include <QWidget>

#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Vault");
    w.show();
    return a.exec();
}
