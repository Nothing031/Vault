#pragma once

#include <QListWidget>
#include <QObject>


#include "src/core/vault/Vault.hpp"
#include "src/core/vault/VaultManager.hpp"
#include "src/widgets/VaultButton.hpp"


class VaultListWidget : public QListWidget
{
    Q_OBJECT
public:
    VaultListWidget(QWidget* parent = nullptr);

private:


private slots:
    void addVaultButton(Vault* button);
    void removeVaultButton(Vault* vault);


signals:
    void requestOpenVault(Vault* vault);

private:


};
