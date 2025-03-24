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

#include "src/json.hpp"
#include "src/enviroment.hpp"

namespace fs = std::filesystem;

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Load getVaults
    auto vaults = Enviroment::GetInstance().getVaults();
    ui->vault_select_comboBox->blockSignals(true);
    for (auto& vault : std::as_const(vaults)){
        ui->vault_select_comboBox->addItem(vault.displayName);
    }
    ui->vault_select_comboBox->setCurrentIndex(-1);
    ui->vault_select_comboBox->blockSignals(false);


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
        ui->outputTerminal_textEdit->append("Vault Created");
        ui->outputTerminal_textEdit->append("  " + vault.dir.path());
    });
    connect(win_newvault, &window_newvault::request_terminal_message, this, [this](const QStringList messages){
        for (auto& message : std::as_const(messages)){
            ui->outputTerminal_textEdit->append(message);
        }
    });


    //# window crypto####################################################################
    window_crypto *win_crypto = new window_crypto(this);
    ui->stackedWidget->addWidget(win_crypto);
    connect(ui->vault_select_comboBox, &QComboBox::currentIndexChanged, win_crypto, [this, win_crypto](const int index){
        if (index == -1){
            return;
        }

        ui->stackedWidget->setCurrentWidget(win_crypto);
        win_crypto->on_request_page(index, vaults.at(index));
    });
    connect(win_crypto, &window_crypto::request_detachVault, this, [this](const int index){
        vaults.erase(vaults.begin() + index);
        ui->vault_select_comboBox->blockSignals(true);
        ui->vault_select_comboBox->clear();
        for (const Vault& vault : vaults){
            ui->vault_select_comboBox->addItem(vault.display_name);
        }
        ui->vault_select_comboBox->setCurrentIndex(-1);
        ui->vault_select_comboBox->blockSignals(false);
        ui->stackedWidget->setCurrentIndex(0);
        Json().SaveVaultJson(vaults);
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

void MainWindow::on_vault_select_comboBox_currentIndexChanged(int index)
{

}

void MainWindow::on_vault_createNew_button_clicked()
{

}

void MainWindow::on_vault_createExisting_button_clicked()
{

}

