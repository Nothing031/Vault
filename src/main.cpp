#include <QApplication>
#include <QWidget>
#include <QDebug>
#include <QMetaType>

#include <Windows.h>
#include "src/VaultApp.hpp"

#include <memory>
#include <src/core/vault/Vault.hpp>

#ifdef QT_DEBUG // memory leak check
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
// #define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

int main(int argc, char *argv[])
{
#ifdef QT_DEBUG // memory leak check
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

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
