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

#include "src/json.h"

namespace fs = std::filesystem;

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    vaults = Json().LoadVaultJson();

    ui->vault_select_comboBox->blockSignals(true);
    for (const Vault& vault : vaults){
        ui->vault_select_comboBox->addItem(vault.display_name);
    }
    ui->vault_select_comboBox->setCurrentIndex(-1);
    ui->vault_select_comboBox->blockSignals(false);
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_vault_select_comboBox_currentIndexChanged(int index)
{
    if (index == -1){
        return;
    }

    ui->vault_createNew_button->blockSignals(true);
    if (ui->stackedWidget->currentIndex() != 0 && ui->stackedWidget->currentWidget() != nullptr){
        ui->stackedWidget->currentWidget()->deleteLater();
    }

    Window_crypto* win_crypto = new Window_crypto(this, vaults[index], index);
    connect(win_crypto, &Window_crypto::request_detachVault_, this, [this](const int persistence_index){
        // delete index;

        vaults.erase(vaults.begin() + persistence_index);

        ui->vault_select_comboBox->blockSignals(true);
        ui->vault_select_comboBox->clear();
        for (const Vault& vault : vaults){
            ui->vault_select_comboBox->addItem(vault.display_name);
        }
        ui->vault_select_comboBox->setCurrentIndex(-1);
        ui->vault_select_comboBox->blockSignals(false);
        Json().SaveVaultJson(vaults);
        ui->stackedWidget->setCurrentIndex(0);
        qDebug() << "Vault Created";
    });
    connect(win_crypto, &Window_crypto::request_setEnable_ui, this, [this](const bool b){
        ui->vault_select_comboBox->setEnabled(b);
        ui->vault_createExisting_button->setEnabled(b);
        ui->vault_createNew_button->setEnabled(b);
    });

    ui->stackedWidget->addWidget(win_crypto);
    ui->stackedWidget->setCurrentWidget(win_crypto);
    ui->vault_createNew_button->blockSignals(false);
}

void MainWindow::on_vault_createNew_button_clicked()
{
    if (ui->stackedWidget->currentIndex() != 0 && ui->stackedWidget->currentWidget() != nullptr){
        ui->stackedWidget->currentWidget()->deleteLater();
    }
    Window_NewVault* win_newVault = new Window_NewVault(this, NewVault::CreateNew);
    ui->stackedWidget->addWidget(win_newVault);
    ui->stackedWidget->setCurrentWidget(win_newVault);
    connect(win_newVault, &Window_NewVault::signal_create_new_vault, this, [this](const Vault vault){
       // create vault;
        vaults.push_back(vault);
        ui->vault_select_comboBox->blockSignals(true);
        for (const Vault& vault : vaults){
            ui->vault_select_comboBox->addItem(vault.display_name);
        }
        ui->vault_select_comboBox->setCurrentIndex(-1);
        ui->vault_select_comboBox->blockSignals(false);
        Json().SaveVaultJson(vaults);
        ui->stackedWidget->setCurrentIndex(0);
        qDebug() << "Vault Created";
    });
}

void MainWindow::on_vault_createExisting_button_clicked()
{
    if (ui->stackedWidget->currentIndex() != 0 && ui->stackedWidget->currentWidget() != nullptr){
        ui->stackedWidget->currentWidget()->deleteLater();
    }
    Window_NewVault* win_newVault = new Window_NewVault(this, NewVault::CreateExisting);
    ui->stackedWidget->addWidget(win_newVault);
    ui->stackedWidget->setCurrentWidget(win_newVault);
    connect(win_newVault, &Window_NewVault::signal_create_new_vault, this, [this](const Vault vault){
        // create vault;
        vaults.push_back(vault);
        ui->vault_select_comboBox->blockSignals(true);
        Vault loopVault;
        for (const Vault& vault : vaults){
            ui->vault_select_comboBox->addItem(vault.display_name);
        }
        ui->vault_select_comboBox->setCurrentIndex(-1);
        ui->vault_select_comboBox->blockSignals(false);
        Json().SaveVaultJson(vaults);
        qDebug() << "Vault Created";
    });
}

