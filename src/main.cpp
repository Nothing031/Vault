#include <QApplication>
#include <QWidget>

#include "src/widgets/FrontendMainWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FrontendMainWindow w;
    w.setWindowTitle("Vault");
    w.show();
    return a.exec();
}
