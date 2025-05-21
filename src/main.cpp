#include <QApplication>
#include <QWidget>
#include <QDebug>
#include <Windows.h>
#include "src/widgets/VaultEntryWindow.hpp"

int main(int argc, char *argv[])
{
    HANDLE hMutex = CreateMutexW(NULL, TRUE, L"Nothing031.Vault");
    if (GetLastError() == ERROR_ALREADY_EXISTS){
        qWarning() << "Error the process is already running";
        return 1;
    }

    QApplication a(argc, argv);
    VaultEntryWindow w;
    w.show();
    int result = a.exec();


    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
    return result;
}
