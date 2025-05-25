#pragma once

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QString>
#include <QVector>
#include "src/core/vault/Vault.hpp"

class VaultButton : public QPushButton {
    Q_OBJECT
public:
    explicit VaultButton(Vault* vault, QWidget* parent = nullptr);

    QSize sizeHint() const override;

    Vault* getVault();

public slots:
    void UpdateDirectory();

signals:
    void onSizeHintChange(QSize sizehint, VaultButton* self);
    void requestOpenVault(Vault* vault);
    void requestDetachVault(Vault* vault);


private slots:
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    Vault* m_vault;

    QLabel* titleLabel;
    QLabel* subtitleLabel;
};
