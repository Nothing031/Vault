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
    void OpenVault(Vault* vault);
    void StartCreateNewVault();
    void OpenFolder();

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

