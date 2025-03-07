#ifndef WINDOW_NEWVAULT_H
#define WINDOW_NEWVAULT_H

#include <QWidget>
#include <QDir>
#include "src/vault.h"

enum class NewVault{
    CreateNew,
    CreateExisting
};

namespace Ui {
class Window_NewVault;
}

class window_newvault : public QWidget
{
    Q_OBJECT

public:
    explicit window_newvault(QWidget *parent = nullptr);
    ~window_newvault();

public slots:
    void on_request_page(const NewVault& mode);

private slots:
    void on_directory_open_button_clicked();

    void on_password_visibility_button_toggled(bool checked);

    void on_confirm_visibility_button_toggled(bool checked);

    void on_confirm_input_lineedit_textEdited(const QString &arg1);

    void on_password_input_lineedit_textEdited(const QString &arg1);

    void on_vault_name_lineedit_textEdited(const QString &arg1);

    void on_vault_create_clicked();

signals:
    void signal_create_new_vault(const Vault& vault);

private:
    void ConditionCheck();

    QDir directory = QDir();
    bool condition_path = false;
    bool condition_password = false;
    bool condition_confirm = false;

    NewVault NewVaultMode = NewVault::CreateNew;
    Ui::Window_NewVault *ui;
};

#endif // WINDOW_NEWVAULT_H
