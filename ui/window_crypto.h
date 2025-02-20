#ifndef WINDOW_CRYPTO_H
#define WINDOW_CRYPTO_H

#include <QWidget>

#include "src/vault.h"

namespace Ui {
class window_crypto;
}

class window_crypto : public QWidget
{
    Q_OBJECT

public:
    explicit window_crypto(QWidget *parent = nullptr);
    ~window_crypto();


private slots:
    void on_password_input_lineedit_returnPressed();

    void on_password_visibility_button_toggled(bool checked);

    void on_openFolder_button_clicked();

private:


    Ui::window_crypto *ui;
    Vault *vault;
};

#endif // WINDOW_CRYPTO_H
