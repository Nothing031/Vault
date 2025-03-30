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
    ui->FileListView->setModel(model);



    connect(&crypto, &Crypto::signal_start, this, [this](){
        ui->TerminalTextedit->clear();
        ui->TerminalTextedit->append("Starting...");
        emit request_setEnable_ui(false);
        ui->EncryptButton->setEnabled(false);
        ui->DecryptButton->setEnabled(false);
        ui->RefreshButton->setEnabled(false);
        ui->SuspendButton->setEnabled(true);
    });
    connect(&crypto, &Crypto::signal_done, this, [this](){
        emit request_setEnable_ui(true);
        ui->EncryptButton->setEnabled(true);
        ui->DecryptButton->setEnabled(true);
        ui->RefreshButton->setEnabled(true);
        ui->SuspendButton->setEnabled(false);
        model->update();
        QTimer::singleShot(1000, [this]{
            ui->ProgressBar->setValue(0);
        });
    });
    connect(&crypto, &Crypto::signal_progress, this, [this](int value){
        ui->ProgressBar->setValue(value);
    });
    connect(&crypto, &Crypto::signal_terminal_message, this, [this](QStringList messages){
        for (auto& message : std::as_const(messages)){
            ui->TerminalTextedit->append(message);
        }        
    });
    connect(&crypto, &Crypto::signal_terminal_clear, this, [this](){
        ui->TerminalTextedit->clear();
    });
}

FrontendVault::~FrontendVault()
{
    delete ui;
}

void FrontendVault::init(Vault* pvault)
{
    this->pVault = pvault;
    this->pVault->LoadFiles();

    model->loadVault(pvault);

    ui->PasswordLineedit->setEnabled(true);

    ui->OpenFolderButton->setEnabled(true);
    ui->EncryptButton->setEnabled(false);
    ui->DecryptButton->setEnabled(false);
    ui->SuspendButton->setEnabled(false);
    ui->ProgressBar->setValue(0);

    ui->TerminalTextedit->clear();
    ui->FolderLabel->setText(pVault->directory.path());
    ui->VaultNameLabel->setText(pVault->name);
    ui->PasswordLineedit->setText("");
}

void FrontendVault::on_PasswordLineedit_returnPressed()
{
    QString hashed_password = Crypto::SHA256(ui->PasswordLineedit->text());
    if (pVault->sha256Password == hashed_password){
        ui->PasswordLineedit->setEnabled(false);
        qDebug() << "Correct password";
        pVault->CreateKey(ui->PasswordLineedit->text());
        ui->DecryptButton->setEnabled(true);
        ui->EncryptButton->setEnabled(true);
    }
    else{
        qDebug() << "Wrong password";
    }
}

void FrontendVault::on_PasswordVisibilityButton_toggled(bool checked)
{
    if (checked){
        ui->PasswordLineedit->setEchoMode(QLineEdit::Normal);
    }else{
        ui->PasswordLineedit->setEchoMode(QLineEdit::Password);
    }
}

//# buttons ###############################################

void FrontendVault::on_DetachVaultButton_clicked()
{
    emit request_detachVault(pVault);
}

void FrontendVault::on_OpenFolderButton_clicked()
{
    if (pVault->directory.exists()){
        QDesktopServices::openUrl(QUrl(pVault->directory.path()));
    }else{
        qDebug() << "Error directory not exists :" << pVault->directory.path();
    }
}

void FrontendVault::on_EncryptButton_clicked()
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
    ui->ProgressBar->setRange(0, pVault->plainIndex.size());
    ui->ProgressBar->setValue(0);

    if (thread) thread->deleteLater();

    thread = QThread::create([&](){
        crypto.AES256_Encrypt_All(pVault);
    });

    thread->start();
}

void FrontendVault::on_DecryptButton_clicked()
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
    ui->ProgressBar->setRange(0, pVault->cipherIndex.size());
    ui->ProgressBar->setValue(0);

    if (thread) thread->deleteLater();

    thread = QThread::create([&](){
        crypto.AES256_Decrypt_All(pVault);
    });

    thread->start();
}

void FrontendVault::on_SuspendButton_clicked()
{
    if (thread && thread->isRunning()){
        qDebug() << "[CRYPTO]  Suspending";
        crypto.flag_suspend = true;
    }
    else{
        qDebug() << "[CRYPTO]  Error thread is not running";
    }
}

void FrontendVault::on_RefreshButton_clicked()
{
    pVault->LoadFiles();
    model->loadVault(pVault);
}
