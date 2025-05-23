#pragma once

#include <QObject>
#include "Vault.hpp"


class VaultLoader : public QObject{
    Q_OBJECT
public:
    enum Event{
        None,
        LoadSucceeded,
        LoadFailed,
        SaveSucceeded,
        SaveFailed
    };


    static VaultLoader& GetInstance();

    void LoadVault(Vault*);
    void SaveVault(Vault*);
    Event GetLastError();

signals:
    void onEvent(Event);

private:
    VaultLoader();
    static VaultLoader instance;

    Event lastError = None;
};
