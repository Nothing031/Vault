#include "window_crypto.h"
#include "ui_window_crypto.h"
#include "src/crypto.hpp"

#include <QProcess>
#include <QDebug>
#include <QUrl>
#include <QDesktopServices>
#include <QBrush>
#include <QColor>

#define RED "\033[31m"
#define GREEN "\033[32m"
#define DEFAULT "\033[0m"
#define GRAY "\033[37m"

#define LOCAL_DEBUG() qDebug() << "[WINDOW_CRYPTO]"

Window_crypto::Window_crypto(QWidget *parent, Vault parent_vault)
    : QWidget(parent)
    , ui(new Ui::window_crypto)
    , crypto(nullptr)
    , thread(nullptr)
    , vault(parent_vault)
{
    LOCAL_DEBUG() << DEFAULT << "Initializing...";
    ui->setupUi(this);

    // set enables
    ui->openFolder_button->setEnabled(true);
    ui->backup_button->setEnabled(true);
    ui->encrypt_button->setEnabled(false);
    ui->decrypt_button->setEnabled(false);
    ui->suspend_button->setEnabled(false);
    ui->progressBar->setValue(0);


    ui->directory_path_label->setText("");
    ui->vault_name_label->setText("");
    ui->password_input_lineedit->setText("");
    ui->directory_viewer->clear();

    Load(vault);
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
    QBrush Red = QBrush(QColor(255, 55, 55));
    QBrush Green = QBrush(QColor(55, 255, 55));

    QBrush QBrush(QColor(255, 55, 55));
    QListWidgetItem *item;
    LOCAL_DEBUG() << DEFAULT << "Adding to directory viewer..";
    QApplication::processEvents();
    for (const auto& file : vault.files){
        item = new QListWidgetItem(file.relativePath);
        item->setForeground(file.encrypted ? Green : Red);
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
    vault.UpdateIndex();
    ui->progressBar->setRange(0, vault.index_decrypted.size());
    ui->progressBar->setValue(0);
    if (thread || crypto){
        qDebug() << "[ERROR] Thread is running, try later";
        return;
    }
    if (vault.index_decrypted.size() == 0){
        qDebug() << "[ERROR] no any encryptable files";
        return;
    }

    thread = new QThread();
    crypto = new Crypto();
    crypto->moveToThread(thread);
    connect(thread, &QThread::started, this, [this](){
        qDebug() << "[ENCRYPTION]  Started";
        ui->encrypt_button->setEnabled(false);
        ui->decrypt_button->setEnabled(false);
        ui->backup_button->setEnabled(false);
        ui->suspend_button->setEnabled(true);
        QApplication::processEvents();
        crypto->AES256_Encrypt_All(vault);
    });
    connect(crypto, &Crypto::signal_done, this, [this](){
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

void Window_crypto::on_decrypt_button_clicked()
{
    vault.UpdateIndex();
    ui->progressBar->setRange(0, vault.index_decrypted.size());
    ui->progressBar->setValue(0);
    if (thread || crypto){
        qDebug() << "[ERROR] Thread is running, try later";
        return;
    }
    if (vault.index_encrypted.size() == 0){
        qDebug() << "[ERROR] no any decryptable files";
        return;
    }

    thread = new QThread();
    crypto = new Crypto();
    crypto->moveToThread(thread);
    connect(thread, &QThread::started, this, [this](){
        qDebug() << "[DECRYPTION]  Started";
        ui->encrypt_button->setEnabled(false);
        ui->decrypt_button->setEnabled(false);
        ui->backup_button->setEnabled(false);
        ui->suspend_button->setEnabled(true);
        QApplication::processEvents();
        crypto->AES256_Decrypt_All(vault);
    });
    connect(crypto, &Crypto::signal_done, this, [this](){
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

void Window_crypto::on_suspend_button_clicked()
{
    if (crypto || thread){
        qDebug() << "[CRYPTO]  Suspending";
        crypto->flag_run = false;
    }
    else{
        qDebug() << "[CRYPTO]  Error thread is not running";
    }
}

void Window_crypto::on_backup_button_clicked()
{
    qDebug() << "[BACKUP]  Backup button pressed";

}

