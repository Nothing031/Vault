#include "mainwindow.h"
#include "window_crypto.h"
#include "ui_mainwindow.h"

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

#include "window_crypto.h"
#include "window_newvault.h"

#include "src/enviroment.hpp"


MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /////////////
    Enviroment::GetInstance().Init(ui->vault_select_comboBox);

    //# window new vault ###############################################################
    window_newvault *win_newvault = new window_newvault(this);
    ui->stackedWidget->addWidget(win_newvault);
    connect(ui->vault_createNew_button, &QPushButton::pressed, win_newvault, [this, win_newvault](){
        ui->vault_select_comboBox->setCurrentIndex(-1);
        win_newvault->on_request_page(NewVault::CreateNew);
        ui->stackedWidget->setCurrentWidget(win_newvault);
    });
    connect(ui->vault_createExisting_button, &QPushButton::pressed, win_newvault, [this, win_newvault](){
        ui->vault_select_comboBox->setCurrentIndex(-1);
        win_newvault->on_request_page(NewVault::CreateExisting);
        ui->stackedWidget->setCurrentWidget(win_newvault);
    });
    connect(win_newvault, &window_newvault::signal_create_new_vault, this, [this](const Vault vault){
        Enviroment::GetInstance().AddNewVault(vault, ui->vault_select_comboBox);
        ui->stackedWidget->setCurrentIndex((int)page::Empty);
    });


    //# window crypto####################################################################
    window_crypto *win_crypto = new window_crypto(this);
    ui->stackedWidget->addWidget(win_crypto);
    connect(ui->vault_select_comboBox, &QComboBox::currentIndexChanged, win_crypto, [this, win_crypto](const int index){
        if (index == -1) return;
        win_crypto->on_request_page(Enviroment::GetInstance().getVault(index));
        ui->stackedWidget->setCurrentWidget(win_crypto);
    });
    connect(win_crypto, &window_crypto::request_detachVault, this, [this](Vault* pVault){
        if (!pVault){
            qDebug() << "Error nullptr";
            return;
        }
        Enviroment::GetInstance().DetachVault(pVault, ui->vault_select_comboBox);
        ui->stackedWidget->setCurrentIndex(0);
    });
    connect(win_crypto, &window_crypto::request_setEnable_ui, this, [this](const bool b){
        ui->vault_select_comboBox->setEnabled(b);
        ui->vault_createExisting_button->setEnabled(b);
        ui->vault_createNew_button->setEnabled(b);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
