#pragma once

#include <QWidget>

namespace Ui{
class VaultCreateNew;
}

class VaultCreateNew : public QWidget
{
    Q_OBJECT
public:
    enum UiOption{
        Path = 0x01,
        EncryptionOption = 0x02,
        Init = 0x04
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
