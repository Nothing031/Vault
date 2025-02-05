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

#include "crypto.hpp"

#define NEW_VAULT_MODE_CREATE "NEW_VAULT_MODE_CREATE"

using namespace fs;
namespace fs = std::filesystem;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow), watcher(new QFileSystemWatcher(this))
{
    crypto = new Crypto();

    ui->setupUi(this);

    // connect
    connect(crypto, &Crypto::signal_encrypted, this, [this](QStringList paths){
        ui->progressBar->setValue(ui->progressBar->value() + paths.size());
        for(const auto& path : paths){
            ui->outputTerminal_textEdit->append("file encrypted : " + path);
        }
    });
    connect(crypto, &Crypto::signal_decrypted, this, [this](QStringList paths){
        ui->progressBar->setValue(ui->progressBar->value() + paths.size());
        for(const auto& path : paths){
            ui->outputTerminal_textEdit->append("file decrypted : " + path);
        }
    });
    connect(crypto, &Crypto::signal_suspended, this, [this](QString text){
        ui->outputTerminal_textEdit->append(text);
        watcher->blockSignals(false);
        ui->crypto_encrypt_button->setEnabled(true);
        ui->crypto_decrypt_button->setEnabled(true);
        ui->crypto_suspend_button->setEnabled(false);
        refreshCryptoPage();
    });
    connect(crypto, &Crypto::signal_exception, this, [this](QString exception){
        ui->outputTerminal_textEdit->append(exception);
        ui->progressBar->setValue(ui->progressBar->value() + 1);
    });
    connect(crypto, &Crypto::signal_work_encryption_done, this, [this](){
        ui->outputTerminal_textEdit->append("file encryption completed");
        watcher->blockSignals(false);
        ui->crypto_encrypt_button->setEnabled(true);
        ui->crypto_decrypt_button->setEnabled(true);
        ui->crypto_suspend_button->setEnabled(false);
        refreshCryptoPage();
    });
    connect(crypto, &Crypto::signal_work_decryption_done, this, [this](){
        ui->outputTerminal_textEdit->append("file decryption completed");
        watcher->blockSignals(false);
        ui->crypto_encrypt_button->setEnabled(true);
        ui->crypto_decrypt_button->setEnabled(true);
        ui->crypto_suspend_button->setEnabled(false);
        refreshCryptoPage();
    });
    connect(crypto, &Crypto::signal_finalExceptions, this, [this](QStringList exceptions){
        if (exceptions.size()){
            ui->outputTerminal_textEdit->append("exceptions: ");
            for (const auto& exc : exceptions){
                ui->outputTerminal_textEdit->append(exc);
            }
        }
    });


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

// left ///////////////////////////////////////////////////////
void MainWindow::on_vault_select_comboBox_activated(int index)
{
    if (index == 0){
        // default page
        ui->stackedWidget->setCurrentIndex(1);
    }else{
        // goto crypto page
        ui->stackedWidget->setCurrentIndex(0);
        // init
        QString qDir = ui->vault_select_comboBox->itemData(index).toString();
        QString qName = ui->vault_select_comboBox->itemText(index);
        InitCryptoPage();
        LoadCryptoPageData(qDir);
    }
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
    ui->vault_new_name_label->setProperty("BOOL", QVariant(true));
    // goto new vault page
    ui->stackedWidget->setCurrentIndex(2);

}

// new vault page ///////////////////////////////////////////////////////
void MainWindow::initNewVaultPage(){
    // set stylesheet
    ui->vault_new_name_label->setStyleSheet("QWidget{ color: rgb(100, 100, 100);}");
    ui->vault_new_password_label->setStyleSheet("QWidget{ color: rgb(100, 100, 100);}");
    ui->vault_new_password_confirm_label->setStyleSheet("QWidget{ color: rgb(100, 100, 100);}");
    // disable
    ui->vault_new_name_label->setEnabled(false);
    ui->vault_new_name_lineEdit->setEnabled(false);
    ui->vault_new_password_label->setEnabled(false);
    ui->vault_new_password_lineEdit->setEnabled(false);
    ui->vault_new_password_confirm_label->setEnabled(false);
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
//// set confirm state
void MainWindow::setCreateButton(){
    bool bool1 = ui->vault_new_name_label->property("BOOL").toBool();
    bool bool2 = ui->vault_new_password_label->property("BOOL").toBool();
    bool bool3 = ui->vault_new_password_confirm_label->property("BOOL").toBool();
    // !ui->vault_new_createVault_button->isEnabled()
    if (bool1 && bool2 && bool3)
        ui->vault_new_createVault_button->setEnabled(true);
    else if (ui->vault_new_createVault_button->isEnabled())
        ui->vault_new_createVault_button->setEnabled(false);
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
//// directory
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
        // ui->vault_new_name_lineEdit->setStyleSheet(R"(QWidget{ color: rgb(255, 255, 255);})");
        // enable
        ui->vault_new_name_lineEdit->setEnabled(true);
    }else{
        // tooltip
        ui->vault_new_path_label->setToolTip(dir);
        // set text
        ui->vault_new_path_label->setText(dir);
        ui->vault_new_name_lineEdit->setText(QString::fromStdWString(fs::path(dir.toStdWString()).filename().generic_wstring()));
    }
    ui->vault_new_password_label->setStyleSheet(R"(QWidget{ color: rgb(255, 100, 100);})");
    ui->vault_new_password_confirm_label->setStyleSheet(R"(QWidget{ color: rgb(255, 100, 100);})");

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
//// password
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
//// createVault
void MainWindow::on_vault_new_createVault_button_clicked()
{
    VAULT_STRUCT vault;
    QString qDirectory = ui->vault_new_path_label->text();
    QString qVaultName = QString::fromStdWString(fs::path(qDirectory.toStdWString()).filename().wstring());
    QString qPassword = ui->vault_new_password_lineEdit->text();

    vault.directory = qDirectory.toStdWString();
    vault.password = Crypto::sha256(qPassword.toStdString());

    VAULTS::addOrReplaceVault(vault);
    VAULTS::save();

    qDebug() << "vault created and saved : ";
    qDebug() << "    Directory: " << vault.directory;
    qDebug() << "    Password: " << vault.password;

    if(!fs::exists(qDirectory.toStdWString())){
        fs::create_directory(qDirectory.toStdWString());
    }

    // add to combobox
    ui->vault_select_comboBox->addItem(qVaultName, QVariant(qDirectory));
    int comboBoxCount = ui->vault_select_comboBox->count();
    ui->vault_select_comboBox->setCurrentIndex(comboBoxCount - 1);
    on_vault_select_comboBox_activated(comboBoxCount - 1);
}

// crypto page /////////////////////////////////////////////////////////
void MainWindow::InitCryptoPage(){
    // set stylesheet
    // ui->crypto_decrypt_button->setStyleSheet("QWidget{color: rgb(100, 100, 100);}");
    // ui->crypto_encrypt_button->setStyleSheet("QWidget{color: rgb(100, 100, 100);}");
    // ui->crypto_suspend_button->setStyleSheet("QWidget{color: rgb(100, 100, 100);}");

    // setenable
    ui->password_edit_lineedit->setEnabled(true);
    ui->crypto_decrypt_button->setEnabled(false);
    ui->crypto_encrypt_button->setEnabled(false);
    ui->crypto_suspend_button->setEnabled(false);

    // clear ui
    ui->fileViewer_list->clear();
    ui->vault_directory_path_label->setText("");
    ui->vault_edit_lineEdit->setText("");
    ui->password_edit_lineedit->setText("");
    ui->progressBar->setTextVisible(false);
    ui->progressBar->setValue(0);

    // clear data
    current_vault = {};
    current_directory_files.clear();
    current_directory_encrypted_files.clear();
    current_directory_decrypted_files.clear();

    // init watcher
    QStringList list = watcher->directories();
    if (list.size()){
        watcher->removePaths(list);
        watcher->disconnect();
    }
}
void MainWindow::LoadCryptoPageData(const QString& QDirectoryPath){
    // load vault
    for (const auto& v : current_vaults){
        if (v.directory == QDirectoryPath.toStdWString()){
            current_vault = v;
            break;
        }
    }
    // load all files
    for (const auto& file : fs::recursive_directory_iterator(current_vault.directory)){
        if (file.is_regular_file()){
            FILE_STRUCT fStruct;
            fStruct.path = file.path();
            fStruct.relativePath = QString::fromStdWString(fs::relative(fStruct.path, current_vault.directory).wstring());
            fStruct.encrypted = fStruct.path.extension() == ".enc" ? true : false;
            if (fStruct.encrypted){
                int size = fStruct.relativePath.size();
                fStruct.relativePath = fStruct.relativePath.left(size - 4);
            }
            current_directory_files.push_back(fStruct);
        }
    }
    // sort
    sort(current_directory_files.begin(), current_directory_files.end(), [](const FILE_STRUCT& a, const FILE_STRUCT& b){
        return a.relativePath < b.relativePath;
    });
    // add to viewer
    QListWidgetItem *item;
    for (const auto& file : current_directory_files){
        item = new QListWidgetItem;
        item->setForeground(QBrush(file.encrypted ? QColor(100, 255, 100) : QColor(255, 100, 100)));
        item->setText(file.relativePath);
        ui->fileViewer_list->addItem(item);
    }
}
void MainWindow::refreshCryptoPage(){
    ui->fileViewer_list->clear();
    QListWidgetItem *item;
    for (const auto& file : current_directory_files){
        item = new QListWidgetItem;
        item->setForeground(QBrush(file.encrypted ? QColor(100, 255, 100) : QColor(255, 100, 100)));
        item->setText(file.relativePath);
        ui->fileViewer_list->addItem(item);
    }
}
void MainWindow::replaceFile(const FILE_STRUCT& fStruct){
    auto it = lower_bound(current_directory_files.begin(), current_directory_files.end(), fStruct, [](const FILE_STRUCT& a, const FILE_STRUCT& b){
        return a.relativePath < b.relativePath;
    });
    if (it != current_directory_files.end() && it->relativePath == fStruct.relativePath){
        *it = fStruct;
    }
}
//// password
void MainWindow::on_password_edit_lineedit_editingFinished()
{
    QString qPassword = ui->password_edit_lineedit->text();
    string password = qPassword.toStdString();
    string hashedPassword = Crypto::sha256(qPassword.toStdString());
    if (hashedPassword == current_vault.password){
        ui->outputTerminal_textEdit->append("Vault unlocked!");
        // create key
        ui->outputTerminal_textEdit->append("generating key...");
        QCoreApplication::processEvents();
        clock_t start, now;
        start = clock();
        PKCS5_PBKDF2_HMAC_SHA1(password.c_str(), password.size(), nullptr, 0, 600000, 32, key);
        now = clock();
        ui->outputTerminal_textEdit->append("key generated, time elapsed: " + QString::number(now - start) + "ms");
        // set enable
        ui->password_edit_lineedit->setEnabled(false);
        ui->crypto_encrypt_button->setEnabled(true);
        ui->crypto_decrypt_button->setEnabled(true);
        ui->crypto_suspend_button->setEnabled(true);
    }else{
        ui->outputTerminal_textEdit->append("password : " + QString::fromStdString(current_vault.password));
        ui->outputTerminal_textEdit->append("Wrong password! : " + QString::fromStdString(hashedPassword));
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
//// openFolder, null
void MainWindow::on_vault_openFolder_button_clicked()
{
    QUrl url = QUrl::fromLocalFile(QString::fromStdWString(current_vault.directory));
    QDesktopServices::openUrl(url);
}
//// encrypt, decrypt, suspend
void MainWindow::on_crypto_encrypt_button_clicked()
{
    // prepare
    watcher->blockSignals(true);
    ui->crypto_encrypt_button->setEnabled(false);
    ui->crypto_decrypt_button->setEnabled(false);
    ui->crypto_suspend_button->setEnabled(true);
    int count = 0;
    for (const auto& file : current_directory_files){
        if(!file.encrypted){
            count++;
        }
    }
    if (count == 0){
        watcher->blockSignals(false);
        ui->crypto_encrypt_button->setEnabled(true);
        ui->crypto_decrypt_button->setEnabled(true);
        ui->crypto_suspend_button->setEnabled(false);
        return;
    }else{
        ui->progressBar->setValue(0);
        ui->progressBar->setRange(0, count);
    }
    // start encrypt
    if (crypto->start_encrypt(current_directory_files, key)){
        return;
    }else{
        watcher->blockSignals(false);
        ui->crypto_encrypt_button->setEnabled(true);
        ui->crypto_decrypt_button->setEnabled(true);
        ui->crypto_suspend_button->setEnabled(false);
        ui->outputTerminal_textEdit->append("failed to start encryption. the thread is in working");
    }
}
void MainWindow::on_crypto_decrypt_button_clicked()
{
    // prepare
    watcher->blockSignals(true);
    ui->crypto_encrypt_button->setEnabled(false);
    ui->crypto_decrypt_button->setEnabled(false);
    ui->crypto_suspend_button->setEnabled(true);
    int count = 0;
    for (const auto& file : current_directory_files){
        if(file.encrypted){
            count++;
        }
    }
    if (count == 0){
        watcher->blockSignals(false);
        ui->crypto_encrypt_button->setEnabled(true);
        ui->crypto_decrypt_button->setEnabled(true);
        ui->crypto_suspend_button->setEnabled(false);
        return;
    }else{
        ui->progressBar->setValue(0);
        ui->progressBar->setRange(0, count);
    }
    // start encrypt
    if (crypto->start_decrypt(current_directory_files, key)){
        return;
    }else{
        watcher->blockSignals(false);
        ui->crypto_encrypt_button->setEnabled(true);
        ui->crypto_decrypt_button->setEnabled(true);
        ui->crypto_suspend_button->setEnabled(false);
        ui->outputTerminal_textEdit->append("failed to start decryption. the thread is in working");
    }
}
void MainWindow::on_crypto_suspend_button_clicked()
{
    ui->outputTerminal_textEdit->append("suspending...");
    ui->crypto_suspend_button->setEnabled(false);
    crypto->suspend();
}
