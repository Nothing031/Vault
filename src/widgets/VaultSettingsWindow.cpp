#include "VaultSettingsWindow.h"
#include "ui_VaultSettingsWindow.h"

VaultSettingsWindow::VaultSettingsWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::VaultSettingsWindow)
{
    ui->setupUi(this);
}

VaultSettingsWindow::~VaultSettingsWindow()
{
    delete ui;
}
