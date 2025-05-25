#include "VaultViewWindow.hpp"

#include "ui_VaultViewWindow.h"

#pragma comment(lib,"dwmapi.lib")
#include <dwmapi.h>
#include <Windows.h>

#include <QUrl>
#include <QThread>
#include <QMouseEvent>

#include <src/core/vault/Vault.hpp>
#include <src/models/FileListModel.hpp>


VaultViewWindow::VaultViewWindow(Vault *pVault)
    : ui(new Ui::VaultViewWindow),
    fileListModel(new FileListModel(this)),
    vault(pVault)
{
    ui->setupUi(this);
    // dark caption
    HWND hWnd = (HWND)this->winId();
    BOOL USE_DARK_MODE = true;
    DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &USE_DARK_MODE, sizeof(USE_DARK_MODE));
    // flag
    setAttribute(Qt::WA_DeleteOnClose);

    // widget
    ui->FileListView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->FileListView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->FileListView->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);

    // init
    setWindowTitle("Vault - " + QUrl(vault->directory.path()).fileName());
    ui->TitleLabel->setText(QUrl(vault->directory.path()).fileName());
    ui->SubtitleLabel->setText(vault->directory.path());
    ui->TerminalTextBrowser->clear();

    vault->aesSettings.Lock();
    ui->UnlockButton->setEnabled(vault->aesSettings.IsEnabled());
    ui->EncryptButton->setEnabled(false);
    ui->DecryptButton->setEnabled(false);
    ui->SuspendButton->setEnabled(false);
    ui->ProgressBar->setValue(0);

    ui->FileListView->setModel(fileListModel);

    // load files
    QThread* thread = QThread::create([this](){
        vault->LoadFiles();
    });
    connect(thread, &QThread::finished, this, [this](){
        fileListModel->setVault(vault);
        ui->TerminalTextBrowser->clear();
    });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    ui->TerminalTextBrowser->append("Loading files...");
    thread->start();
}

VaultViewWindow::~VaultViewWindow()
{
    delete ui;
}

void VaultViewWindow::mousePressEvent(QMouseEvent *event)
{
    QWidget *clicked = childAt(event->pos());
    if (!ui->FileListView->isAncestorOf(clicked)) {
        ui->FileListView->clearSelection();
        ui->FileListView->clearFocus();
    }
    QMainWindow::mousePressEvent(event);
}




