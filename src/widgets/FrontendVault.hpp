#pragma once


#include <QWidget>

#include "src/filelistmodel.hpp"
#include "src/vault.hpp"
#include "src/core/Crypto.hpp"

namespace Ui {
class FrontendVault;
}

class FrontendVault : public QWidget
{
    Q_OBJECT
public:
    explicit FrontendVault(QWidget *parent = nullptr);

    ~FrontendVault();

    void init(Vault* pvault);

private slots:

    void CheckPassword();

    void StartEncrypt();

    void StartDecrypt();

    void Suspend();

signals:
    void request_detachVault(Vault* pvault);

    void request_setEnable_ui(bool b);

    void request_terminal_message(const QStringList& messages);

private:
    FileListModel *model;
    Ui::FrontendVault *ui;

    Vault* pVault;

    Crypto crypto;
    QThread *thread;
};


