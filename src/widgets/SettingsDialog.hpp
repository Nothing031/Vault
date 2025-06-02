#pragma once

#include <QDialog>

#include "src/core/vault/Vault.hpp"

namespace Ui{
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    SettingsDialog(std::shared_ptr<Vault> vault , QWidget* parent);
    ~SettingsDialog();

public slots:
    void BrowseFolder();
    void CheckPath();
    bool hasDifferent();
    void TryToggleEncryption();
    void TryChangePassword();

private:
    Ui::SettingsDialog *ui;
    std::shared_ptr<Vault> vault;

    bool conditionPath;
};
