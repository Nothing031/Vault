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
    explicit VaultButton(std::shared_ptr<Vault> vault, QWidget* parent = nullptr);

    QSize sizeHint() const override;

    std::shared_ptr<Vault> getVault();

public slots:
    void UpdateDirectory();

signals:
    void onSizeHintChange(QSize sizehint, VaultButton* self);
    void requestOpenVault(std::shared_ptr<Vault>);
    void requestDetachVault(std::shared_ptr<Vault>);

private slots:
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    std::shared_ptr<Vault> vault;

    QLabel* titleLabel;
    QLabel* subtitleLabel;
};
