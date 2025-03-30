#ifndef FRONTENDNEWVAULT_H
#define FRONTENDNEWVAULT_H

#include <QWidget>
#include <QDir>
#include "src/vault.hpp"

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

private slots:
    void on_VaultNameLineedit_textEdited(const QString &arg1);

    void on_PasswordLineedit_textEdited(const QString &arg1);

    void on_PwConfirmLineedit_textEdited(const QString &arg1);

    void on_PasswordVisibilityButton_toggled(bool checked);

    void on_DirectoryOpenButton_clicked();

    void on_PwConfirmVisibilityButton_toggled(bool checked);

    void on_CreateVaultButton_clicked();

signals:
    void signal_create_vault(const Vault& vault);

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
