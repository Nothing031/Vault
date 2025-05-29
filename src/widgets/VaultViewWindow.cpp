#include "VaultViewWindow.hpp"

#include "ui_VaultViewWindow.h"

#pragma comment(lib,"dwmapi.lib")
#include <dwmapi.h>
#include <Windows.h>

#include <QIcon>
#include <QUrl>
#include <QThread>
#include <QMouseEvent>
#include <QDesktopServices>
#include <QDialog>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QQueue>

#include <src/core/vault/Vault.hpp>
#include <src/models/FileListModel.hpp>
#include "src/widgets/FileListView.hpp"
#include "src/core/vault/VaultManager.hpp"


VaultViewWindow::VaultViewWindow(Vault *pVault)
    : ui(new Ui::VaultViewWindow),
    vault(pVault)
{
    vault->owner = this;

    ui->setupUi(this);
    // dark caption
    HWND hWnd = (HWND)this->winId();
    BOOL USE_DARK_MODE = true;
    DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &USE_DARK_MODE, sizeof(USE_DARK_MODE));
    // flag
    setAttribute(Qt::WA_DeleteOnClose);

    // widget
    ui->fileListView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->fileListView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->fileListView->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);

    // init
    setWindowTitle("Vault - " + QUrl(vault->directory.path()).fileName());
    ui->titleButton->setText(QUrl(vault->directory.path()).fileName());
    ui->subtitleLabel->setText(vault->directory.path());
    ui->terminalTextBrowser->clear();

    vault->aes.Lock();
    ui->unlockButton->setEnabled(vault->aes.IsEnabled());
    ui->encryptButton->setEnabled(false);
    ui->decryptButton->setEnabled(false);
    ui->suspendButton->setEnabled(false);
    ui->progressBar->setValue(0);


    // connect
    connect(ui->fileListView, &FileListView::onSelectionChange, this, &VaultViewWindow::UpdateButton);
    connect(ui->titleButton, &QPushButton::clicked, this, &VaultViewWindow::requestShowEntryWindow);
    connect(ui->openPathButton, &QPushButton::clicked, this, [this](){ QDesktopServices::openUrl(QUrl(vault->directory.path())); });
    connect(ui->settingsButton, &QPushButton::clicked, this, &VaultViewWindow::OpenSettings);
    connect(ui->unlockButton, &QPushButton::clicked, this, &VaultViewWindow::TryUnlock);
    connect(ui->encryptButton, &QPushButton::clicked, this, &VaultViewWindow::Encrypt);
    connect(ui->decryptButton, &QPushButton::clicked, this, &VaultViewWindow::Decrypt);


    // load files
    QThread* thread = QThread::create([this](){
        vault->LoadFiles();
    });
    connect(thread, &QThread::finished, this, [this](){
        ui->fileListView->SetModelVault(vault);
        ui->terminalTextBrowser->clear();
    });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    ui->terminalTextBrowser->append("Loading files...");
    thread->start();
}

VaultViewWindow::~VaultViewWindow()
{
    vault->aes.Lock();
    vault->owner = &VaultManager::GetInstance();
    delete ui;
}

void VaultViewWindow::UpdateButton(int plains, int ciphers)
{
    if (plains == 0 && ciphers == 0){
        ui->encryptButton->setText("Encrypt vault");
        ui->decryptButton->setText("Decrypt vault");
        // ui->encryptButton->setEnabled(true);
        // ui->decryptButton->setEnabled(true);
        return;
    }

    if (plains + ciphers == 1){
        ui->encryptButton->setText("Encrypt file");
        ui->decryptButton->setText("Decrypt file");
    }else{
        QString numStr = QString::number(plains + ciphers);
        ui->encryptButton->setText("Encrypt selected files");
        ui->decryptButton->setText("Decrypt selected files");
    }


    // if (plains){
    //     ui->encryptButton->setEnabled(true);
    //     if (plains == 1)
    //         ui->encryptButton->setText("Encrypt file");
    //     else
    //         ui->encryptButton->setText("Encrypt " + QString::number(plains) + " files");
    // }else{
    //     ui->encryptButton->setEnabled(false);
    //     ui->encryptButton->setText("Encrypt");
    // }

    // if (ciphers){
    //     ui->decryptButton->setEnabled(true);
    //     if (ciphers == 1)
    //         ui->decryptButton->setText("Decrypt file");
    //     else
    //         ui->decryptButton->setText("Decrypt " + QString::number(ciphers) + " files");
    // }else{
    //     ui->decryptButton->setEnabled(false);
    //     ui->decryptButton->setText("Decrypt");
    // }
}

