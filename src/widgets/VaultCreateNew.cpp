#include "VaultCreateNew.hpp"

#include <QDesktopServices>
#include <QUrl>
#include <QPalette>
#include <QDir>
#include <QApplication>
#include <QFileDialog>
#include <QThread>


#include "src/core/vault/Vault.hpp"
#include "src/core/vault/VaultManager.hpp"


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


    connect(ui->NameLineEdit, &QLineEdit::textEdited, this, &VaultCreateNew::UpdatePathCondition);
    connect(ui->PasswordLineEdit, &QLineEdit::textEdited, this, &VaultCreateNew::UpdatePasswordCondition);
    connect(ui->ConfirmLineEdit, &QLineEdit::textEdited, this, &VaultCreateNew::UpdatePasswordCondition);
    connect(ui->FolderBrowseButton, &QPushButton::clicked, this, &VaultCreateNew::BrowseFolder);
    connect(ui->BackButton, &QPushButton::clicked, this, &VaultCreateNew::Back);

    connect(ui->EncryptionCheckbox, &QCheckBox::checkStateChanged, this, &VaultCreateNew::EnableEncryptionSetting);
    connect(ui->AditionalEncryptionInfoButton, &QPushButton::clicked, this, &VaultCreateNew::OpenAditionalEncryptionInfo);
    connect(ui->PasswordVisibilityButton, &QPushButton::toggled, this, [this](bool b){ ui->PasswordLineEdit->setEchoMode(b ? QLineEdit::Normal : QLineEdit::Password); });
    connect(ui->ConfirmVisibilityButton, &QPushButton::toggled, this, [this](bool b){ ui->ConfirmLineEdit->setEchoMode(b ? QLineEdit::Normal : QLineEdit::Password); });

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
    ui->PasswordLineEdit->clear();
    ui->ConfirmLineEdit->clear();

    ui->CreateButton->setText("Create");

    // enable
    ui->BackButton->setEnabled(true);
    ui->FolderBrowseButton->setEnabled(true);
    ui->NameLineEdit->setEnabled(true);
    ui->EncryptionCheckbox->setEnabled(true);
    ui->PasswordLineEdit->setEnabled(true);
    ui->ConfirmLineEdit->setEnabled(true);
    ui->CreateButton->setEnabled(true);

    // state
    ui->EncryptionCheckbox->setChecked(true);
    ui->PasswordVisibilityButton->setChecked(false);
    ui->ConfirmVisibilityButton->setChecked(false);

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
    ui->PasswordLabel->setEnabled(b);
    ui->ConfirmLabel->setEnabled(b);
    ui->PasswordLineEdit->setEnabled(b);
    ui->ConfirmLineEdit->setEnabled(b);
    ui->PasswordVisibilityButton->setEnabled(b);
    ui->ConfirmVisibilityButton->setEnabled(b);

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

void VaultCreateNew::UpdatePasswordCondition()
{
    QString password = ui->PasswordLineEdit->text();
    QString confirm  = ui->ConfirmLineEdit->text();
    isPasswordValid = password.size() >= 4 ? true : false;
    isConfirmValid = password == confirm ? true : false;

    updateStyle(UiOption::Password);
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

        ui->PasswordLabel->setStyleSheet(m_styleWhite);
        ui->ConfirmLabel->setStyleSheet(m_styleWhite);
        ui->PasswordInfoLabel->setStyleSheet(m_styleLightGray);
        ui->ConfirmInfoLabel->setStyleSheet(m_styleRed);

        ui->ConfirmInfoLabel->setMaximumHeight(0);
    }

    if (option & UiOption::Path){
        ui->NameInfoLabel->setStyleSheet(isPathValid ? m_styleLightGray : m_styleRed);
    }

    if (option & UiOption::Password){
        ui->PasswordInfoLabel->setStyleSheet(isPasswordValid ? m_styleLightGray : m_styleRed);
        if (isConfirmValid && ui->ConfirmInfoLabel->sizeHint().height() == ui->ConfirmInfoLabel->size().height()){
            ui->ConfirmInfoLabel->setMaximumHeight(0);
        }else if (!isConfirmValid && ui->ConfirmInfoLabel->sizeHint().height() != ui->ConfirmInfoLabel->size().height()){
            ui->ConfirmInfoLabel->setMaximumHeight(sizeHint().height());
        }
    }

    if (option & UiOption::EncryptionOption){
        bool& b = isAES256EncryptionEnabled;
        ui->EncryptionCheckbox->setStyleSheet(b ? m_styleWhite : m_styleGray);
        ui->PasswordLabel->setStyleSheet(b ? m_styleWhite : m_styleGray);
        ui->ConfirmLabel->setStyleSheet(b ? m_styleWhite : m_styleGray);
        ui->PasswordInfoLabel->setStyleSheet(b ? m_styleWhite : m_styleDarkGray);

        if (!b){
            ui->ConfirmInfoLabel->setMaximumHeight(0);
        }else if (b && !isConfirmValid){
            ui->ConfirmInfoLabel->setMaximumHeight(sizeHint().height());
        }
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
    ui->PasswordLineEdit->setEnabled(false);
    ui->ConfirmLineEdit->setEnabled(false);
    ui->CreateButton->setEnabled(false);
    ui->CreateButton->setText("Creating Vault");

    QString password = ui->PasswordLineEdit->text();
    QString dir  = rootDirectory + "/" + ui->NameLineEdit->text();

    QThread *thread = QThread::create([password, dir](){
        VaultManager& manager = VaultManager::GetInstance();
        manager.CreateVault(dir, password);
    });
    connect(thread, &QThread::finished, this, &VaultCreateNew::Back);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

