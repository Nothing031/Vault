#include "window_crypto.h"
#include "ui_window_crypto.h"
#include "src/crypto.hpp"

#include <QProcess>
#include <QDebug>
#include <QUrl>
#include <QDesktopServices>
#include <QBrush>
#include <QColor>

#include "src/filelistmodel.hpp"

#define RED "\033[31m"
#define GREEN "\033[32m"
#define DEFAULT "\033[0m"
#define GRAY "\033[37m"

#define LOCAL_DEBUG() qDebug() << "[WINDOW_CRYPTO]"

window_crypto::window_crypto(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::window_crypto)
    , crypto(nullptr)
    , thread(nullptr)
{
    ui->setupUi(this);
    ui->files_listview->setModel(new FileListModel(this));




}

window_crypto::~window_crypto()
{
    delete ui;
}

void window_crypto::on_request_page(int index, Vault vault)
{
    persistence_index = index;
    this->vault = vault;

    ui->openFolder_button->setEnabled(true);
    ui->backup_button->setEnabled(true);
    ui->encrypt_button->setEnabled(false);
    ui->decrypt_button->setEnabled(false);
    ui->suspend_button->setEnabled(false);
    ui->progressBar->setValue(0);

    ui->directory_path_label->setText("");
    ui->vault_name_label->setText("");
    ui->password_input_lineedit->setText("");
}

void window_crypto::on_password_input_lineedit_returnPressed()
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
    if (vault.dir.exists()){
        LOCAL_DEBUG() << DEFAULT << "Opending Directory :" << vault.dir.path();
        QDesktopServices::openUrl(QUrl(vault.dir.path()));
    }else{
        LOCAL_DEBUG() << RED << "Error directory not exists :" << vault.dir.path() << DEFAULT;
    }
}

void window_crypto::on_encrypt_button_clicked()
{
    vault.UpdateIndex();
    if (thread || crypto){
        qDebug() << "[ERROR] Thread is running, try later";
        return;
    }
    if (vault.index_decrypted.size() == 0){
        qDebug() << "[ERROR] no any encryptable files";
        return;
    }
    ui->progressBar->setRange(0, vault.index_decrypted.size());
    ui->progressBar->setValue(0);

    thread = new QThread();
    crypto = new Crypto();
    crypto->moveToThread(thread);
    connect(thread, &QThread::started, this, [this](){
        emit request_setEnable_ui(false);
        qDebug() << "[ENCRYPTION]  Started";
        ui->encrypt_button->setEnabled(false);
        ui->decrypt_button->setEnabled(false);
        ui->backup_button->setEnabled(false);
        ui->suspend_button->setEnabled(true);
        QApplication::processEvents();
        crypto->AES256_Encrypt_All(vault);
    });
    connect(crypto, &Crypto::signal_done, this, [this](){
        UpdateDirectoryViewer();
        emit request_setEnable_ui(true);
        ui->encrypt_button->setEnabled(true);
        ui->decrypt_button->setEnabled(true);
        ui->backup_button->setEnabled(true);
        ui->suspend_button->setEnabled(false);
        thread->quit();
        thread->wait();
        delete crypto;
        crypto = nullptr;
        delete thread;
        thread = nullptr;
        qDebug() << "[ENCRYPTION]  Done";
    });
    thread->start();
}

void window_crypto::on_decrypt_button_clicked()
{
    vault.UpdateIndex();
    if (thread || crypto){
        qDebug() << "[ERROR] Thread is running, try later";
        return;
    }
    if (vault.index_encrypted.size() == 0){
        qDebug() << "[ERROR] no any decryptable files";
        return;
    }
    ui->progressBar->setRange(0, vault.index_encrypted.size());
    ui->progressBar->setValue(0);

    thread = new QThread();
    crypto = new Crypto();
    crypto->moveToThread(thread);
    connect(thread, &QThread::started, this, [this](){
        emit request_setEnable_ui(false);
        qDebug() << "[DECRYPTION]  Started";
        ui->encrypt_button->setEnabled(false);
        ui->decrypt_button->setEnabled(false);
        ui->backup_button->setEnabled(false);
        ui->suspend_button->setEnabled(true);
        QApplication::processEvents();
        crypto->AES256_Decrypt_All(vault);
    });
    connect(crypto, &Crypto::signal_done, this, [this](){
        UpdateDirectoryViewer();
        emit request_setEnable_ui(true);
        ui->encrypt_button->setEnabled(true);
        ui->decrypt_button->setEnabled(true);
        ui->backup_button->setEnabled(true);
        ui->suspend_button->setEnabled(false);
        thread->quit();
        thread->wait();
        delete crypto;
        crypto = nullptr;
        delete thread;
        thread = nullptr;
        qDebug() << "[ENCRYPTION]  Done";
    });

    thread->start();
}

void window_crypto::on_suspend_button_clicked()
{
    if (crypto || thread){
        qDebug() << "[CRYPTO]  Suspending";
        crypto->flag_run = false;
    }
    else{
        qDebug() << "[CRYPTO]  Error thread is not running";
    }
}

void window_crypto::on_backup_button_clicked()
{
    qDebug() << "[BACKUP]  Backup button pressed";
}

void window_crypto::on_vault_detach_button_clicked()
{
    emit request_detachVault(persistence_index);
    this->deleteLater();
}

