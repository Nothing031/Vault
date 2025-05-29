#pragma once
#include <QMainWindow>


#include <src/core/vault/Vault.hpp>
#include <src/models/FileListModel.hpp>
#include "src/core/cryptography/CryptoEngine.hpp"

namespace Ui{
class VaultViewWindow;
}

class VaultViewWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit VaultViewWindow(Vault *vault);
    ~VaultViewWindow();


private slots:
    void UpdateButton(int plains, int ciphers);
    void OpenSettings();
    void TryUnlock();
    void Encrypt();
    void Decrypt();
    void ProcessCryptoEngineMessage(CryptoEngine::Event event, QVariant param);

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void requestShowEntryWindow();

private:
    Ui::VaultViewWindow *ui;
    Vault* vault;
};

