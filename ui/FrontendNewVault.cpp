#include "FrontendNewVault.h"
#include "ui_FrontendNewVault.h"

#include "src/crypto.hpp"
#include "src/enviroment.hpp"

#include <filesystem>

#include <QString>
#include <QFileDialog>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

namespace fs = std::filesystem;

FrontendNewVault::FrontendNewVault(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FrontendNewVault)
{
    ui->setupUi(this);

    // validator
    QRegularExpression path_re(R"([^\\/:*?\"<>|]*)");
    QRegularExpression password_re(R"(^[A-Za-z0-9!@#$%^&*()_+\-=\[\]{};':\"\\|,.<>\\/?]*$)");
    QRegularExpressionValidator *path_validator = new QRegularExpressionValidator(path_re, this);
    QRegularExpressionValidator *password_validator = new QRegularExpressionValidator(password_re, this);

    ui->VaultNameLineedit->setValidator(path_validator);
    ui->PasswordLineedit->setValidator(password_validator);
    ui->PwConfirmLineedit->setValidator(password_validator);
}

FrontendNewVault::~FrontendNewVault()
{
    delete ui;
}


void FrontendNewVault::init(const Mode &mode)
{
    this->mode = mode;
    ui->VaultLabel->setEnabled(false);
    ui->VaultNameLineedit->setEnabled(false);

    ui->PasswordLabel->setEnabled(false);
    ui->PasswordLineedit->setEnabled(false);
    ui->PasswordVisibilityButton->setEnabled(false);
    ui->PwConfirmLabel->setEnabled(false);
    ui->PwConfirmLineedit->setEnabled(false);
    ui->PwConfirmVisibilityButton->setEnabled(false);
    ui->CreateVaultButton->setEnabled(false);

    QString style = "QWidget{color: rgb(255, 255, 255);}";
    ui->VaultLabel->setStyleSheet(style);
    ui->PasswordLabel->setStyleSheet(style);
    ui->PwConfirmLabel->setStyleSheet(style);


    ui->DirectoryPathLabel->setText("");
    ui->VaultNameLineedit->setText("");
    ui->PasswordLineedit->setText("");
    ui->PwConfirmLineedit->setText("");
}

void FrontendNewVault::ConditionCheck()
{
    QString styleSheet;
    if (NewVaultMode == NewVault::CreateNew){
        QString styleSheet = "QLabel{color:rgb";
        styleSheet += (conditionPath ? "(55, 255, 55);}" : "(255, 55, 55);}");
        ui->VaultLabel->setStyleSheet(styleSheet);
    }
    styleSheet = "QLabel{color:rgb";
    styleSheet += (conditionPassword ? "(55, 255, 55);}" : "(255, 55, 55);}");
    ui->PasswordLabel->setStyleSheet(styleSheet);

    styleSheet = "QLabel{color:rgb";
    styleSheet += (conditionConfirm ? "(55, 255, 55);}" : "(255, 55, 55);}");
    ui->PwConfirmLabel->setStyleSheet(styleSheet);

    if (conditionPath && conditionPassword && conditionConfirm){
        ui->CreateVaultButton->setEnabled(true);
    }
}


void FrontendNewVault::on_vault_create_clicked()
{
    Vault vault(ui->DirectoryPathLabel->text(), Crypto::SHA256(ui->PwConfirmLineedit->text()), ENV_BACKUPDIR_LENGTH);

    if (!vault.dir.exists()){
        vault.dir.mkpath(vault.dir.path());
    }
    if (!vault.backupDir.exists()){
        vault.backupDir.mkpath(vault.backupDir.path());
    }

    // create
    emit signal_create_new_vault(vault);
}

void FrontendNewVault::on_VaultNameLineedit_textEdited(const QString &arg1)
{
    ui->DirectoryPathLabel->setText(directory.path() + "/" + arg1);
    conditionPath = (!fs::exists(ui->DirectoryPathLabel->text().toStdWString()) ? true : false);
    ConditionCheck();
}


void FrontendNewVault::on_PasswordLineedit_textEdited(const QString &arg1)
{
    conditionPassword = !(arg1.size() < 4);
    if (conditionPassword && arg1 == ui->PwConfirmLineedit->text()){
        conditionConfirm = true;
    }else{
        conditionConfirm = false;
    }
    ConditionCheck();
}


void FrontendNewVault::on_PwConfirmLineedit_textEdited(const QString &arg1)
{
    if (arg1 == ui->PasswordLineedit->text() && !(arg1.size() < 4)){
        conditionConfirm = true;
    }else{
        conditionConfirm = false;
    }
    ConditionCheck();
}


void FrontendNewVault::on_PasswordVisibilityButton_toggled(bool checked)
{
    ui->PasswordLineedit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
}

void FrontendNewVault::on_DirectoryOpenButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Folder", QDir::rootPath(), QFileDialog::ShowDirsOnly);
    if (dir.isEmpty()) return;
    directory = QDir(dir);
    if (!directory.exists()) return;
    ui->DirectoryPathLabel->setText(dir);
    if (mode == Mode::Local){
        ui->VaultLabel->setEnabled(true);
        ui->VaultNameLineedit->setEnabled(true);
    }else{
        conditionPath = true;
    }
    ui->PasswordLabel->setEnabled(true);
    ui->PasswordLineedit->setEnabled(true);
    ui->PasswordVisibilityButton->setEnabled(true);
    ui->PwConfirmLabel->setEnabled(true);
    ui->PwConfirmLineedit->setEnabled(true);
    ui->PwConfirmVisibilityButton->setEnabled(true);
    ConditionCheck();
}


void FrontendNewVault::on_PwConfirmVisibilityButton_toggled(bool checked)
{
    ui->PwConfirmLineedit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
}

