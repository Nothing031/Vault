#pragma once

#include <QObject>
#include "Vault.hpp"
#include <memory>


class VaultLoader : public QObject{
    Q_OBJECT
public:
    enum Event{
        CLEAN,
        FAILED
    };

    static VaultLoader& GetInstance();

    void LoadVault(std::shared_ptr<Vault>);
    void SaveVault(std::shared_ptr<Vault>);
    Event GetLastError();

signals:
    void onEvent(Event);

private:
    VaultLoader();
    static VaultLoader instance;

    Event lastError = CLEAN;
};
