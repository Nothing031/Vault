#include <QApplication>
#include <QMetaType>

#include <Windows.h>
#include <memory>

#include "src/core/vault/Vault.hpp"
#include "src/VaultApp.hpp"

// Memory leak check
#ifdef QT_DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

int main(int argc, char *argv[])
{
// memory leak check
#ifdef QT_DEBUG
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
