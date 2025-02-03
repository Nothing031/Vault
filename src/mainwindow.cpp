#include "mainwindow.h"
#include "ui_mainwindow.h"

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

#include "crypto.hpp"

#define NEW_VAULT_MODE_CREATE "NEW_VAULT_MODE_CREATE"

using namespace fs;
namespace fs = std::filesystem;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // load data
    VAULTS::load();
    current_vaults = VAULTS::getVaults();
    // load to combobox;
    ui->vault_select_comboBox->addItem("select vault");
    for (const auto& vault : current_vaults){
        QString directory = QString::fromStdWString(vault.directory);
        QString vaultName = QString::fromStdWString(fs::path(vault.directory).filename().wstring());
        ui->vault_select_comboBox->addItem(vaultName, QVariant(directory));
    }

    // set validator
    QRegularExpression pwRegex("^[A-Za-z0-9!@#$%^&*()_+\\-=\\[\\]{};':\"\\\\|,.<>\\/?]*$");
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(pwRegex, this);

    ui->vault_new_password_lineEdit->setValidator(validator);
    ui->vault_new_password_confirm_lineEdit->setValidator(validator);


    // set page
    ui->stackedWidget->setCurrentIndex(1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// left
void MainWindow::on_vault_select_comboBox_currentIndexChanged(int index)
{


}

void MainWindow::on_vault_createNew_button_clicked()
{
    initNewVaultPage();
    // set property
    ui->vault_new_name_label->setProperty(NEW_VAULT_MODE_CREATE, QVariant(true));
    // goto new vault page
    ui->stackedWidget->setCurrentIndex(2);
}
void MainWindow::on_vault_createExisting_button_clicked()
{
    initNewVaultPage();
    // set property
    ui->vault_new_name_label->setProperty(NEW_VAULT_MODE_CREATE, QVariant(false));
    // goto new vault page
    ui->stackedWidget->setCurrentIndex(2);

}

// page new vault
void MainWindow::setCreateButton(){
    bool bool1 = ui->vault_new_name_label->property("BOOL").toBool();
    bool bool2 = ui->vault_new_password_label->property("BOOL").toBool();
    bool bool3 = ui->vault_new_password_confirm_label->property("BOOL").toBool();
    // !ui->vault_new_createVault_button->isEnabled()
    if (bool1 && bool2 && bool3){
        ui->vault_new_createVault_button->setStyleSheet("QWidget{color: rgb(255, 255, 255);}");
        ui->vault_new_createVault_button->setEnabled(true);
    }else if (ui->vault_new_createVault_button->isEnabled()){
        ui->vault_new_createVault_button->setStyleSheet("QWidget{color: rgb(100, 100, 100);}");
        ui->vault_new_createVault_button->setEnabled(false);
    }
}
void MainWindow::setPasswordLabel(){
    bool labelBool = ui->vault_new_password_label->property("BOOL").toBool();
    bool pw_condition_confirmed;
    int textSize = ui->vault_new_password_lineEdit->text().size();
    if(textSize < 4) pw_condition_confirmed = false;
    else pw_condition_confirmed = true;
    if (pw_condition_confirmed && !labelBool){
        ui->vault_new_password_label->setStyleSheet("QWidget{color: rgb(100, 255, 100);}");
        ui->vault_new_password_label->setProperty("BOOL", QVariant(true));
    }
    else if (!pw_condition_confirmed && labelBool){
        ui->vault_new_password_label->setStyleSheet("QWidget{color: rgb(255, 100, 100);}");
        ui->vault_new_password_label->setProperty("BOOL", QVariant(false));
    }
}
void MainWindow::setPasswordConfirmLabel(){
    bool labelBool = ui->vault_new_password_confirm_label->property("BOOL").toBool();
    QString password = ui->vault_new_password_lineEdit->text();
    QString password_confirm = ui->vault_new_password_confirm_lineEdit->text();
    if (password.size() >= 4 && password == password_confirm && !labelBool){
        if (!labelBool){
            ui->vault_new_password_confirm_label->setStyleSheet("QWidget{color: rgb(100, 255, 100);}");
            ui->vault_new_password_confirm_label->setProperty("BOOL", QVariant(true));
        }
    }else{
        if (labelBool){
            ui->vault_new_password_confirm_label->setStyleSheet("QWidget{color: rgb(255, 100, 100);}");
            ui->vault_new_password_confirm_label->setProperty("BOOL", QVariant(false));
        }
    }
}
void MainWindow::initNewVaultPage(){
    // set stylesheet
    ui->vault_new_label->setStyleSheet("QWidget{ color: rgb(255, 255, 255);}");
    ui->vault_new_path_label->setStyleSheet("QWidget{ color: rgb(255, 255, 255);}");
    ui->vault_new_name_label->setStyleSheet("QWidget{ color: rgb(100, 100, 100);}");
    ui->vault_new_name_lineEdit->setStyleSheet("QWidget{ color: rgb(100, 100, 100);}");
    ui->vault_new_password_label->setStyleSheet("QWidget{ color: rgb(100, 100, 100);}");
    ui->vault_new_password_lineEdit->setStyleSheet("QWidget{ color: rgb(100, 100, 100);}");
    ui->vault_new_password_confirm_label->setStyleSheet("QWidget{ color: rgb(100, 100, 100);}");
    ui->vault_new_password_confirm_lineEdit->setStyleSheet("QWidget{ color: rgb(100, 100, 100);}");
    ui->vault_new_createVault_button->setStyleSheet("QWidget{ color: rgb(100, 100, 100);}");
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
    // reset button
    ui->vault_new_password_visible_button->setChecked(false);
    ui->vault_new_password_confirm_visible_button->setChecked(false);
    // reset property
    ui->vault_new_name_label->setProperty(NEW_VAULT_MODE_CREATE, QVariant(false));
    ui->vault_new_name_label->setProperty("BOOL", QVariant(false));
    ui->vault_new_password_label->setProperty("BOOL", QVariant(false));
    ui->vault_new_password_confirm_label->setProperty("BOOL", QVariant(false));
}

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
        ui->vault_new_name_label->setStyleSheet(R"(QWidget{ color: rgb(255, 100, 100);})");
        ui->vault_new_name_lineEdit->setStyleSheet(R"(QWidget{ color: rgb(255, 255, 255);})");
        // enable
        ui->vault_new_name_lineEdit->setEnabled(true);
    }else{
        // tooltip
        ui->vault_new_path_label->setToolTip(dir);
        // set text
        ui->vault_new_path_label->setText(dir);
        ui->vault_new_name_lineEdit->setText(QString::fromStdWString(fs::path(dir.toStdWString()).filename().wstring()));
    }
    ui->vault_new_password_label->setStyleSheet(R"(QWidget{ color: rgb(255, 100, 100);})");
    ui->vault_new_password_lineEdit->setStyleSheet(R"(QWidget{ color: rgb(255, 255, 255);})");
    ui->vault_new_password_confirm_label->setStyleSheet(R"(QWidget{ color: rgb(255, 100, 100);})");
    ui->vault_new_password_confirm_lineEdit->setStyleSheet(R"(QWidget{ color: rgb(255, 255, 255);})");

    ui->vault_new_password_lineEdit->setEnabled(true);
    ui->vault_new_password_confirm_lineEdit->setEnabled(true);
    ui->vault_new_password_visible_button->setEnabled(true);
    ui->vault_new_password_confirm_visible_button->setEnabled(true);
}

