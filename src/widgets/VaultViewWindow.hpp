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

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:


private:
    Ui::VaultViewWindow *ui;
    FileListModel *fileListModel;
    Vault* vault;

};

