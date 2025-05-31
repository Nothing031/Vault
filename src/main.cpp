#include <QApplication>
#include <QWidget>
#include <QDebug>
#include <QMetaType>

#include <Windows.h>
#include "src/VaultApp.hpp"

#include <memory>
#include <src/core/vault/Vault.hpp>

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

int main(int argc, char *argv[])
{    
    HANDLE hMutex = CreateMutexW(NULL, TRUE, L"Nothing031.Vault");
    if (GetLastError() == ERROR_ALREADY_EXISTS){
        qWarning() << "Error the process is already running";
        return 1;
    }
    qRegisterMetaType<std::shared_ptr<Vault>>("std::shared_ptr<Vault>");
    qRegisterMetaType<std::shared_ptr<FileInfo>>("std::shared_ptr<FileInfo>");

    QApplication a(argc, argv);
    VaultApp w;
    w.show();
    int result = a.exec();

    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
    return result;
}
