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


}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_vault_select_comboBox_currentIndexChanged(int index)
{
    ui->vault_createNew_button->blockSignals(true);
    if (ui->stackedWidget->currentWidget() != nullptr){
        ui->stackedWidget->currentWidget()->deleteLater();
    }

    Vault vault;
    vault.dir = QDir("C:/Users/LSH/Documents/TestDirectory");
    vault.display_name = "TestDirectory";
    vault.password = "03ac674216f3e15c761ee1a5e255f067953623c8b388b4459e13f978d7c846f4"; //1234

    Window_crypto* win_crypto = new Window_crypto(this, vault);
    ui->stackedWidget->addWidget(win_crypto);
    ui->stackedWidget->setCurrentWidget(win_crypto);
    ui->vault_createNew_button->blockSignals(false);
}

void MainWindow::on_vault_createNew_button_clicked()
{
    if (ui->stackedWidget->currentWidget() != nullptr){
        ui->stackedWidget->currentWidget()->deleteLater();
    }
    Window_NewVault* win_newVault = new Window_NewVault(this, NewVault::CreateNew);
    ui->stackedWidget->addWidget(win_newVault);
    ui->stackedWidget->setCurrentWidget(win_newVault);
    connect(win_newVault, &Window_NewVault::signal_create_new_vault, this, [this](const Vault vault){
       // create vault;

    });
}


void MainWindow::on_vault_createExisting_button_clicked()
{
    if (ui->stackedWidget->currentWidget() != nullptr){
        ui->stackedWidget->currentWidget()->deleteLater();
    }
    Window_NewVault* win_newVault = new Window_NewVault(this, NewVault::CreateExisting);
    ui->stackedWidget->addWidget(win_newVault);
    ui->stackedWidget->setCurrentWidget(win_newVault);
    connect(win_newVault, &Window_NewVault::signal_create_new_vault, this, [this](const Vault vault){
        // create vault;

    });
}

