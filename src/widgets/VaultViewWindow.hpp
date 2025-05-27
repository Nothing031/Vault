#pragma once
#include <QMainWindow>


#include <src/core/vault/Vault.hpp>
#include <src/models/FileListModel.hpp>

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
    void OpenSettings();

    void TryUnlock();
    void Decrypt();
    void Encrypt();


protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void requestShowEntryWindow();

private:
    Ui::VaultViewWindow *ui;
    FileListModel *fileListModel;
    Vault* vault;
};

