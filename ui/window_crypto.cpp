#include "window_crypto.h"
#include "ui_window_crypto.h"
#include "src/crypto.hpp"

#include <QProcess>
#include <QDebug>
#include <QUrl>
#include <QDesktopServices>
#include <QBrush>
#include <QColor>
#include <QPropertyAnimation>
#include <QTimer>

#include "src/filelistmodel.hpp"

window_crypto::window_crypto(QWidget *parent)
    : QWidget(parent)
    , model(new FileListModel(this))
    , ui(new Ui::window_crypto)
    , thread(nullptr)
{
    ui->setupUi(this);
    ui->files_listview->setModel(model);



    connect(&crypto, &Crypto::signal_start, this, [this](){
        ui->terminal_textedit->clear();
        emit request_setEnable_ui(false);
        ui->encrypt_button->setEnabled(false);
        ui->decrypt_button->setEnabled(false);
        ui->refresh_button->setEnabled(false);
        ui->backup_button->setEnabled(false);
        ui->suspend_button->setEnabled(true);
    });
    connect(&crypto, &Crypto::signal_done, this, [this](){
        emit request_setEnable_ui(true);
        ui->encrypt_button->setEnabled(true);
        ui->decrypt_button->setEnabled(true);
        ui->refresh_button->setEnabled(true);
        ui->backup_button->setEnabled(true);
        ui->suspend_button->setEnabled(false);
        model->update();
        QTimer::singleShot(1000, [this]{
            ui->progressBar->setValue(0);
        });
    });
    connect(&crypto, &Crypto::signal_progress, this, [this](int value){
        ui->progressBar->setValue(value);
    });
    connect(&crypto, &Crypto::signal_terminal_message, this, [this](QStringList messages){
        for (auto& message : std::as_const(messages)){
            ui->terminal_textedit->append(message);
        }        
    });
    connect(&crypto, &Crypto::signal_terminal_clear, this, [this](){
        ui->terminal_textedit->clear();
    });
}

window_crypto::~window_crypto()
{
    delete ui;
}

void window_crypto::on_request_page(Vault* pvault)
{
    this->pVault = pvault;
    this->pVault->LoadFiles();

    model->loadVault(pvault);

    ui->openFolder_button->setEnabled(true);
    ui->backup_button->setEnabled(true);
    ui->encrypt_button->setEnabled(false);
    ui->decrypt_button->setEnabled(false);
    ui->suspend_button->setEnabled(false);
    ui->progressBar->setValue(0);

    ui->terminal_textedit->clear();
    ui->directory_path_label->setText(pVault->dir.path());
    ui->vault_name_label->setText(pVault->displayName);
    ui->password_input_lineedit->setText("");
}

void window_crypto::on_password_input_lineedit_returnPressed()
{
    QString hashed_password = Crypto::SHA256(ui->password_input_lineedit->text());
    if (pVault->password == hashed_password){
        ui->password_input_lineedit->setEnabled(false);
        qDebug() << "Correct password";
        pVault->CreateKey(ui->password_input_lineedit->text());
        ui->decrypt_button->setEnabled(true);
        ui->encrypt_button->setEnabled(true);
    }
    else{
        qDebug() << "Wrong password";
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

//# buttons ###############################################

void window_crypto::on_vault_detach_button_clicked()
{
    emit request_detachVault(pVault);
}

void window_crypto::on_openFolder_button_clicked()
{
    if (pVault->dir.exists()){
        QDesktopServices::openUrl(QUrl(pVault->dir.path()));
    }else{
        qDebug() << "Error directory not exists :" << pVault->dir.path();
    }
}

void window_crypto::on_encrypt_button_clicked()
{
    pVault->UpdateIndex();
    if (thread && thread->isRunning()){
        qDebug() << "[ERROR] Thread is running, try later";
        return;
    }
    if (pVault->plainIndex.size() == 0){
        qDebug() << "[ERROR] no any encryptable files";
        return;
    }
    ui->progressBar->setRange(0, pVault->plainIndex.size());
    ui->progressBar->setValue(0);

    if (thread) thread->deleteLater();

    thread = QThread::create([&](){
        crypto.AES256_Encrypt_All(pVault);
    });

    thread->start();
}

void window_crypto::on_decrypt_button_clicked()
{
    pVault->UpdateIndex();
    if (thread && thread->isRunning()){
        qDebug() << "[ERROR] Thread is running, try later";
        return;
    }
    if (pVault->cipherIndex.size() == 0){
        qDebug() << "[ERROR] no any decryptable files";
        return;
    }
    ui->progressBar->setRange(0, pVault->cipherIndex.size());
    ui->progressBar->setValue(0);

    if (thread) thread->deleteLater();

    thread = QThread::create([&](){
        crypto.AES256_Decrypt_All(pVault);
    });

    thread->start();
}

void window_crypto::on_suspend_button_clicked()
{
    if (thread && thread->isRunning()){
        qDebug() << "[CRYPTO]  Suspending";
        crypto.flag_suspend = true;
    }
    else{
        qDebug() << "[CRYPTO]  Error thread is not running";
    }
}

void window_crypto::on_backup_button_clicked()
{
    qDebug() << "Backup button pressed";
    qDebug() << "backup dir";
}

void window_crypto::on_refresh_button_clicked()
{
    pVault->LoadFiles();
    model->loadVault(pVault);
}
