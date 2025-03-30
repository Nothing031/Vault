#include "FrontendMainWindow.h"
#include "FrontendVault.h"
#include "ui_FrontendMainWindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QToolTip>
#include <QPoint>
#include <QDesktopServices>
#include <QUrl>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFileSystemWatcher>
#include <QDir>
#include <QDirIterator>
#include <QTimer>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QListView>
#include <QThread>
#include <QFileDialog>

#include "FrontendVault.h"
#include "FrontendNewVault.h"

#include "src/enviroment.hpp"


FrontendMainWindow::FrontendMainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::FrontendMainWindow)
{
    ui->setupUi(this);

    /////////////
    Enviroment::GetInstance().Init(ui->OpenVaultComboBox);

    //# window new vault ###############################################################
    FrontendNewVault *win_newvault = new FrontendNewVault(this);
    ui->stackedWidget->addWidget(win_newvault);
    connect(ui->OpenPortableButton, &QPushButton::pressed, win_newvault, [this, win_newvault](){
        ui->OpenVaultComboBox->setCurrentIndex(-1);
        win_newvault->on_request_page(Mode::Portable);
        ui->stackedWidget->setCurrentWidget(win_newvault);
    });
    connect(ui->CreateLocalButton, &QPushButton::pressed, win_newvault, [this, win_newvault](){
        ui->OpenVaultComboBox->setCurrentIndex(-1);
        win_newvault->on_request_page(Mode::Local);
        ui->stackedWidget->setCurrentWidget(win_newvault);
    });
    connect(ui->OpenPortableButton, &QPushButton::pressed, win_newvault, [this, win_newvault](){
        QString vaultJson = QFileDialog::getExistingDirectory(this, "Select Folder", QDir::rootPath(), QFileDialog::ShowDirsOnly);
        if (dir.isEmpty()) return;
        directory = QDir(dir);



    });

    connect(win_newvault, &FrontendNewVault::signal_create_local_vault, this, [this](const Vault vault){
        Enviroment::GetInstance().AddNewVault(vault, ui->OpenVaultComboBox);
        ui->stackedWidget->setCurrentIndex((int)page::Empty);
    });
    connect(win_newvault, &FrontendNewVault::signal_create_portable_vault, this, [this](const Vault vault){
        // idk


    });


    //# window crypto####################################################################
    FrontendVault *win_crypto = new FrontendVault(this);
    ui->stackedWidget->addWidget(win_crypto);
    connect(ui->OpenVaultComboBox, &QComboBox::currentIndexChanged, win_crypto, [this, win_crypto](const int index){
        if (index == -1) return;
        win_crypto->on_request_page(Enviroment::GetInstance().getVault(index));
        ui->stackedWidget->setCurrentWidget(win_crypto);
    });
    connect(win_crypto, &FrontendVault::request_detachVault, this, [this](Vault* pVault){
        if (!pVault){
            qDebug() << "Error nullptr";
            return;
        }
        Enviroment::GetInstance().DetachVault(pVault, ui->OpenVaultComboBox);
        ui->stackedWidget->setCurrentIndex(0);
    });
    connect(win_crypto, &FrontendVault::request_setEnable_ui, this, [this](const bool b){
        ui->OpenVaultComboBox->setEnabled(b);
        ui->CreateLocalButton->setEnabled(b);
        ui->OpenPortableButton->setEnabled(b);
    });
}

FrontendMainWindow::~FrontendMainWindow()
{
    delete ui;
}
