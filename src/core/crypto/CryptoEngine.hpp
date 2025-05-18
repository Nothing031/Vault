#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <QStringList>

#include "src/core/vault/Vault.hpp"

class Crypto : public QObject{
public:
    enum Event{
        START,
        END,
        ABORT,
        PROGRESS_CURRENT,
        PROGRESS_MAX,
        MESSAGE_SINGLE,
        MESSAGE_LIST,
    };



signals:
    void onEvent(Event event, QVariant param);

public slots:
    void EncryptVault(Vault*);
    void DecryptVault(Vault*);

};






