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

#include "crypto.hpp"



#define PROPERTY_BOOL "_PROPERTY_BOOL_"

#define PROPERTY_VAULT_DIRECTORY_PATH "__VAULT_DIRECTORY_PATH__"
#define PROPERTY_VAULT_PASSWORD "__VAULT_PASSWORD__"
#define PROPERTY_VAULT_NAME "__VAULT_NAME__"



using namespace fs;
namespace fs = std::filesystem;

void MainWindow::Vault_ComboBox_LoadVaults(){
    ui->vault_select_comboBox->blockSignals(true);
    ui->vault_select_comboBox->clear();
    VAULTS::load();
    current_vaults = VAULTS::getVaults();
    // load to combobox;
    for (int i = 0; i < current_vaults.size(); i++){
        QString vaultName = QString::fromStdWString(fs::path(current_vaults[i].directory).filename().wstring());
        ui->vault_select_comboBox->addItem(vaultName, QVariant(i));
        qDebug() << "Vault loaded: " << vaultName;
        qDebug() << "  Directory: " << QString::fromStdWString(current_vaults.at(i).directory);
        qDebug() << "  index: " << i;
    }
    ui->vault_select_comboBox->setCurrentIndex(-1);
    ui->vault_select_comboBox->blockSignals(false);
}

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    crypto = new Crypto();
    watcher = new QFileSystemWatcher(this);

    ui->setupUi(this);

    // connect
    connect(crypto, &Crypto::signal_outputMessage, this, [this](QStringList msgs, eCryptoState cState, eCryptoMode cMode){
        QString outputText;
        switch (cState){
        case eCryptoState::SUCCESS:
            ui->progressBar->setValue(ui->progressBar->value() + msgs.size());
            break;
        case eCryptoState::FAIL:
            ui->progressBar->setValue(ui->progressBar->value() + msgs.size());
            for (const auto& msg : msgs){
                outputText = "<font color=#ff6464>Failed to " + QString(cMode == eCryptoMode::ENCRYPTION ? "encrypt file: " : "decrypt file: ") + msg + "</font>";
                ui->outputTerminal_textEdit->append(outputText);
            }
            break;
        case eCryptoState::END:
            outputText = QString(cMode == eCryptoMode::ENCRYPTION ? "File encryption " : "File decryption ") + "successed";
            ui->outputTerminal_textEdit->append(outputText);
            watcher->blockSignals(false);
            ui->vault_select_comboBox->setEnabled(true);
            ui->crypto_decrypt_button->setEnabled(true);
            ui->crypto_encrypt_button->setEnabled(true);
            ui->crypto_suspend_button->setEnabled(false);
            break;
        case eCryptoState::ABORT:
            outputText = QString(cMode == eCryptoMode::DECRYPTION ? "File encryption " : "File decryption ") + "suspended";
            ui->outputTerminal_textEdit->append(outputText);
            ui->progressBar->setValue(0);
            ui->progressBar->setRange(0, 100);
            watcher->blockSignals(false);
            ui->vault_select_comboBox->setEnabled(true);
            ui->crypto_decrypt_button->setEnabled(true);
            ui->crypto_encrypt_button->setEnabled(true);
            ui->crypto_suspend_button->setEnabled(false);
            break;
        }
    });

    // load data
    Vault_ComboBox_LoadVaults();

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

void MainWindow::appendToTerminal(const QString &message)
{
    ui->outputTerminal_textEdit->append(message);
}

void MainWindow::setFileViewerModel(QStandardItemModel *model)
{
    ui->fileviewer_listView->setModel(model);
}

// left ///////////////////////////////////////////////////////
void MainWindow::on_vault_select_comboBox_currentIndexChanged(int index)
{
    if (index == -1){
        // default page
        ui->stackedWidget->setCurrentIndex(1);
    }else{
        // load vault
        QString qName = ui->vault_select_comboBox->currentText();
        int vault_index = ui->vault_select_comboBox->currentData().toInt();
        current_vault = current_vaults.at(vault_index);
        QString qDir = QString::fromStdWString(current_vault.directory);
        if (qDir.isEmpty() || qName.isEmpty()){
            qDebug() << "error : no data in comboBox";
            ui->outputTerminal_textEdit->append("<font color=\"red\">Error no data in selectBox</font>");
            ui->vault_select_comboBox->setCurrentIndex(-1);
            return;
        }
        if (!fs::exists(qDir.toStdWString())){
            qDebug() << "error : cannot find directory : " << qDir;
            ui->outputTerminal_textEdit->append("<font color=\"red\">Error cannot find directory: " + qDir + "</font><br>");
            ui->vault_select_comboBox->setCurrentIndex(-1);
            return;
        }

        InitCryptoPage();
        LoadCryptoPage();
        ui->stackedWidget->setCurrentIndex(0);
    }
}
void MainWindow::on_vault_createNew_button_clicked()
{
    initNewVaultPage();
    ui->stackedWidget->setCurrentIndex(2);
}
void MainWindow::on_vault_createExisting_button_clicked()
{
    initNewVaultPage();
    ui->vault_new_name_label->setProperty(PROPERTY_BOOL, QVariant(true));
    ui->stackedWidget->setCurrentIndex(2);
}