void VaultViewWindow::OpenSettings()
{

}

void VaultViewWindow::TryUnlock()
{
    QDialog dialog(this);
    HWND hWnd = (HWND)dialog.winId();
    // disable maximize button
    LONG style = GetWindowLong(hWnd, GWL_STYLE);
    //style &= ~WS_SYSMENU; // remove sys menu;
    SetWindowLong(hWnd, GWL_STYLE, style);
    // dark caption
    BOOL USE_DARK_MODE = true;
    DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &USE_DARK_MODE, sizeof(USE_DARK_MODE));
    // redraw
    SetWindowPos(hWnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    dialog.setModal(true);

    dialog.setFocusPolicy(Qt::NoFocus);
    dialog.setStyleSheet(R"(
QWidget{background: rgb(30, 30, 30);color: rgb(244, 244, 244);font: 12pt "Arial";}
QLabel{max-width: 100px;min-width: 100px;}
QLineEdit{font: 10pt;border: 1px solid rgb(70, 70, 70);border-radius: 4px;min-height: 24px;max-height: 24px; min-width: 320px;}
QPushButton{font: 11pt;max-height: 24px;min-height: 24px;max-width: 24px;min-width: 24px;background: rgb(40, 40, 40);border: 1px solid rgb(70, 70, 70);border-radius: 4px;}
QPushButton:hover{background: rgb(60, 60, 60);}
QPushButton::pressed{background: rgb(40, 40, 40);}
)");
    QLabel* label = new QLabel("Password" ,&dialog);

    QPushButton* button = new QPushButton(&dialog);
    QIcon icon;
    icon.addFile(":/icons/visibility_off.png", QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    icon.addFile(":/icons/visibility_on.png", QSize(), QIcon::Mode::Normal, QIcon::State::On);
    button->setIcon(icon);
    button->setCheckable(true);
    button->setAutoDefault(false);
    button->setDefault(false);

    QLineEdit* lineEdit = new QLineEdit(&dialog);
    lineEdit->setEchoMode(QLineEdit::Password);
    QRegularExpression passwordReg(R"(^[A-Za-z0-9!@#$%^&*()_+\-=\[\]{};':\"\\|,.<>\\/?]*$)");
    QRegularExpressionValidator *passwordValidator = new QRegularExpressionValidator(passwordReg, this);
    lineEdit->setValidator(passwordValidator);
    lineEdit->setMaxLength(32);

    QLabel* infoLabel = new QLabel(&dialog);
    infoLabel->setStyleSheet("QLabel{color: rgb(200, 200, 200); font: 9pt; max-width: 10000px;min-width: 0px; max-height: 12px}");

    connect(button, &QPushButton::toggled, lineEdit, [lineEdit](bool b){
       lineEdit->setEchoMode(b ? QLineEdit::Normal : QLineEdit::Password);
    });
    connect(lineEdit, &QLineEdit::returnPressed, [this, &dialog, infoLabel, lineEdit](){
        infoLabel->clear();
        QApplication::processEvents();
        bool result = vault->aes.TryUnlock(lineEdit->text());
        if (result){
            dialog.close();
            // TODO: unlock vault
            ui->unlockButton->setEnabled(false);
            ui->encryptButton->setEnabled(true);
            ui->decryptButton->setEnabled(true);
        }else{
            infoLabel->setText("wrong password. please try again");
        }
    });

    QVBoxLayout* vBoxLayout = new QVBoxLayout();
    QHBoxLayout* hBoxLayout = new QHBoxLayout();
    hBoxLayout->addWidget(label);
    hBoxLayout->addWidget(lineEdit);
    hBoxLayout->addWidget(button);
    vBoxLayout->addLayout(hBoxLayout);
    vBoxLayout->addWidget(infoLabel);

    dialog.setWindowTitle("");
    dialog.setLayout(vBoxLayout);
    dialog.adjustSize();
    dialog.setFixedSize(dialog.size());
    dialog.exec();
}

void VaultViewWindow::Encrypt()
{
    CryptoEngine* engine = new CryptoEngine();
    QVector<FileInfo*> files = ui->fileListView->GetSelectedFiles();
    QThread* thread = QThread::create([engine, files, this](){
        QQueue<FileInfo*> targets;
        if (files.size()){
            for (auto& file : files){
                if (file->state == FileInfo::PLAIN_GOOD)
                    targets.append(file);
            }
        }else{ // entire vault
            for (auto& file : vault->files){
                if (file->state == FileInfo::PLAIN_GOOD)
                    targets.append(file);
            }
        }
        engine->AES256EncryptFiles(targets, &vault->mutex, vault->aes);
    });
    connect(ui->suspendButton, &QPushButton::clicked, engine, &CryptoEngine::SuspendProcess);
    connect(engine, &CryptoEngine::onEvent, this, &VaultViewWindow::ProcessCryptoEngineMessage);
    connect(thread, &QThread::finished, engine, &CryptoEngine::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

void VaultViewWindow::Decrypt()
{
    QVector<FileInfo*> files = ui->fileListView->GetSelectedFiles();
    CryptoEngine* engine = new CryptoEngine();
    QThread* thread = QThread::create([engine, files, this](){
        QQueue<FileInfo*> targets;
        if (files.size()){
            for (auto& file : files){
                if (file->state == FileInfo::CIPHER_GOOD)
                    targets.append(file);
            }
        }else{ // entire vault
            for (auto& file : vault->files){
                if (file->state == FileInfo::CIPHER_GOOD)
                    targets.append(file);
            }
        }
        engine->AES256DecryptFiles(targets, &vault->mutex, vault->aes);
    });
    connect(ui->suspendButton, &QPushButton::clicked, engine, &CryptoEngine::SuspendProcess);
    connect(engine, &CryptoEngine::onEvent, this, &VaultViewWindow::ProcessCryptoEngineMessage);
    connect(thread, &QThread::finished, engine, &CryptoEngine::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

void VaultViewWindow::ProcessCryptoEngineMessage(CryptoEngine::Event event, QVariant param)
{
    using cEvent = CryptoEngine::Event;
    switch (event){
    case cEvent::START:
        // set ui
        ui->suspendButton->setEnabled(true);
        ui->encryptButton->setEnabled(false);
        ui->decryptButton->setEnabled(false);
        ui->settingsButton->setEnabled(false);
        ui->fileListView->setEnabled(false);
        break;
    case cEvent::END:
        // set ui
        ui->suspendButton->setEnabled(false);
        ui->settingsButton->setEnabled(true);
        ui->encryptButton->setEnabled(true);
        ui->decryptButton->setEnabled(true);
        ui->fileListView->setEnabled(true);
        ui->fileListView->update();
        break;
    case cEvent::PROGRESS_CURRENT:
        ui->progressBar->setValue(param.toInt());
        break;
    case cEvent::PROGRESS_MAX:
        ui->progressBar->setMaximum(param.toInt());
        break;
    case cEvent::MESSAGE_SINGLE:
        ui->terminalTextBrowser->append(param.toString());
        break;
    case cEvent::MESSAGE_LIST:
        for (auto& msg : param.toStringList()){
            ui->terminalTextBrowser->append(msg);
        }
        break;
    case cEvent::CLEAR_TERMINAL:
        ui->terminalTextBrowser->clear();
        break;
    }
}

void VaultViewWindow::mousePressEvent(QMouseEvent *event)
{
    QWidget *clicked = childAt(event->pos());
    if (!ui->fileListView->isAncestorOf(clicked)) {
        ui->fileListView->clearSelection();
        ui->fileListView->clearFocus();
        ui->encryptButton->setText("Encrypt all");
        ui->decryptButton->setText("Decrypt all");
    }
    QMainWindow::mousePressEvent(event);
}




