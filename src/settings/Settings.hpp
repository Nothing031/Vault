#pragma once
#include <QMainWindow>

#include "src/core/vault/Vault.hpp"

namespace Ui{
class Settings;
}

class Settings : public QMainWindow
{
    Q_OBJECT
public:
    explicit Settings(std::shared_ptr<Vault> vault, QWidget *parent = nullptr);
    ~Settings();

private slots:
    void BrowseFolder();
    void CheckPathCondition();
    void TryDisableEncryption();
    void TryEnableEncryption();
    void TrySetVaultEncryptionEnabled(bool b);
    void TryChangePassword();

signals:
    void Updated();
    void Discarded();

private:
    bool pathCondition;

    std::shared_ptr<Vault> vault;
    Ui::Settings *ui;
};

