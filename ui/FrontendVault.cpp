#include "FrontendVault.h"
#include "ui_FrontendVault.h"
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

FrontendVault::FrontendVault(QWidget *parent)
    : QWidget(parent)
    , model(new FileListModel(this))
    , ui(new Ui::FrontendVault)
    , thread(nullptr)
{
    ui->setupUi(this);
    ui->files_listview->setModel(model);



    connect(&crypto, &Crypto::signal_start, this, [this](){
        ui->terminal_textedit->clear();
        ui->terminal_textedit->append("Starting...");
        emit request_setEnable_ui(false);
        ui->encrypt_button->setEnabled(false);
        ui->decrypt_button->setEnabled(false);
        ui->refresh_button->setEnabled(false);
        ui->suspend_button->setEnabled(true);
    });
    connect(&crypto, &Crypto::signal_done, this, [this](){
        emit request_setEnable_ui(true);
        ui->encrypt_button->setEnabled(true);
        ui->decrypt_button->setEnabled(true);
        ui->refresh_button->setEnabled(true);
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

FrontendVault::~FrontendVault()
{
    delete ui;
}

void FrontendVault::on_request_page(Vault* pvault)
{
    this->pVault = pvault;
    this->pVault->LoadFiles();

    model->loadVault(pvault);

    ui->password_input_lineedit->setEnabled(true);

    ui->openFolder_button->setEnabled(true);
    ui->encrypt_button->setEnabled(false);
    ui->decrypt_button->setEnabled(false);
    ui->suspend_button->setEnabled(false);
    ui->progressBar->setValue(0);

    ui->terminal_textedit->clear();
    ui->directory_path_label->setText(pVault->directory.path());
    ui->vault_name_label->setText(pVault->name);
    ui->password_input_lineedit->setText("");
}

void FrontendVault::on_password_input_lineedit_returnPressed()
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

void FrontendVault::on_password_visibility_button_toggled(bool checked)
{
    if (checked){
        ui->password_input_lineedit->setEchoMode(QLineEdit::Normal);
    }else{
        ui->password_input_lineedit->setEchoMode(QLineEdit::Password);
    }
}

//# buttons ###############################################

void FrontendVault::on_vault_detach_button_clicked()
{
    emit request_detachVault(pVault);
}

void FrontendVault::on_openFolder_button_clicked()
{
    if (pVault->directory.exists()){
        QDesktopServices::openUrl(QUrl(pVault->directory.path()));
    }else{
        qDebug() << "Error directory not exists :" << pVault->directory.path();
    }
}

void FrontendVault::on_encrypt_button_clicked()
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

void FrontendVault::on_decrypt_button_clicked()
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

void FrontendVault::on_suspend_button_clicked()
{
    if (thread && thread->isRunning()){
        qDebug() << "[CRYPTO]  Suspending";
        crypto.flag_suspend = true;
    }
    else{
        qDebug() << "[CRYPTO]  Error thread is not running";
    }
}

void FrontendVault::on_refresh_button_clicked()
{
    pVault->LoadFiles();
    model->loadVault(pVault);
}
