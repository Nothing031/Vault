#pragma once
#include "ui_VaultEntryWindow.h"

#include <QMainWindow>
#include <QVector>
#include <QPoint>

#include "VaultCreateNew.hpp"
#include "VaultTitle.hpp"

#include "src/core/vault/Vault.hpp"

namespace Ui{
class VaultEntryWindow;
}

class VaultEntryWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit VaultEntryWindow(QWidget *parent = nullptr);
    ~VaultEntryWindow();


private slots:
    void AddVault(Vault* vault);
    void RemoveVault(int index);

    void OpenVault(Vault* vault);

protected:
    void closeEvent(QCloseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

signals:


private:
    Ui::VaultEntryWindow *ui;
    VaultTitle *vaultTitle;
    VaultCreateNew *vaultCreateNew;


    QVector<QMainWindow*> childWindows;

    QPoint m_dragStartPos;
};

