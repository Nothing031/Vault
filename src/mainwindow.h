#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "vault.h"
#include <mutex>
#include <vector>
#include <string>


using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_vault_select_comboBox_currentIndexChanged(int index);
    void on_vault_createExisting_button_clicked();
    void on_vault_createNew_button_clicked();

    // page
    void on_vault_new_openFolder_button_clicked();
    void on_vault_new_name_lineEdit_textEdited(const QString &arg1);
    void on_vault_new_password_lineEdit_textEdited(const QString &arg1);
    void on_vault_new_password_confirm_lineEdit_textEdited(const QString &arg1);

    void on_vault_new_password_visible_button_toggled(bool checked);

    void on_vault_new_password_confirm_visible_button_toggled(bool checked);

    void on_vault_new_createVault_button_clicked();

    void on_password_edit_lineedit_editingFinished();

    void on_password_visible_button_toggled(bool checked);

    void on_vault_select_comboBox_activated(int index);

    void on_vault_openFolder_button_clicked();

private:


    Ui::MainWindow *ui;

    VAULT_STRUCT current_vault;
    vector<VAULT_STRUCT> current_vaults;

    mutex vecMutex;
    vector<fs::path> current_directory_files;
    vector<fs::path> current_directory_encrypted_files;
    vector<fs::path> current_directory_decrypted_files;


    void setCreateButton();
    void setPasswordLabel();
    void setPasswordConfirmLabel();
    void initNewVaultPage();
    void InitCryptoPage();
};
#endif // MAINWINDOW_H
