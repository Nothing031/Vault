#ifndef FRONTENDVAULT_H
#define FRONTENDVAULT_H

#include <QWidget>

#include "src/filelistmodel.hpp"
#include "src/vault.hpp"
#include "src/crypto.hpp"

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

    void on_PasswordLineedit_returnPressed();

    void on_PasswordVisibilityButton_toggled(bool checked);

    void on_OpenFolderButton_clicked();

    void on_EncryptButton_clicked();

    void on_DecryptButton_clicked();

    void on_SuspendButton_clicked();

    void on_DetachVaultButton_clicked();

    void on_RefreshButton_clicked();

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

#endif // FRONTENDVAULT_H
