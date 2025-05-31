#pragma once

#include <QMainWindow>
#include <QVector>
#include <QPoint>
#include <QPair>

#include "src/core/vault/Vault.hpp"
#include "src/widgets/VaultTitle.hpp"
#include "src/widgets/VaultCreateNew.hpp"

namespace Ui{
class VaultApp;
}

class VaultApp : public QMainWindow
{
    Q_OBJECT
public:
    explicit VaultApp(QWidget *parent = nullptr);
    ~VaultApp();

private slots:
    void OpenVault(std::shared_ptr<Vault> vault);
    void StartCreateNewVault();
    void OpenFolder();

protected:
    void closeEvent(QCloseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

signals:


private:
    Ui::VaultApp *ui;
    VaultTitle *vaultTitleWidget;
    VaultCreateNew *vaultCreateNewWidget;


    QVector<QPair<QMainWindow*, std::shared_ptr<Vault>>> childWindows;

    QPoint m_dragStartPos;
};

