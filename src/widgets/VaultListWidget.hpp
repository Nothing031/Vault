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


public slots:
    void addVaultButton(std::shared_ptr<Vault> vault);
    void removeVaultButton(std::shared_ptr<Vault> vault);

signals:
    void requestOpenVault(std::shared_ptr<Vault> vault);
    void requestDetachVault(std::shared_ptr<Vault> vault);

private:


};
