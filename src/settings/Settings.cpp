#include "Settings.hpp"
#include "ui_Settings.h"

#include <QFileDialog>

Settings::Settings(std::shared_ptr<Vault> sp_vault, QWidget *parent)
    : QMainWindow(parent),
    vault(sp_vault)
{
    ui->setupUi(this);







}

Settings::~Settings()
{
    delete ui;
}

void Settings::BrowseFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Folder", QDir::rootPath(), QFileDialog::ShowDirsOnly);
    if (!dir.isEmpty() || QDir(dir).exists()){
        //ui->folderPathLabel->setText(dir);
    }



}

void Settings::CheckPathCondition()
{

}

void Settings::TryDisableEncryption()
{

}

void Settings::TryEnableEncryption()
{

}

void Settings::TrySetVaultEncryptionEnabled(bool b)
{

}

void Settings::TryChangePassword()
{

}
