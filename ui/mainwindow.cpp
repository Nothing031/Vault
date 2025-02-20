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


namespace fs = std::filesystem;

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , window_crypto(new Window_crypto(this))
{
    ui->setupUi(this);





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
    ui->stackedWidget->addWidget(window_crypto);
    ui->stackedWidget->setCurrentIndex(2);
    Vault vault;
    vault.dir = QDir("C:/Users/LSH/Documents/TestDirectory");
    vault.display_name = "TestDirectory";
    vault.password = "03ac674216f3e15c761ee1a5e255f067953623c8b388b4459e13f978d7c846f4"; //1234
    window_crypto->Load(vault);
}

