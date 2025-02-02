#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <filesystem>
#include <string>
#include <QToolTip>
#include <QPoint>

#define NEW_VAULT_MODE_CREATE "NEW_VAULT_MODE_CREATE"

using namespace fs;
namespace fs = std::filesystem;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // load data
    VAULTS::GetInstance().load();
    vector<VAULT_STRUCT> vaults = VAULTS::GetInstance().getVaults();
    ui->vault_select_comboBox->addItem("select vault");
    for (const auto& vault : vaults){
        QString vaultName = QString::fromStdWString(fs::path(vault.directory).filename().wstring());
        ui->vault_select_comboBox->addItem(vaultName);
    }




    // set validator
    QRegularExpression pwRegex("^[A-Za-z0-9!@#$%^&*()_+\\-=\\[\\]{};':\"\\\\|,.<>\\/?]*$");
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(pwRegex, this);

    ui->vault_new_password_lineEdit->setValidator(validator);
    ui->vault_new_password_confirm_lineEdit->setValidator(validator);

}

MainWindow::~MainWindow()
{
    delete ui;
}

// left
void MainWindow::on_vault_select_comboBox_currentIndexChanged(int index)
{
    // goto crypto page
    if (index == 0){
        ui->stackedWidget->setCurrentIndex(1);
    }else{
        ui->stackedWidget->setCurrentIndex(0);
    }
}

void MainWindow::on_vault_createNew_button_clicked()
{
    // set property
    ui->vault_new_name_label->setProperty(NEW_VAULT_MODE_CREATE, QVariant(true));
    // goto new vault page
    ui->stackedWidget->setCurrentIndex(2);
    // set stylesheet
    ui->page_newVault->setStyleSheet(R"(QWidget{ color: rgb(100, 100, 100);})");
    ui->vault_new_label->setStyleSheet(R"(QWidget{ color: rgb(255, 255, 255);})");
    ui->vault_new_path_label->setStyleSheet(R"(QWidget{ color: rgb(255, 255, 255);})");
    // disable
    ui->vault_new_name_lineEdit->setEnabled(false);
    ui->vault_new_password_lineEdit->setEnabled(false);
    ui->vault_new_password_confirm_lineEdit->setEnabled(false);
    ui->vault_new_password_visible_button->setEnabled(false);
    ui->vault_new_password_confirm_visible_button->setEnabled(false);
    ui->vault_new_createVault_button->setEnabled(false);
    // reset text
    ui->vault_new_path_label->setText("");
    ui->vault_new_name_lineEdit->setText("");
    ui->vault_new_password_lineEdit->setText("");
    ui->vault_new_password_confirm_lineEdit->setText("");
}

void MainWindow::on_vault_createExisting_button_clicked()
{
    // set property
    ui->vault_new_name_label->setProperty(NEW_VAULT_MODE_CREATE, QVariant(false));
    // goto new vault page
    ui->stackedWidget->setCurrentIndex(2);
    // set stylesheet
    ui->page_newVault->setStyleSheet(R"(QWidget{ color: rgb(100, 100, 100);})");
    ui->vault_new_label->setStyleSheet(R"(QWidget{ color: rgb(255, 255, 255);})");
    ui->vault_new_path_label->setStyleSheet(R"(QWidget{ color: rgb(255, 255, 255);})");
    // disable
    ui->vault_new_name_lineEdit->setEnabled(false);
    ui->vault_new_password_lineEdit->setEnabled(false);
    ui->vault_new_password_confirm_lineEdit->setEnabled(false);
    ui->vault_new_password_visible_button->setEnabled(false);
    ui->vault_new_password_confirm_visible_button->setEnabled(false);
    ui->vault_new_createVault_button->setEnabled(false);
    // reset text
    ui->vault_new_path_label->setText("");
    ui->vault_new_name_lineEdit->setText("");
    ui->vault_new_password_lineEdit->setText("");
    ui->vault_new_password_confirm_lineEdit->setText("");
}

