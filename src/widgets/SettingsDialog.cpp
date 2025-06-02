#include "SettingsDialog.hpp"
#include "ui_SettingsDialog.h"

#include <dwmapi.h>
#include <Windows.h>

#include <QUrl>
#include <QMessageBox>

#include "src/core/vault/Vault.hpp"
#include "src/core/vault/VaultLoader.hpp"
#include "src/core/vault/VaultManager.hpp"


SettingsDialog::SettingsDialog(std::shared_ptr<Vault> spVault , QWidget* parent) :
    QDialog(parent),
    vault(spVault),
    conditionPath(false)
{
    ui->setupUi(this);
    // dark caption
    HWND hWnd = (HWND)this->winId();
    BOOL USE_DARK_MODE = true;
    DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &USE_DARK_MODE, sizeof(USE_DARK_MODE));

    // init
    QFileInfo info(vault->directory.path());
    ui->folderPathLabel->setText(info.dir().path());
    ui->nameLineEdit->setText(info.dir().path());
    ui->nameInfoLabel->clear();
    if (vault->aes.IsEnabled()){
        ui->enableEncryptionButton->setText("Disable AES-256 encryption");
        ui->changePasswordButton->setEnabled(true);
    }else{
        ui->enableEncryptionButton->setText("Enable AES-256 encryption");
        ui->changePasswordButton->setEnabled(false);
    }
    ui->discardButton->setEnabled(true);
    ui->saveButton->setEnabled(false);

    // connect
    connect(ui->browseButton, &QPushButton::clicked, this, &SettingsDialog::BrowseFolder);
    connect(ui->nameLineEdit, &QLineEdit::textEdited, this, &SettingsDialog::CheckPath);
    connect(ui->enableEncryptionButton, &QPushButton::clicked, this, &SettingsDialog::TryToggleEncryption);
    connect(ui->changePasswordButton, &QPushButton::clicked, this, &SettingsDialog::TryChangePassword);




}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::BrowseFolder()
{



}

void SettingsDialog::CheckPath()
{

}

bool SettingsDialog::hasDifferent()
{
    return false;
}

void SettingsDialog::TryToggleEncryption()
{
    if (vault->aes.IsEnabled()){
        vault->LoadFiles();
        bool passed = true;
        for (auto& file : std::as_const(vault->files)){
            if (file->state != FileInfo::PLAIN_GOOD){
                passed = false;
                break;
            }
        }
        if (passed){
            vault->aes.SetEnabled(false);
            VaultLoader::GetInstance().SaveVault(vault);
            return;
        }else{
            QMessageBox::warning(this, "Failed to disable AES-256 encryption", "Please decrypt all and try again");
        }
    }else{
        // set password



        // enable



        // save
        VaultLoader::GetInstance().SaveVault(vault);
    }
}

void SettingsDialog::TryChangePassword()
{

}




