#ifndef WINDOW_CRYPTO_H
#define WINDOW_CRYPTO_H

#include <QWidget>

#include "src/filelistmodel.hpp"
#include "src/vault.hpp"
#include "src/crypto.hpp"

namespace Ui {
class window_crypto;
}

class window_crypto : public QWidget
{
    Q_OBJECT
public:
    explicit window_crypto(QWidget *parent = nullptr);

    ~window_crypto();

    void on_request_page(Vault* pvault);

private slots:

    void on_password_input_lineedit_returnPressed();

    void on_password_visibility_button_toggled(bool checked);

    void on_openFolder_button_clicked();

    void on_encrypt_button_clicked();

    void on_decrypt_button_clicked();

    void on_suspend_button_clicked();

    void on_vault_detach_button_clicked();

    void on_refresh_button_clicked();

signals:
    void request_detachVault(Vault* pvault);

    void request_setEnable_ui(bool b);

    void request_terminal_message(const QStringList& messages);

private:
    FileListModel *model;
    Ui::window_crypto *ui;

    Vault* pVault;

    Crypto crypto;
    QThread *thread;
};

#endif // WINDOW_CRYPTO_H