// page new vault
void MainWindow::on_vault_new_openFolder_button_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Folder", QDir::rootPath(), QFileDialog::ShowDirsOnly);
    if (dir.isEmpty()) return;
    // set property
    ui->vault_new_path_label->setProperty("PATH", QVariant(dir));
    // read property
    if (ui->vault_new_name_label->property(NEW_VAULT_MODE_CREATE).toBool()){
        // tooltip
        ui->vault_new_path_label->setToolTip(dir + "/");
        // set text
        ui->vault_new_path_label->setText(dir + "/");
        // set stylesheet
        ui->page_newVault->setStyleSheet(R"(QWidget{ color: rgb(255, 255, 255);})");
        ui->vault_new_createVault_button->setStyleSheet(R"(QWidget{ color: rgb(100, 100, 100);})");
        // enable
        ui->vault_new_name_lineEdit->setEnabled(true);
        ui->vault_new_password_lineEdit->setEnabled(true);
        ui->vault_new_password_confirm_lineEdit->setEnabled(true);
        ui->vault_new_password_visible_button->setEnabled(true);
        ui->vault_new_password_confirm_visible_button->setEnabled(true);
    }else{
        // tooltip
        ui->vault_new_path_label->setToolTip(dir);
        // set text
        ui->vault_new_path_label->setText(dir);
        ui->vault_new_name_lineEdit->setText(QString::fromStdWString(fs::path(dir.toStdWString()).filename().wstring()));
        // set stylesheet
        ui->page_newVault->setStyleSheet(R"(QWidget{ color: rgb(255, 255, 255);})");
        ui->vault_new_name_label->setStyleSheet(R"(QWidget{ color: rgb(100, 100, 100);})");
        ui->vault_new_name_lineEdit->setStyleSheet(R"(QWidget{ color: rgb(100, 100, 100);})");
        ui->vault_new_createVault_button->setStyleSheet(R"(QWidget{ color: rgb(100, 100, 100);})");
        // enable
        ui->vault_new_password_lineEdit->setEnabled(true);
        ui->vault_new_password_confirm_lineEdit->setEnabled(true);
        ui->vault_new_password_visible_button->setEnabled(true);
        ui->vault_new_password_confirm_visible_button->setEnabled(true);
    }
}


void MainWindow::on_vault_new_name_lineEdit_textEdited(const QString &arg1)
{
    QString str = ui->vault_new_path_label->property("PATH").toString() + "/" + arg1;
    //ui->vault_new_path_label->setText(old + "/" + arg1);

    int maxWidth = ui->vault_new_path_label->width();
    QFontMetrics metrics(ui->vault_new_path_label->font());
    QString elidedStr = metrics.elidedText(str, Qt::ElideLeft, maxWidth);
    ui->vault_new_path_label->setText(elidedStr);
    ui->vault_new_path_label->setToolTip(str);
}

void MainWindow::on_vault_new_password_lineEdit_textEdited(const QString &arg1)
{
    if (ui->vault_new_password_visible_button->isChecked()){
        // show
        ui->vault_new_password_lineEdit->setText(arg1);
        ui->vault_new_password_lineEdit->setProperty("PASSWORD", QVariant(arg1));
    }else{
        // hide
        QString password = ui->vault_new_password_lineEdit->property("PASSWORD").toString();
        if (arg1.size() > password.size()){
            // added
            QString newPassword = arg1;
            newPassword.replace(0, password.size(), password);
            ui->vault_new_password_lineEdit->setProperty("PASSWORD", QVariant(newPassword));
        }else{
            // deleted
            QString newPassword = password.left(arg1.size());
            ui->vault_new_password_lineEdit->setProperty("PASSWORD", QVariant(newPassword));
        }
        ui->vault_new_password_lineEdit->setText(QString("*").repeated(arg1.size()));
    }
}
void MainWindow::on_vault_new_password_confirm_lineEdit_textEdited(const QString &arg1)
{
    if (ui->vault_new_password_visible_button->isChecked()){
        // show
        ui->vault_new_password_confirm_lineEdit->setText(arg1);
        ui->vault_new_password_confirm_lineEdit->setProperty("PASSWORD", QVariant(arg1));
    }else{
        // hide
        QString password = ui->vault_new_password_confirm_lineEdit->property("PASSWORD").toString();
        if (arg1.size() > password.size()){
            // added
            QString newPassword = arg1;
            newPassword.replace(0, password.size(), password);
            ui->vault_new_password_confirm_lineEdit->setProperty("PASSWORD", QVariant(newPassword));
        }else{
            // deleted
            QString newPassword = password.left(arg1.size());
            ui->vault_new_password_confirm_lineEdit->setProperty("PASSWORD", QVariant(newPassword));
        }
        ui->vault_new_password_confirm_lineEdit->setText(QString("*").repeated(arg1.size()));
    }
}

void MainWindow::on_vault_new_password_visible_button_toggled(bool checked)
{
    // checked = show
    // unchecked = hide
    //ui->vault_new_password_confirm_visible_button->setChecked(checked);
    if (checked){
        // show
        QString password = ui->vault_new_password_lineEdit->property("PASSWORD").toString();
        ui->vault_new_password_lineEdit->setText(password);
    }else{
        // hide
        QString text = ui->vault_new_password_lineEdit->property("PASSWORD").toString();
        ui->vault_new_password_lineEdit->setText(QString("*").repeated(text.size()));
    }
}
void MainWindow::on_vault_new_password_confirm_visible_button_toggled(bool checked)
{
    //ui->vault_new_password_visible_button->setChecked(checked);
    if (checked){
        // show
        QString password = ui->vault_new_password_confirm_lineEdit->property("PASSWORD").toString();
        ui->vault_new_password_confirm_lineEdit->setText(password);
    }else{
        // hide
        QString text = ui->vault_new_password_confirm_lineEdit->property("PASSWORD").toString();
        ui->vault_new_password_confirm_lineEdit->setText(QString("*").repeated(text.size()));
    }
}

