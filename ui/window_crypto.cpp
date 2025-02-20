#include "window_crypto.h"
#include "ui_window_crypto.h"
#include "src/crypto.hpp"

#include <QProcess>
#include <QDebug>

#define DEBUG_LOCAL "[WINDOW_CRYPTO] "

window_crypto::window_crypto(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::window_crypto)
    , vault(new Vault)
{
    ui->setupUi(this);


    vault->dir = QDir("C:/Users/LSH/Documents/TestDirectory");
    vault->display_name = "TestDirectory";

}

window_crypto::~window_crypto()
{
    delete ui;
}

void window_crypto::on_password_input_lineedit_returnPressed()
{
    vault->password = "03ac674216f3e15c761ee1a5e255f067953623c8b388b4459e13f978d7c846f4";
    QString hashed_password = Crypto::SHA256(ui->password_input_lineedit->text());
    if (vault->password == hashed_password){
        qDebug() << DEBUG_LOCAL << " \033[32mCorrect password\033[0m";
        // unlock





    }else{
        qDebug() << DEBUG_LOCAL << "\033[31mWrong password\033[0m";
        qDebug() << "  input    : " << hashed_password;
        qDebug() << "  password : " << vault->password;
    }

}


void window_crypto::on_password_visibility_button_toggled(bool checked)
{
    if (checked){
        ui->password_input_lineedit->setEchoMode(QLineEdit::Normal);
    }else{
        ui->password_input_lineedit->setEchoMode(QLineEdit::Password);
    }
}




void window_crypto::on_openFolder_button_clicked()
{
    qDebug() << vault->dir;
    if (vault->dir.exists()){
        qDebug() << DEBUG_LOCAL << "opening";
        QStringList args = {vault->dir.path()};
        QProcess::startDetached("explorer.exe", args);
    }else{
        qDebug() << DEBUG_LOCAL "\033[31mError directory not exists\033[0m : " << vault->dir.path();
    }
}

