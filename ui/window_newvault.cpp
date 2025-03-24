#include "window_newvault.h"
#include "ui_window_newvault.h"

#include "src/crypto.hpp"

#include <filesystem>

#include <QString>
#include <QFileDialog>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

namespace fs = std::filesystem;

window_newvault::window_newvault(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Window_NewVault)
{
    ui->setupUi(this);

    // validator
    QRegularExpression path_re(R"([^\\/:*?\"<>|]*)");
    QRegularExpression password_re(R"(^[A-Za-z0-9!@#$%^&*()_+\-=\[\]{};':\"\\|,.<>\\/?]*$)");
    QRegularExpressionValidator *path_validator = new QRegularExpressionValidator(path_re, this);
    QRegularExpressionValidator *password_validator = new QRegularExpressionValidator(password_re, this);

    ui->vault_name_lineedit->setValidator(path_validator);
    ui->password_input_lineedit->setValidator(password_validator);
    ui->confirm_input_lineedit->setValidator(password_validator);
}

window_newvault::~window_newvault()
{
    delete ui;
}

void window_newvault::ConditionCheck()
{
    QString styleSheet;
    if (NewVaultMode == NewVault::CreateNew){
        QString styleSheet = "QLabel{color:rgb";
        styleSheet += (condition_path ? "(55, 255, 55);}" : "(255, 55, 55);}");
        ui->vault_l_label->setStyleSheet(styleSheet);
    }
    styleSheet = "QLabel{color:rgb";
    styleSheet += (condition_password ? "(55, 255, 55);}" : "(255, 55, 55);}");
    ui->password_l_label->setStyleSheet(styleSheet);

    styleSheet = "QLabel{color:rgb";
    styleSheet += (condition_confirm ? "(55, 255, 55);}" : "(255, 55, 55);}");
    ui->confirm_l_label->setStyleSheet(styleSheet);

    if (condition_path && condition_password && condition_confirm){
        ui->vault_create->setEnabled(true);
    }
}

void window_newvault::on_request_page(const NewVault& mode)
{
    NewVaultMode = mode;
    ui->vault_l_label->setEnabled(false);
    ui->vault_name_lineedit->setEnabled(false);
    ui->password_l_label->setEnabled(false);
    ui->password_input_lineedit->setEnabled(false);
    ui->password_visibility_button->setEnabled(false);
    ui->confirm_l_label->setEnabled(false);
    ui->confirm_input_lineedit->setEnabled(false);
    ui->confirm_visibility_button->setEnabled(false);
    ui->vault_create->setEnabled(false);

    QString style = "QWidget{color: rgb(255, 255, 255);}";
    ui->vault_l_label->setStyleSheet(style);
    ui->password_l_label->setStyleSheet(style);
    ui->confirm_l_label->setStyleSheet(style);


    ui->directory_path_label->setText("");
    ui->vault_name_lineedit->setText("");
    ui->password_input_lineedit->setText("");
    ui->confirm_input_lineedit->setText("");
}

void window_newvault::on_directory_open_button_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Folder", QDir::rootPath(), QFileDialog::ShowDirsOnly);
    if (dir.isEmpty()) return;
    directory = QDir(dir);
    if (!directory.exists()) return;
    ui->directory_path_label->setText(dir);
    if (NewVaultMode == NewVault::CreateNew){
        ui->vault_l_label->setEnabled(true);
        ui->vault_name_lineedit->setEnabled(true);
    }else{
        condition_path = true;
    }
    ui->password_l_label->setEnabled(true);
    ui->password_input_lineedit->setEnabled(true);
    ui->password_visibility_button->setEnabled(true);
    ui->confirm_l_label->setEnabled(true);
    ui->confirm_input_lineedit->setEnabled(true);
    ui->confirm_visibility_button->setEnabled(true);
    ConditionCheck();
}

void window_newvault::on_vault_name_lineedit_textEdited(const QString &arg1)
{
    ui->directory_path_label->setText(directory.path() + "/" + arg1);
    condition_path = (!fs::exists(ui->directory_path_label->text().toStdWString()) ? true : false);
    ConditionCheck();
}

void window_newvault::on_password_visibility_button_toggled(bool checked)
{
    ui->password_input_lineedit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
}

void window_newvault::on_password_input_lineedit_textEdited(const QString &arg1)
{
    condition_password = !(arg1.size() < 4);
    if (condition_password && arg1 == ui->confirm_input_lineedit->text()){
        condition_confirm = true;
    }else{
        condition_confirm = false;
    }
    ConditionCheck();
}

void window_newvault::on_confirm_visibility_button_toggled(bool checked)
{
    ui->confirm_input_lineedit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
}

void window_newvault::on_confirm_input_lineedit_textEdited(const QString &arg1)
{
    if (arg1 == ui->password_input_lineedit->text() && !(arg1.size() < 4)){
        condition_confirm = true;
    }else{
        condition_confirm = false;
    }
    ConditionCheck();
}

void window_newvault::on_vault_create_clicked()
{
    directory = QDir(ui->directory_path_label->text());
    if (!directory.exists()){
        qDebug() << "[NEWVAULT] Directory not exists, creating director :" << directory.path();
        QDir().mkdir(directory.path());
    }

    Vault newVault;
    newVault.displayName = directory.dirName();
    newVault.dir = directory;
    newVault.password = Crypto::SHA256(ui->confirm_input_lineedit->text());

    // create
    emit signal_create_new_vault(newVault);
}

