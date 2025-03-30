#include <QApplication>
#include <QWidget>

#include "ui/FrontendMainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FrontendMainWindow w;
    w.setWindowTitle("Vault");
    w.show();
    return a.exec();
}
