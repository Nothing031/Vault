#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <ctime>
#include <QDebug>
#include <QFileDialog>
#include <filesystem>
#include <string>
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


#define PROPERTY_BOOL "_PROPERTY_BOOL_"

#define PROPERTY_VAULT_DIRECTORY_PATH "__VAULT_DIRECTORY_PATH__"
#define PROPERTY_VAULT_PASSWORD "__VAULT_PASSWORD__"
#define PROPERTY_VAULT_NAME "__VAULT_NAME__"



using namespace fs;
namespace fs = std::filesystem;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}
MainWindow::~MainWindow()
{
    delete ui;
}


