#include "VaultEntryWindow.hpp"

#include <QCloseEvent>
#include <QObject>
#include <QVector>

#include <QListWidgetItem>
#include <QAction>
#include <QFileDialog>
#include "src/core/vault/VaultManager.hpp"
#include "VaultViewWindow.hpp"

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

    connect(vaultTitle, &VaultTitle::createButtonPressed, this, &VaultEntryWindow::StartCreateNewVault);
    connect(vaultTitle, &VaultTitle::openButtonPressed, this, &VaultEntryWindow::OpenFolder);

    connect(vaultCreateNew, &VaultCreateNew::Back, this, [this](){ ui->stackedWidget->setCurrentWidget(vaultTitle); });

    connect(qobject_cast<VaultListWidget*>(ui->vaultListWidget), &VaultListWidget::requestOpenVault, this, &VaultEntryWindow::OpenVault);


    // load data
    manager.LoadData();
}

VaultEntryWindow::~VaultEntryWindow()
{
    delete ui;
}

void VaultEntryWindow::OpenVault(Vault *vault)
{
    qDebug() << "opening vault : " << vault->directory.path();
    QMainWindow* window = new VaultViewWindow(vault);
    childWindows.append(window);
    connect(window, &QMainWindow::destroyed, this, [this, window](){
        for (int i = 0; i < childWindows.size(); i++){
            if (window == childWindows[i]){
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
