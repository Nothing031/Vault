#include "VaultEntryWindow.hpp"

#include <QCloseEvent>
#include <QObject>
#include <QVector>
#include <dwmapi.h>

#include <QListWidgetItem>

#include <QFileDialog>
#include "VaultInfoButton.hpp"
#include "src/core/vault/VaultManager.hpp"

#pragma comment(lib, "dwmapi")
#pragma comment(lib, "User32.lib")


VaultEntryWindow::VaultEntryWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::VaultEntryWindow),
    vaultTitle(new VaultTitle(this)),
    vaultCreateNew(new VaultCreateNew(this))
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    this->resize(900, 600);

    VaultManager& manager = VaultManager::GetInstance();


    ui->stackedWidget->addWidget(vaultTitle);
    ui->stackedWidget->addWidget(vaultCreateNew);
    ui->stackedWidget->setCurrentIndex(0);

    connect(ui->CloseButton, &QPushButton::clicked, this, &VaultEntryWindow::close);

    connect(vaultTitle, &VaultTitle::createButtonPressed, this, &VaultEntryWindow::CreateNewVault);
    connect(vaultTitle, &VaultTitle::openButtonPressed, this, &VaultEntryWindow::OpenFolder);

    connect(vaultCreateNew, &VaultCreateNew::Back, this, [this](){ ui->stackedWidget->setCurrentWidget(vaultTitle); });

    connect(&manager, &VaultManager::onAttachVault, this, &VaultEntryWindow::AddVault);
    connect(&manager, &VaultManager::onDetachVault, this, &VaultEntryWindow::RemoveVault);



    // load data
    manager.LoadData();
}

VaultEntryWindow::~VaultEntryWindow()
{
    delete ui;
}

void VaultEntryWindow::AddVault(Vault *vault)
{
    if (!vault) return;
    QListWidgetItem* item = new QListWidgetItem(ui->VaultListWidget);
    VaultInfoButton* button = new VaultInfoButton(vault, ui->VaultListWidget);
    item->setSizeHint(button->sizeHint());
    ui->VaultListWidget->setItemWidget(item, button);

    connect(button, &QPushButton::clicked, this, [button, this](){
        OpenVault(button->getVault());
    });
}

void VaultEntryWindow::RemoveVault(int index)
{
    QListWidgetItem* target = ui->VaultListWidget->item(index);
    ui->VaultListWidget->removeItemWidget(target);
}

void VaultEntryWindow::OpenVault(Vault *vault)
{
    qDebug() << "opening vault";
}

void VaultEntryWindow::CreateNewVault()
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
}

void VaultEntryWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "closing Window";
    if (childWindows.size()){
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
