#include "window_crypto.h"
#include "ui_window_crypto.h"
#include "src/crypto.hpp"

#include <QProcess>
#include <QDebug>
#include <QUrl>
#include <QDesktopServices>

#define RED "\033[31m"
#define GREEN "\033[32m"
#define DEFAULT "\033[0m"
#define GRAY "\033[37m"

#define LOCAL_DEBUG() qDebug() << "[WINDOW_CRYPTO]"

Window_crypto::Window_crypto(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::window_crypto)
    , crypto(new Crypto)
    , vault()
{
    LOCAL_DEBUG() << DEFAULT << "Initializing...";
    ui->setupUi(this);

    // set enables
    ui->encrypt_button->setEnabled(false);
    ui->decrypt_button->setEnabled(false);
    ui->suspend_button->setEnabled(false);


}

Window_crypto::~Window_crypto()
{
    delete ui;
}

void Window_crypto::Load(const Vault &newVault)
{
    QElapsedTimer timer;
    vault = newVault;
    if (!vault.LoadFiles()){
        LOCAL_DEBUG() << RED << "Error Failed to load files" << DEFAULT;
        return;
    }

    //ui
    ui->encrypt_button->setEnabled(false);
    ui->decrypt_button->setEnabled(false);
    ui->suspend_button->setEnabled(false);

    ui->directory_path_label->setText(vault.dir.path());
    ui->vault_name_label->setText(vault.display_name);
    ui->directory_viewer->clear();

    timer.start();
    QColor Green(55, 255, 55);
    QColor Red(255, 55, 55);
    QListWidgetItem *item;
    LOCAL_DEBUG() << DEFAULT << "Adding to directory viewer..";
    for (const auto& file : vault.files){
        item = new QListWidgetItem(file.relativePath);
        item->foreground().setColor(file.encrypted ? Green : Red);
        ui->directory_viewer->addItem(item);
    }
    LOCAL_DEBUG() << DEFAULT << "Elapsed time :" << GREEN << timer.elapsed() << DEFAULT;
}

void Window_crypto::on_password_input_lineedit_returnPressed()
{
    QString hashed_password = Crypto::SHA256(ui->password_input_lineedit->text());
    if (vault.password == hashed_password){
        ui->password_input_lineedit->setEnabled(false);


        LOCAL_DEBUG() << GREEN << "Correct password" << DEFAULT;
        vault.CreateKey(ui->password_input_lineedit->text());
        ui->decrypt_button->setEnabled(true);
        ui->encrypt_button->setEnabled(true);
    }
    else{
        LOCAL_DEBUG() << RED << "Wrong password" << DEFAULT;
        qDebug() << GRAY << "  input    : " << hashed_password << DEFAULT;
        qDebug() << GRAY << "  password : " << vault.password << DEFAULT;
    }
}

void Window_crypto::on_password_visibility_button_toggled(bool checked)
{
    if (checked){
        ui->password_input_lineedit->setEchoMode(QLineEdit::Normal);
    }else{
        ui->password_input_lineedit->setEchoMode(QLineEdit::Password);
    }
}

void Window_crypto::on_openFolder_button_clicked()
{
    if (vault.dir.exists()){
        LOCAL_DEBUG() << DEFAULT << "Opending Directory :" << vault.dir.path();
        QDesktopServices::openUrl(QUrl(vault.dir.path()));
    }else{
        LOCAL_DEBUG() << RED << "Error directory not exists :" << vault.dir.path() << DEFAULT;
    }
}

void Window_crypto::on_encrypt_button_clicked()
{

}

void Window_crypto::on_decrypt_button_clicked()
{

}

void Window_crypto::on_suspend_button_clicked()
{

}

void Window_crypto::on_backup_button_clicked()
{

}

