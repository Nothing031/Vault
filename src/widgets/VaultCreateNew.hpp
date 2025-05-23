#pragma once

#include <QWidget>

#include "ui_VaultCreateNew.h"

namespace Ui{
class VaultCreateNew;
}

class VaultCreateNew : public QWidget
{
    Q_OBJECT
public:
    enum UiOption{
        Path = 0x01,
        Password = 0x02,
        EncryptionOption = 0x04,
        Init = 0x08
    };

    enum VaultCreationMode{
        Create,
        Open
    };

    explicit VaultCreateNew(QWidget *parent = nullptr);
    ~VaultCreateNew();

public:
    void init();

private slots:
    void EnableEncryptionSetting(Qt::CheckState);

    void UpdatePathCondition(const QString &arg1);

    void UpdatePasswordCondition();

    void BrowseFolder();

private:
    void updateStyle(int option);

    void CheckCondition();

    void OpenAditionalEncryptionInfo();

    void CreateVault();

signals:
    void Back();


private:
    Ui::VaultCreateNew *ui;

    QString rootDirectory;

    bool isPathValid = false;
    bool isAES256EncryptionEnabled = false;
    bool isPasswordValid = false;
    bool isConfirmValid = false;

    //------------------------------
    static QString m_styleRed;
    static QString m_styleGrean;
    static QString m_styleLightGray;
    static QString m_styleGray;
    static QString m_styleDarkGray;
    static QString m_styleWhite;
};
