#pragma once
#include <QMainWindow>


#include <src/core/vault/Vault.hpp>
#include <src/models/FileListModel.hpp>
#include "src/core/cryptography/CryptoEngine.hpp"

namespace Ui{
class VaultApp_Viewer;
}

class VaultApp_Viewer : public QMainWindow
{
    Q_OBJECT
public:
    explicit VaultApp_Viewer(std::shared_ptr<Vault>);
    ~VaultApp_Viewer();

private slots:
    void UpdateButton(int plains, int ciphers);
    void OpenSettings();
    void TryUnlock();
    void Encrypt();
    void Decrypt();
    void ProcessCryptoEngineMessage(CryptoEngine::Event event, QVariant param);
    void RefreshVaultFiles();

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void requestShowEntryWindow();

private:
    Ui::VaultApp_Viewer *ui;
    std::shared_ptr<Vault> vault;
};