// new vault page ///////////////////////////////////////////////////////
void MainWindow::initNewVaultPage(){
    qDebug() << "initializing new vault page";
    // stylesheet
    ui->vault_new_name_label->setStyleSheet("QWidget{color:rgb(100,100,100);}");
    ui->vault_new_name_lineEdit->setStyleSheet("QWidget{color:rgb(100,100,100);}");
    ui->vault_new_password_label->setStyleSheet("QWidget{color:rgb(100,100,100);}");
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
    ui->vault_new_name_label->setProperty(PROPERTY_BOOL, QVariant(false));
    ui->vault_new_password_label->setProperty(PROPERTY_BOOL, QVariant(false));
    ui->vault_new_password_confirm_label->setProperty(PROPERTY_BOOL, QVariant(false));
}
//// set confirm state
void MainWindow::setCreateButton(){
    bool bool1 = ui->vault_new_name_label->property(PROPERTY_BOOL).toBool();
    bool bool2 = ui->vault_new_password_label->property(PROPERTY_BOOL).toBool();
    bool bool3 = ui->vault_new_password_confirm_label->property(PROPERTY_BOOL).toBool();
    // !ui->vault_new_createVault_button->isEnabled()
    if (bool1 && bool2 && bool3)
        ui->vault_new_createVault_button->setEnabled(true);
    else if (ui->vault_new_createVault_button->isEnabled())
        ui->vault_new_createVault_button->setEnabled(false);
}
void MainWindow::setPasswordLabel(){
    bool labelBool = ui->vault_new_password_label->property(PROPERTY_BOOL).toBool();
    bool pw_condition_confirmed;
    int textSize = ui->vault_new_password_lineEdit->text().size();
    if(textSize < 4) pw_condition_confirmed = false;
    else pw_condition_confirmed = true;
    if (pw_condition_confirmed && !labelBool){
        ui->vault_new_password_label->setStyleSheet("QWidget{color: rgb(100, 255, 100);}");
        ui->vault_new_password_label->setProperty(PROPERTY_BOOL, QVariant(true));
    }
    else if (!pw_condition_confirmed && labelBool){
        ui->vault_new_password_label->setStyleSheet("QWidget{color: rgb(255, 100, 100);}");
        ui->vault_new_password_label->setProperty(PROPERTY_BOOL, QVariant(false));
    }
}
void MainWindow::setPasswordConfirmLabel(){
    bool labelBool = ui->vault_new_password_confirm_label->property(PROPERTY_BOOL).toBool();
    QString password = ui->vault_new_password_lineEdit->text();
    QString password_confirm = ui->vault_new_password_confirm_lineEdit->text();
    if (password.size() >= 4 && password == password_confirm && !labelBool){
        if (!labelBool){
            ui->vault_new_password_confirm_label->setStyleSheet("QWidget{color: rgb(100, 255, 100);}");
            ui->vault_new_password_confirm_label->setProperty(PROPERTY_BOOL, QVariant(true));
        }
    }else{
        if (labelBool){
            ui->vault_new_password_confirm_label->setStyleSheet("QWidget{color: rgb(255, 100, 100);}");
            ui->vault_new_password_confirm_label->setProperty(PROPERTY_BOOL, QVariant(false));
        }
    }
}
//// directory
void MainWindow::on_vault_new_openFolder_button_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Folder", QDir::rootPath(), QFileDialog::ShowDirsOnly);
    if (dir.isEmpty()) return;
    // set property
    ui->vault_new_path_label->setProperty(PROPERTY_BOOL, QVariant(dir));
    // read property
    if (!ui->vault_new_name_label->property(PROPERTY_BOOL).toBool()){
        ui->vault_new_path_label->setToolTip(dir + "/");
        ui->vault_new_path_label->setText(dir + "/");
        ui->vault_new_name_label->setStyleSheet(R"(QWidget{ color: rgb(255, 100, 100);})");
        ui->vault_new_name_lineEdit->setEnabled(true);
    }else{
        // tooltip
        ui->vault_new_path_label->setToolTip(dir);
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
    QString str = ui->vault_new_path_label->property(PROPERTY_BOOL).toString() + "/" + arg1;
    int maxWidth = ui->vault_new_path_label->width();
    QFontMetrics metrics(ui->vault_new_path_label->font());
    QString elidedStr = metrics.elidedText(str, Qt::ElideLeft, maxWidth);
    ui->vault_new_path_label->setText(elidedStr);
    // set tooltip
    ui->vault_new_path_label->setToolTip(str);

    // check
    bool nameBool = ui->vault_new_name_label->property(PROPERTY_BOOL).toBool();
    bool condition_confirmed;
    if(arg1.size() < 1) condition_confirmed = false;
    else condition_confirmed = true && !fs::exists(str.toStdWString());

    if (condition_confirmed && !nameBool){
        ui->vault_new_name_label->setStyleSheet("QWidget{color: rgb(100, 255, 100);}");
        ui->vault_new_name_label->setProperty(PROPERTY_BOOL, QVariant(true));
    }else if (!condition_confirmed && nameBool){
        ui->vault_new_name_label->setStyleSheet("QWidget{color: rgb(255, 100, 100);}");
        ui->vault_new_name_label->setProperty(PROPERTY_BOOL, QVariant(false));
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
    Vault_ComboBox_LoadVaults();
}

// crypto page /////////////////////////////////////////////////////////


void MainWindow::InitCryptoPage(){
    // setenable
    ui->vault_select_comboBox->setEnabled(false);
    ui->vault_createExisting_button->setEnabled(false);
    ui->vault_createNew_button->setEnabled(false);
    ui->password_edit_lineedit->setEnabled(false);
    ui->crypto_decrypt_button->setEnabled(false);
    ui->crypto_encrypt_button->setEnabled(false);
    ui->crypto_suspend_button->setEnabled(false);

    // clear ui
    ui->fileviewer_listView->setModel(nullptr);
    ui->vault_directory_path_label->setText("");
    ui->vault_edit_lineEdit->setText("");
    ui->password_edit_lineedit->setText("");
    ui->progressBar->setTextVisible(false);
    ui->progressBar->setValue(0);

    // clear data
    current_directory_files.clear();

    // init watcher
    QStringList list = watcher->directories();
    if (list.size()){
        watcher->removePaths(list);
        watcher->disconnect();
    }
}
void MainWindow::LoadCryptoPage(){
    ui->vault_directory_path_label->setText(QString::fromStdWString(current_vault.directory));
    ui->vault_edit_lineEdit->setText(ui->vault_select_comboBox->currentText());
    QTimer::singleShot(0, this, &MainWindow::refreshCryptoPage);
}
void MainWindow::refreshCryptoPage(){
    QThread *thread = QThread::create([this](){
        clock_t start, end;
        QString outputText;
        // search files;
        QMetaObject::invokeMethod(this, "appendToTerminal", Qt::QueuedConnection, Q_ARG(QString, "searching files... please wait"));
        start = clock();
        current_directory_files.clear();
        FILE_STRUCT fStruct;
        for (const auto& file : fs::recursive_directory_iterator(current_vault.directory)){
            if (file.is_regular_file()){
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
        end = clock();
        outputText = "found <font color=#64ff64>" + QString::number(current_directory_files.size()) + "</font> files, elapsed time: " + QString::number(end - start) + "ms";
        QMetaObject::invokeMethod(this, "appendToTerminal", Qt::QueuedConnection, Q_ARG(QString, outputText));

        // sort
        QMetaObject::invokeMethod(this, "appendToTerminal", Qt::QueuedConnection, Q_ARG(QString, "sorting files... please wait"));
        QApplication::processEvents();
        start = clock();

        sort(current_directory_files.begin(), current_directory_files.end(), [](const FILE_STRUCT& a, const FILE_STRUCT& b){
            return a.relativePath < b.relativePath;
        });

        end = clock();
        outputText = "sorting end!, elapsed time: " + QString::number(end - start) + "ms";
        QMetaObject::invokeMethod(this, "appendToTerminal", Qt::QueuedConnection, Q_ARG(QString, outputText));
        QApplication::processEvents();

        // add to fileviewer
        QMetaObject::invokeMethod(this, "appendToTerminal", Qt::QueuedConnection, Q_ARG(QString, "adding to file viewer... please wait"));
        QApplication::processEvents();
        start = clock();

        QStandardItemModel *model = new QStandardItemModel();

        QStandardItem *item;
        QBrush redBrush(QColor(255, 100, 100));
        QBrush greenBrush(QColor(100, 255, 100));
        for (const auto& file : current_directory_files){
            item = new QStandardItem(file.relativePath);
            item->setForeground(file.encrypted ? greenBrush : redBrush);
            model->appendRow(item);
        }

        QMetaObject::invokeMethod(this, "setFileViewerModel", Qt::QueuedConnection, Q_ARG(QStandardItemModel*, model));

        end = clock();
        outputText = "added to file viewer";
        QMetaObject::invokeMethod(this, "appendToTerminal", Qt::QueuedConnection, Q_ARG(QString, outputText));
    });
    thread->start();
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
    ui->vault_select_comboBox->setEnabled(false);
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
    ui->vault_select_comboBox->setEnabled(false);
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



