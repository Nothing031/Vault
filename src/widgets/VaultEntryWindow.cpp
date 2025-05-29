#include "VaultEntryWindow.hpp"

#include <QCloseEvent>
#include <QObject>
#include <QVector>

#include <QListWidgetItem>
#include <QAction>
#include <QFileDialog>
#include <QTimer>

#pragma comment(lib,"dwmapi.lib")
#pragma comment(lib, "user32.lib")
#include <dwmapi.h>
#include <Windows.h>

#include "src/core/vault/VaultManager.hpp"
#include "VaultViewWindow.hpp"

VaultEntryWindow::VaultEntryWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::VaultEntryWindow),
    vaultTitle(new VaultTitle(this)),
    vaultCreateNew(new VaultCreateNew(this))
{
    ui->setupUi(this);
    resize(900, 550);
    setWindowTitle("Vault");

    HWND hWnd = (HWND)winId();
    // disable maximize button
    LONG style = GetWindowLong(hWnd, GWL_STYLE);
    style &= ~WS_MAXIMIZEBOX;
    style &= ~WS_THICKFRAME; // unresizable
    SetWindowLong(hWnd, GWL_STYLE, style);
    // dark caption
    BOOL USE_DARK_MODE = true;
    DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &USE_DARK_MODE, sizeof(USE_DARK_MODE));
    // redraw
    SetWindowPos(hWnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    ui->stackedWidget->addWidget(vaultTitle);
    ui->stackedWidget->addWidget(vaultCreateNew);
    ui->stackedWidget->setCurrentIndex(0);

    VaultManager& manager = VaultManager::GetInstance();

    connect(vaultTitle, &VaultTitle::createButtonPressed, this, &VaultEntryWindow::StartCreateNewVault);
    connect(vaultTitle, &VaultTitle::openButtonPressed, this, &VaultEntryWindow::OpenFolder);
    connect(vaultCreateNew, &VaultCreateNew::Back, this, [this](){ ui->stackedWidget->setCurrentWidget(vaultTitle); });
    connect(ui->vaultListWidget, &VaultListWidget::requestOpenVault, this, &VaultEntryWindow::OpenVault);

    // load data
    manager.LoadData();
}

VaultEntryWindow::~VaultEntryWindow()
{
    delete ui;
}

void VaultEntryWindow::OpenVault(Vault *vault)
{
    // check opened window
    for (auto& wndVaultPair : std::as_const(childWindows)){
        if (vault == wndVaultPair.second) {
            wndVaultPair.first->raise();
            return;
        }
    }

    qDebug() << "opening vault : " << vault->directory.path();
    VaultViewWindow* window = new VaultViewWindow(vault);
    childWindows.append(QPair<QMainWindow*, Vault*>(window, vault));
    connect(window, &VaultViewWindow::requestShowEntryWindow, this, &VaultEntryWindow::show);
    connect(window, &VaultViewWindow::requestShowEntryWindow, this, &VaultEntryWindow::raise);
    connect(window, &VaultViewWindow::destroyed, this, [this, window](){
        for (int i = 0; i < childWindows.size(); i++){
            if (window == childWindows[i].first){
                childWindows.remove(i, 1);
            }
        }
        if (childWindows.empty() && !this->isVisible()){
            this->show();
            this->close();
        }
    });
    window->show();
    this->hide();
}

void VaultEntryWindow::StartCreateNewVault()
{
    vaultCreateNew->init();
    ui->stackedWidget->setCurrentWidget(vaultCreateNew);
}

void VaultEntryWindow::OpenFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Folder", QDir::rootPath(), QFileDialog::ShowDirsOnly);
    qDebug() << dir;
    if (dir.isEmpty() || !QDir(dir).exists()){
        return;
    }
    VaultManager::GetInstance().CreateVault(false, dir, "");
}

void VaultEntryWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "closing Window";
    if (childWindows.size()){
        qDebug() << "hidnig";
        event->ignore();
        this->hide();
    }
}

void VaultEntryWindow::mousePressEvent(QMouseEvent *event)
{
    m_dragStartPos = event->globalPosition().toPoint() - frameGeometry().topLeft();
}

void VaultEntryWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton){
        move(event->globalPosition().toPoint() - m_dragStartPos);
    }
}
