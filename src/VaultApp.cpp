#include "VaultApp.hpp"
#include "ui_VaultApp.h"

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

#include "src/VaultApp_Viewer.hpp"

#include "src/core/vault/VaultManager.hpp"
#include "src/widgets/VaultTitle.hpp"
#include "src/widgets/VaultCreateNew.hpp"

VaultApp::VaultApp(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::VaultApp),
    vaultTitleWidget(new VaultTitle(this)),
    vaultCreateNewWidget(new VaultCreateNew(this))
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

    ui->stackedWidget->addWidget(vaultTitleWidget);
    ui->stackedWidget->addWidget(vaultCreateNewWidget);
    ui->stackedWidget->setCurrentIndex(0);

    // manager
    VaultManager& manager = VaultManager::GetInstance();
    connect(&manager, &VaultManager::onVaultAdded, ui->vaultListWidget, &VaultListWidget::addVaultButton);
    connect(&manager, &VaultManager::onVaultRemoved, ui->vaultListWidget, &VaultListWidget::removeVaultButton);


    connect(vaultTitleWidget, &VaultTitle::createButtonPressed, this, &VaultApp::StartCreateNewVault);
    connect(vaultTitleWidget, &VaultTitle::openButtonPressed, this, &VaultApp::OpenFolder);
    connect(vaultCreateNewWidget, &VaultCreateNew::Back, this, [this](){ ui->stackedWidget->setCurrentWidget(vaultTitleWidget); });
    connect(ui->vaultListWidget, &VaultListWidget::requestOpenVault, this, &VaultApp::OpenVault);
    connect(ui->vaultListWidget, &VaultListWidget::requestDetachVault, &manager, &VaultManager::DetachVault);

    // load data
    manager.LoadData();
}

VaultApp::~VaultApp()
{
    delete ui;
}

void VaultApp::OpenVault(std::shared_ptr<Vault> vault)
{
    // check opened window
    for (auto& wndVaultPair : std::as_const(childWindows)){
        if (vault == wndVaultPair.second) {
            wndVaultPair.first->raise();
            return;
        }
    }

    // open
    VaultApp_Viewer* window = new VaultApp_Viewer(vault);
    childWindows.append(QPair<QMainWindow*, std::shared_ptr<Vault>>(window, vault));
    connect(window, &VaultApp_Viewer::requestShowEntryWindow, this, &VaultApp::show);
    connect(window, &VaultApp_Viewer::requestShowEntryWindow, this, &VaultApp::raise);
    connect(window, &VaultApp_Viewer::destroyed, this, [this, window](){
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

void VaultApp::StartCreateNewVault()
{
    vaultCreateNewWidget->init();
    ui->stackedWidget->setCurrentWidget(vaultCreateNewWidget);
}

void VaultApp::OpenFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Folder", QDir::rootPath(), QFileDialog::ShowDirsOnly);
    qDebug() << dir;
    if (dir.isEmpty() || !QDir(dir).exists()){
        return;
    }
    VaultManager::GetInstance().CreateVault(false, dir, "");
}

void VaultApp::closeEvent(QCloseEvent *event)
{
    qDebug() << "closing Window";
    if (childWindows.size()){
        qDebug() << "hidnig";
        event->ignore();
        this->hide();
    }
}

void VaultApp::mousePressEvent(QMouseEvent *event)
{
    m_dragStartPos = event->globalPosition().toPoint() - frameGeometry().topLeft();
}

void VaultApp::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton){
        move(event->globalPosition().toPoint() - m_dragStartPos);
    }
}