void MainWindow::on_vault_new_name_lineEdit_textEdited(const QString &arg1)
{
    // set text
    QString str = ui->vault_new_path_label->property("PATH").toString() + "/" + arg1;
    int maxWidth = ui->vault_new_path_label->width();
    QFontMetrics metrics(ui->vault_new_path_label->font());
    QString elidedStr = metrics.elidedText(str, Qt::ElideLeft, maxWidth);
    ui->vault_new_path_label->setText(elidedStr);
    // set tooltip
    ui->vault_new_path_label->setToolTip(str);

    // check
    bool nameBool = ui->vault_new_name_label->property("BOOL").toBool();
    bool condition_confirmed;
    if(arg1.size() < 1) condition_confirmed = false;
    else condition_confirmed = true && !fs::exists(str.toStdWString());

    if (condition_confirmed && !nameBool){
        ui->vault_new_name_label->setStyleSheet("QWidget{color: rgb(100, 255, 100);}");
        ui->vault_new_name_label->setProperty("BOOL", QVariant(true));
    }else if (!condition_confirmed && nameBool){
        ui->vault_new_name_label->setStyleSheet("QWidget{color: rgb(255, 100, 100);}");
        ui->vault_new_name_label->setProperty("BOOL", QVariant(false));
    }
    setCreateButton();
}

void MainWindow::on_vault_new_password_lineEdit_textEdited(const QString &arg1)
{
    setPasswordLabel();
    setPasswordConfirmLabel();
    setCreateButton();
}
void MainWindow::on_vault_new_password_confirm_lineEdit_textEdited(const QString &arg1)
{
    setPasswordConfirmLabel();
    setCreateButton();
}

void MainWindow::on_vault_new_password_visible_button_toggled(bool checked)
{
    // show
    if (checked)
        ui->vault_new_password_lineEdit->setEchoMode(QLineEdit::Normal);
    // hide
    else
        ui->vault_new_password_lineEdit->setEchoMode(QLineEdit::Password);

}
void MainWindow::on_vault_new_password_confirm_visible_button_toggled(bool checked)
{
    // show
    if (checked)
        ui->vault_new_password_confirm_lineEdit->setEchoMode(QLineEdit::Normal);
    // hide
    else
        ui->vault_new_password_confirm_lineEdit->setEchoMode(QLineEdit::Password);
}

