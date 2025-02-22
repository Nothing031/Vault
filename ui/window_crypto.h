#ifndef WINDOW_CRYPTO_H
#define WINDOW_CRYPTO_H

#include <QWidget>

#include "src/vault.h"
#include "src/crypto.hpp"

namespace Ui {
class window_crypto;
}

class Window_crypto : public QWidget
{
    Q_OBJECT

public:
    explicit Window_crypto(QWidget *parent = nullptr, Vault vault = Vault());
    ~Window_crypto();

    void Load(const Vault& vault);

private slots:
    void on_password_input_lineedit_returnPressed();

    void on_password_visibility_button_toggled(bool checked);

    void on_openFolder_button_clicked();

    void on_encrypt_button_clicked();

    void on_decrypt_button_clicked();

    void on_suspend_button_clicked();

    void on_backup_button_clicked();

signals:
    void request_disable_ui(bool b);


private:
    Ui::window_crypto *ui;
    Crypto *crypto;
    QThread *thread;
    Vault vault;
};

#endif // WINDOW_CRYPTO_H
