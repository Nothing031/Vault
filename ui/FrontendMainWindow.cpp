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
    ui->VersionLabel->setText(ENV_VERSION);


    QWidget *emptyWidget = new QWidget(this);
    ui->stackedWidget->addWidget(emptyWidget);

    FrontendNewVault *frontendNewVault = new FrontendNewVault(this);
    ui->stackedWidget->addWidget(frontendNewVault);

    FrontendVault *frontendVault = new FrontendVault(this);
    ui->stackedWidget->addWidget(frontendVault);

    // MAINWINDOW ###############################################################
    connect(ui->OpenVaultComboBox, &QComboBox::currentIndexChanged, this, [this, frontendVault](const int index){
        if (index == -1) return;
        frontendVault->init(Enviroment::GetInstance().getVault(index));
        ui->stackedWidget->setCurrentWidget(frontendVault);
    });
    connect(ui->OpenPortableButton, &QPushButton::pressed, this, [this, frontendVault](){
        QString jsonPath = QFileDialog::getOpenFileName(this, "Select Vault File", "", "JSON File (*.json)");
        if (jsonPath.isEmpty() || !QFile::exists(jsonPath)) return;
        Vault* pVault = Enviroment::GetInstance().LoadPortableVault(jsonPath);
        frontendVault->init(pVault);
        ui->OpenVaultComboBox->setCurrentIndex(-1);
        ui->stackedWidget->setCurrentWidget(frontendVault);
    });
    connect(ui->CreateLocalButton, &QPushButton::pressed, this, [this, frontendNewVault](){
        ui->OpenVaultComboBox->setCurrentIndex(-1);
        frontendNewVault->init(Mode::Local);
        ui->stackedWidget->setCurrentWidget(frontendNewVault);
    });
    connect(ui->CreatePortableButton, &QPushButton::pressed, this, [this, frontendNewVault](){
        ui->OpenVaultComboBox->setCurrentIndex(-1);
        frontendNewVault->init(Mode::Portable);
        ui->stackedWidget->setCurrentWidget(frontendNewVault);
    });

    //# NEW VAULT ###############################################################
    connect(frontendNewVault, &FrontendNewVault::signal_create_vault, this, [this, emptyWidget](const Vault vault){
        if (vault.mode == Mode::Local){
            Enviroment::GetInstance().AddNewVault(vault, ui->OpenVaultComboBox);
        }
        else {
            Enviroment::GetInstance().SavePortableVault(&vault);
        }
        ui->stackedWidget->setCurrentWidget(emptyWidget);
    });

    //# VAULT ####################################################################
    connect(frontendVault, &FrontendVault::request_detachVault, this, [this, emptyWidget](Vault* pVault){
        if (!pVault){
            qDebug() << "Error nullptr";
            return;
        }
        Enviroment::GetInstance().DetachVault(pVault, ui->OpenVaultComboBox);
        ui->stackedWidget->setCurrentWidget(emptyWidget);
    });
    connect(frontendVault, &FrontendVault::request_setEnable_ui, this, [this](const bool b){
        ui->OpenVaultComboBox->setEnabled(b);
        ui->CreateLocalButton->setEnabled(b);
        ui->OpenPortableButton->setEnabled(b);
    });
}

FrontendMainWindow::~FrontendMainWindow()
{
    delete ui;
}