void MainWindow::on_vault_new_createVault_button_clicked()
{
    VAULT_STRUCT vault;
    QString qDirectory = ui->vault_new_path_label->text();
    QString qVaultName = QString::fromStdWString(fs::path(qDirectory.toStdWString()).filename().wstring());
    QString qPassword = ui->vault_new_password_lineEdit->text();

    vault.directory = qDirectory.toStdWString();
    vault.password = crypto::sha256(qPassword.toStdString());

    VAULTS::addOrReplaceVault(vault);
    VAULTS::save();

    qDebug() << "vault created and saved : ";
    qDebug() << "    Directory: " << vault.directory;
    qDebug() << "    Password: " << vault.password;

    // add to combobox
    ui->vault_select_comboBox->addItem(qVaultName, QVariant(qDirectory));
    int comboBoxCount = ui->vault_select_comboBox->count();
    ui->vault_select_comboBox->setCurrentIndex(comboBoxCount - 1);
}




///// crypto page
void MainWindow::InitCryptoPage(){
    // set stylesheet
    ui->crypto_decrypt_button->setStyleSheet("QWidget{color: rgb(100, 100, 100);}");
    ui->crypto_encrypt_button->setStyleSheet("QWidget{color: rgb(100, 100, 100);}");
    ui->crypto_suspend_button->setStyleSheet("QWidget{color: rgb(100, 100, 100);}");

    // disable
    ui->crypto_decrypt_button->setEnabled(false);
    ui->crypto_encrypt_button->setEnabled(false);
    ui->crypto_suspend_button->setEnabled(false);

    // clear data
    ui->fileViewer_list->clear();
}

void MainWindow::on_password_edit_lineedit_editingFinished()
{
    QString qPassword = ui->password_edit_lineedit->text();
    string hashedPassword = crypto::sha256(qPassword.toStdString());
    if (hashedPassword == current_vault.password){
        ui->outputTerminal_textEdit->append("Vault unlocked!");
        // disable
        ui->password_edit_lineedit->setEnabled(false);

        // unlock


    }else{
        ui->outputTerminal_textEdit->append("Wrong password!");
    }
}

void MainWindow::on_password_visible_button_toggled(bool checked)
{
    // show
    if (checked)
        ui->password_edit_lineedit->setEchoMode(QLineEdit::Normal);
    // hide
    else
        ui->password_edit_lineedit->setEchoMode(QLineEdit::Password);
}





void MainWindow::on_vault_select_comboBox_activated(int index)
{
    InitCryptoPage();
    if (index == 0){
        // default page
        ui->stackedWidget->setCurrentIndex(1);
    }else{
        // goto crypto page
        ui->stackedWidget->setCurrentIndex(0);
        // load vault
        QString qDir = ui->vault_select_comboBox->itemData(index).toString();
        QString qName = ui->vault_select_comboBox->itemText(index);
        vector<VAULT_STRUCT> vaults = VAULTS::getVaults();
        bool found = false;
        for (const auto& vault : vaults){
            if (vault.directory == qDir.toStdWString()){
                current_vault = vault;
                found = true;
                break;
            }
        }
        if (!found){
            ui->stackedWidget->setCurrentIndex(1);
            ui->outputTerminal_textEdit->append("error cannot find vault");
        }

        // set ui
        ui->vault_directory_path_label->setText(qDir);
        ui->vault_edit_lineEdit->setText(qName);

        // load data
        fs::path directory(current_vault.directory);
        for (const auto& file : fs::recursive_directory_iterator(directory)){
            if (file.is_regular_file()){
                current_directory_files.push_back(file.path());
            }
        }


        // sort
        QListWidgetItem *item;
        for (const auto& file : current_directory_files){
            if (file.extension() == ".enc"){
                current_directory_encrypted_files.push_back(file);
                item = new QListWidgetItem(QString::fromStdWString(fs::relative(file, directory).wstring()));
                item->setForeground(QBrush(QColor(100, 255, 100)));
                ui->fileViewer_list->addItem(item);
            }else{
                current_directory_decrypted_files.push_back(file);
                item = new QListWidgetItem(QString::fromStdWString(fs::relative(file, directory).wstring()));
                item->setForeground(QBrush(QColor(255, 100, 100)));
                ui->fileViewer_list->addItem(item);
            }
        }
    }
}


void MainWindow::on_vault_openFolder_button_clicked()
{
    QUrl url = QUrl::fromLocalFile(QString::fromStdWString(current_vault.directory));
    QDesktopServices::openUrl(url);
}

