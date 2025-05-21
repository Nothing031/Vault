#pragma once

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QString>
#include "src/core/vault/Vault.hpp"

class VaultInfoButton : public QPushButton {
    Q_OBJECT
public:
    explicit VaultInfoButton(Vault* vault, QWidget* parent = nullptr);

    QSize sizeHint() const override;

    Vault* getVault();

signals:
    void signalOpenVault(Vault* vault);


private:
    Vault* vault;

    QVBoxLayout* vLayout;
    QLabel* titleLabel;
    QLabel* subtitleLabel;
};
