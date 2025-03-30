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

static const QString GreenStyleSheet = "QLabel{ border: 0px; color: rgb(0, 255, 0);}";
static const QString RedStyleSheet = "QLabel{ border: 0px; color: rgb(255, 0, 0);}";
static const QString DisabledStyleSheet = "QLabel{ border: 0px; color: rgb(100, 100, 100);}";
static const QString EnabledStyleSheet = "QLabel{ border: 0px; color: rgb(255, 255, 255);}";


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

    ui->DirectoryPathLabel->setText("");

    ui->VaultLabel->setStyleSheet(DisabledStyleSheet);
    ui->VaultLabel->setEnabled(false);
    ui->VaultNameLineedit->setText("");
    ui->VaultNameLineedit->setEnabled(false);

    ui->VaultAditionalInfoLabel->setText("the Vault will be created at the selected directory path");

    ui->PasswordLabel->setStyleSheet(DisabledStyleSheet);
    ui->PasswordLabel->setEnabled(false);
    ui->PasswordLineedit->setText("");
    ui->PasswordLineedit->setEnabled(false);
    ui->PasswordVisibilityButton->setEnabled(false);

    ui->PwConfirmLabel->setStyleSheet(DisabledStyleSheet);
    ui->PwConfirmLabel->setEnabled(false);
    ui->PwConfirmLineedit->setText("");
    ui->PwConfirmLineedit->setEnabled(false);
    ui->PwConfirmVisibilityButton->setEnabled(false);

    ui->CreateVaultButton->setEnabled(false);

    conditionPath = false;
    conditionPassword = false;
    conditionConfirm = false;
}

void FrontendNewVault::ConditionCheck()
{
    ui->VaultLabel->setStyleSheet(conditionPath ? GreenStyleSheet : RedStyleSheet);
    ui->PasswordLabel->setStyleSheet(conditionPassword ? GreenStyleSheet : RedStyleSheet);
    ui->PwConfirmLabel->setStyleSheet(conditionConfirm ? GreenStyleSheet : RedStyleSheet);

    if (conditionPath && conditionPassword && conditionConfirm){
        ui->CreateVaultButton->setEnabled(true);
    }else{
        ui->CreateVaultButton->setEnabled(false);
    }
}

void FrontendNewVault::on_DirectoryOpenButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Folder", QDir::rootPath(), QFileDialog::ShowDirsOnly);
    if (dir.isEmpty()) return;
    directory = QDir(dir);
    if (!directory.exists()) return;

    ui->DirectoryPathLabel->setText(dir);

    ui->VaultLabel->setEnabled(true);
    ui->VaultNameLineedit->setEnabled(true);

    ui->PasswordLabel->setEnabled(true);
    ui->PasswordLineedit->setEnabled(true);
    ui->PasswordVisibilityButton->setEnabled(true);


    ui->PwConfirmLabel->setEnabled(true);
    ui->PwConfirmLineedit->setEnabled(true);
    ui->PwConfirmVisibilityButton->setEnabled(true);

    ConditionCheck();
}

void FrontendNewVault::on_VaultNameLineedit_textEdited(const QString &arg1)
{
    //the Vault will be created at the selected directory path
    //the Vault will be created under the selected folder
    if (arg1.isEmpty()){
        ui->DirectoryPathLabel->setText(directory.path());
        ui->VaultAditionalInfoLabel->setText("the Vault will be created at the selected directory path");
        conditionPath = true;
    }else{
        ui->DirectoryPathLabel->setText(directory.path() + "/" + arg1);
        ui->VaultAditionalInfoLabel->setText("the Vault will be created under the selected folder");
        conditionPath =  fs::exists(ui->DirectoryPathLabel->text().toStdWString()) ? false : true;
    }
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

void FrontendNewVault::on_PasswordVisibilityButton_toggled(bool checked)
{
    ui->PasswordLineedit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
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

void FrontendNewVault::on_PwConfirmVisibilityButton_toggled(bool checked)
{
    ui->PwConfirmLineedit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
}

void FrontendNewVault::on_CreateVaultButton_clicked()
{
    Vault vault(ui->DirectoryPathLabel->text(), Crypto::SHA256(ui->PwConfirmLineedit->text()), mode);

    if (!vault.directory.exists()){
        vault.directory.mkpath(vault.directory.path());
    }
    if (!vault.backupDir.exists()){
        vault.backupDir.mkpath(vault.backupDir.path());
    }

    // create
    emit signal_create_vault(vault);
}
