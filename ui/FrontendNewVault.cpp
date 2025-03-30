#include "FrontendNewVault.h"
#include "ui_FrontendNewVault.h"

#include "src/crypto.hpp"

#include <filesystem>

#include <QString>
#include <QFileDialog>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

namespace fs = std::filesystem;

inline QString GreenStyleSheet = "QLabel{ border: 0px; color: rgb(0, 255, 0);}";
inline QString RedStyleSheet = "QLabel{ border: 0px; color: rgb(255, 0, 0);}";
inline QString DisabledStyleSheet = "QLabel{ border: 0px; color: rgb(100, 100, 100);}";
inline QString EnabledStyleSheet = QStringLiteral("QLabel{ border: 0px; color: rgb(255, 255, 255);}");

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

    // connect
    connect(ui->DirectoryOpenButton, &QPushButton::pressed, this, &FrontendNewVault::GetDirectory);

    connect(ui->VaultNameLineedit, &QLineEdit::textEdited, this, &FrontendNewVault::CheckVaultPath);

    connect(ui->PasswordLineedit, &QLineEdit::textEdited, this, &FrontendNewVault::CheckPassword);
    connect(ui->PasswordVisibilityButton, &QPushButton::toggled, this, [this](bool b){
        ui->PasswordLineedit->setEchoMode(b ? QLineEdit::Normal : QLineEdit::Password);
    });

    connect(ui->PwConfirmLineedit, &QLineEdit::textEdited, this, &FrontendNewVault::CheckPwConfirm);
    connect(ui->PwConfirmVisibilityButton, &QPushButton::toggled, this, [this](bool b){
        ui->PwConfirmLineedit->setEchoMode(b ? QLineEdit::Normal : QLineEdit::Password);
    });

    connect(ui->CreateVaultButton, &QPushButton::clicked, this, [this](){
        Vault vault(ui->DirectoryPathLabel->text(), Crypto::SHA256(ui->PwConfirmLineedit->text()), mode);

        if (!vault.directory.exists()){
            vault.directory.mkpath(vault.directory.path());
        }
        if (!vault.backupDir.exists()){
            vault.backupDir.mkpath(vault.backupDir.path());
        }

        // create
        emit signal_create_vault(vault);
    });
}

FrontendNewVault::~FrontendNewVault()
{
    delete ui;
}

void FrontendNewVault::init(const Mode mode)
{
    this->mode = mode;

    ui->ModeLabel->setText(mode == Mode::Local ? "Local" : "Portable");

    ui->DirectoryPathLabel->setText("");

    ui->VaultLabel->setStyleSheet(DisabledStyleSheet);
    ui->VaultLabel->setEnabled(false);
    ui->VaultNameLineedit->setText("");
    ui->VaultNameLineedit->setEnabled(false);

    ui->VaultAditionalInfoLabel->setText("Vault will be connected to the selected folder");

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

    conditionPath = true;
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

void FrontendNewVault::GetDirectory()
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

void FrontendNewVault::CheckVaultPath(const QString &arg1)
{
    //the Vault will be created at the selected directory path
    //the Vault will be created under the selected folder
    if (arg1.isEmpty()){
        ui->DirectoryPathLabel->setText(directory.path());
        ui->VaultAditionalInfoLabel->setText("Vault will be connected to the selected folder");
        conditionPath = true;
    }else{
        ui->DirectoryPathLabel->setText(directory.path() + "/" + arg1);
        ui->VaultAditionalInfoLabel->setText("Vault will be created at the selected folder");
        conditionPath =  fs::exists(ui->DirectoryPathLabel->text().toStdWString()) ? false : true;
    }
    ConditionCheck();
}

void FrontendNewVault::CheckPassword(const QString &arg1)
{
    conditionPassword = !(arg1.size() < 4);
    if (conditionPassword && arg1 == ui->PwConfirmLineedit->text()){
        conditionConfirm = true;
    }else{
        conditionConfirm = false;
    }
    ConditionCheck();
}

void FrontendNewVault::CheckPwConfirm(const QString &arg1)
{
    if (arg1 == ui->PasswordLineedit->text() && !(arg1.size() < 4)){
        conditionConfirm = true;
    }else{
        conditionConfirm = false;
    }
    ConditionCheck();
}


