#include "VaultEntryWindow.hpp"

#include <QCloseEvent>
#include <QObject>
#include <QVector>
#include <dwmapi.h>

#include <QListWidgetItem>

#include "VaultInfoButton.hpp"

#pragma comment(lib, "dwmapi")
#pragma comment(lib, "User32.lib")

static Vault vault = {};
static Vault vault2 = {};

VaultEntryWindow::VaultEntryWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::VaultEntryWindow),
    vaultTitle(new VaultTitle(this)),
    vaultCreateNew(new VaultCreateNew(this))
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    this->resize(900, 600);

    ui->stackedWidget->addWidget(vaultTitle);
    ui->stackedWidget->addWidget(vaultCreateNew);
    ui->stackedWidget->setCurrentIndex(0);


    vault.directory = QDir("C:\\Users\\Nothing\\Documents\\PassWord\\.obsidian");
    vault2.directory = QDir("C:\\Users\\Nothing\\source\\repos\\Vault\\build\\Desktop_Qt_6_9_0_MSVC2022_64bit-Debug\\.qtc_clangd\\.cache\\clangd\\index");
    AddVault(&vault);
    AddVault(&vault2);


    connect(ui->CloseButton, &QPushButton::clicked, this, &VaultEntryWindow::close);
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
    qDebug() << "try opening vault";
    qDebug() << vault->directory;
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
