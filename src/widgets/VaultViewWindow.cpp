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
#include <src/core/vault/Vault.hpp>
#include <src/models/FileListModel.hpp>
#include "src/widgets/FileListView.hpp"
#include "src/core/vault/VaultManager.hpp"


VaultViewWindow::VaultViewWindow(Vault *pVault)
    : ui(new Ui::VaultViewWindow),
    fileListModel(new FileListModel(this)),
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
    ui->fileListView->setModel(fileListModel);
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
    connect(ui->fileListView, &FileListView::onSelectionChange, this, [this](FileListView::SelectionMode mode, int count){
        QString countStr = QString::number(count);
        switch (mode){
            case FileListView::Single :
                ui->encryptButton->setText("Encrypt");
                ui->decryptButton->setText("Decrypt");
                break;
            case FileListView::Multi :
                ui->encryptButton->setText("Encrypt " + countStr + " files");
                ui->decryptButton->setText("Decrypt " + countStr + " files");
                break;
            case FileListView::None:
                ui->encryptButton->setText("Encrypt all");
                ui->decryptButton->setText("Decrypt all");
                break;
        }
    });

    connect(ui->titleButton, &QPushButton::clicked, this, &VaultViewWindow::requestShowEntryWindow);


    connect(ui->openPathButton, &QPushButton::clicked, this, [this](){ QDesktopServices::openUrl(QUrl(vault->directory.path())); });
    connect(ui->settingsButton, &QPushButton::clicked, this, &VaultViewWindow::OpenSettings);
    connect(ui->unlockButton, &QPushButton::clicked, this, &VaultViewWindow::TryUnlock);


    // load files
    QThread* thread = QThread::create([this](){
        vault->LoadFiles();
    });
    connect(thread, &QThread::finished, this, [this](){
        fileListModel->setVault(vault);
        ui->terminalTextBrowser->clear();
    });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    ui->terminalTextBrowser->append("Loading files...");
    thread->start();
}

VaultViewWindow::~VaultViewWindow()
{
    vault->owner = &VaultManager::GetInstance();
    delete ui;
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

void VaultViewWindow::Decrypt()
{

}

void VaultViewWindow::Encrypt()
{

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




