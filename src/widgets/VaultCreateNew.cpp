#include "VaultCreateNew.hpp"
#include "ui_VaultCreateNew.h"

#include <QUrl>
#include <QDir>
#include <QThread>
#include <QPalette>
#include <QFileDialog>
#include <QApplication>
#include <QDesktopServices>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

#include "src/core/vault/VaultManager.hpp"
#include "src/widgets/PasswordWidget.hpp"

QString VaultCreateNew::m_styleRed      = R"(
    QWidget{ color: rgb(255, 55, 55); }
)";
QString VaultCreateNew::m_styleGrean    = R"(
    QWidget{ color: rgb(55, 255, 55); }
)";
QString VaultCreateNew::m_styleLightGray = R"(
    QWidget{ color: rgb(200, 200, 200); }
)";
QString VaultCreateNew::m_styleGray     = R"(
    QWidget{ color: rgb(150, 150, 150); }
)";
QString VaultCreateNew::m_styleDarkGray = R"(
    QWidget{ color: rgb(100, 100, 100); }
)";
QString VaultCreateNew::m_styleWhite    = R"(
    QWidget{ color: rgb(255, 255, 255); }
)";

VaultCreateNew::VaultCreateNew(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::VaultCreateNew)
{
    ui->setupUi(this);

    QRegularExpression pathReg(R"([^\\/:*?\"<>|]*)");
    QRegularExpressionValidator *pathValidator = new QRegularExpressionValidator(pathReg, this);


    connect(ui->NameLineEdit, &QLineEdit::textEdited, this, &VaultCreateNew::UpdatePathCondition);
    connect(ui->FolderBrowseButton, &QPushButton::clicked, this, &VaultCreateNew::BrowseFolder);
    connect(ui->BackButton, &QPushButton::clicked, this, &VaultCreateNew::Back);

    connect(ui->EncryptionCheckbox, &QCheckBox::checkStateChanged, this, &VaultCreateNew::EnableEncryptionSetting);
    connect(ui->AditionalEncryptionInfoButton, &QPushButton::clicked, this, &VaultCreateNew::OpenAditionalEncryptionInfo);

    connect(ui->CreateButton, &QPushButton::clicked, this, &VaultCreateNew::CreateVault);
}

VaultCreateNew::~VaultCreateNew()
{
    delete ui;
}

void VaultCreateNew::init()
{
    // clear
    ui->FolderPathLabel->clear();
    ui->NameLineEdit->clear();
    ui->NameInfoLabel->clear();

    ui->CreateButton->setText("Create");

    // enable
    ui->BackButton->setEnabled(true);
    ui->FolderBrowseButton->setEnabled(true);
    ui->NameLineEdit->setEnabled(true);
    ui->EncryptionCheckbox->setEnabled(true);
    ui->CreateButton->setEnabled(false);

    // state
    ui->EncryptionCheckbox->setChecked(true);

    // init members
    rootDirectory.clear();
    isAES256EncryptionEnabled = true;
    isPathValid = false;
    isPasswordValid = false;
    isConfirmValid = true;

    updateStyle(UiOption::Init);
}

void VaultCreateNew::EnableEncryptionSetting(Qt::CheckState checkState)
{
    bool b = checkState == Qt::Checked ? true : false;
    isAES256EncryptionEnabled = b;
    updateStyle(UiOption::EncryptionOption);
    CheckCondition();
}

void VaultCreateNew::UpdatePathCondition(const QString &arg1)
{
    ui->FolderPathLabel->setText(rootDirectory);

    if (rootDirectory.isEmpty()){
        isPathValid = false;
    }
    else if (arg1.isEmpty()){
        isPathValid = QDir().exists(rootDirectory);
        ui->NameInfoLabel->setText(isPathValid ? "vault will be set to the selected folder" : "folder not exists");
    }
    else{
        isPathValid = !QDir().exists(rootDirectory + "/" + arg1);
        ui->NameInfoLabel->setText(isPathValid ? "vault will be created under the selected folder" : "folder already exists");
    }

    updateStyle(UiOption::Path);
    CheckCondition();
}

void VaultCreateNew::BrowseFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Folder", QDir::rootPath(), QFileDialog::ShowDirsOnly);
    if (!dir.isEmpty()){
        rootDirectory = dir;
    }

    UpdatePathCondition(ui->NameLineEdit->text());

    updateStyle(UiOption::Path);
    CheckCondition();
}

void VaultCreateNew::updateStyle(int option)
{
    if (option & UiOption::Init){
        ui->NameInfoLabel->setStyleSheet(m_styleLightGray);
        ui->EncryptionCheckbox->setStyleSheet(m_styleWhite);
    }

    if (option & UiOption::Path){
        ui->NameInfoLabel->setStyleSheet(isPathValid ? m_styleLightGray : m_styleRed);
    }
    if (option & UiOption::EncryptionOption){
        bool& b = isAES256EncryptionEnabled;
        ui->EncryptionCheckbox->setStyleSheet(b ? m_styleWhite : m_styleGray);
    }
}

void VaultCreateNew::CheckCondition()
{
    if (!isPathValid) goto disable;
    if (isAES256EncryptionEnabled && (!isPasswordValid || !isConfirmValid)) goto disable;

    // enable
    ui->CreateButton->setEnabled(true);
    return;
disable:
    ui->CreateButton->setEnabled(false);
    return;
}

void VaultCreateNew::OpenAditionalEncryptionInfo()
{
    QString link = "https://github.com/Nothing031/Vault/blob/v0.3.0/README.md";
    QDesktopServices::openUrl(QUrl(link));
}

void VaultCreateNew::CreateVault()
{
    qDebug() << "creating vault";
    ui->BackButton->setEnabled(false);
    ui->FolderBrowseButton->setEnabled(false);
    ui->NameLineEdit->setEnabled(false);
    ui->EncryptionCheckbox->setEnabled(false);
    // password wiget

    ui->CreateButton->setEnabled(false);
    ui->CreateButton->setText("Creating Vault");

    return;

    QString password = "";
    QString dir  = rootDirectory + "/" + ui->NameLineEdit->text();
    bool    enableEncryption = isAES256EncryptionEnabled;

    QThread *thread = QThread::create([enableEncryption, password, dir](){
        VaultManager& manager = VaultManager::GetInstance();
        manager.CreateVault(enableEncryption, dir, password);
    });
    connect(thread, &QThread::finished, this, &VaultCreateNew::Back);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

