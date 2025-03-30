#ifndef FRONTENDNEWVAULT_H
#define FRONTENDNEWVAULT_H

#include <QWidget>
#include <QDir>
#include "src/vault.hpp"

enum class Mode{
    Local,
    Portable
};

namespace Ui {
class FrontendNewVault;
}

class FrontendNewVault : public QWidget
{
    Q_OBJECT

public:
    explicit FrontendNewVault(QWidget *parent = nullptr);
    ~FrontendNewVault();

    void init(const Mode& mode);

    void on_request_page(const Mode& mode);

private slots:


    void on_directory_open_button_clicked();

    void on_password_visibility_button_toggled(bool checked);

    void on_confirm_visibility_button_toggled(bool checked);

    void on_confirm_input_lineedit_textEdited(const QString &arg1);

    void on_password_input_lineedit_textEdited(const QString &arg1);

    void on_vault_name_lineedit_textEdited(const QString &arg1);

    void on_vault_create_clicked();



    void on_VaultNameLineedit_textEdited(const QString &arg1);

    void on_PasswordLineedit_textEdited(const QString &arg1);

    void on_PwConfirmLineedit_textEdited(const QString &arg1);

    void on_PasswordVisibilityButton_toggled(bool checked);

    void on_DirectoryOpenButton_clicked();

    void on_PwConfirmVisibilityButton_toggled(bool checked);

signals:
    void signal_create_local_vault(const Vault& vault);
    void signal_create_portable_vault(const Vault& vault);

private:
    void ConditionCheck();

    QDir directory = QDir();
    bool conditionPath = false;
    bool conditionPassword = false;
    bool conditionConfirm = false;
    Mode mode = Mode::Local;

    Ui::FrontendNewVault *ui;
};

#endif // FRONTENDNEWVAULT_H
