#pragma once

#include <QWidget>
#include <QCheckBox>

#include "src/core/cryptography/AES256Settings.hpp"
#include "src/core/fileinfo/FileHeader.hpp"

namespace Ui{
class PasswordWidget;
}


class PasswordWidget : public QWidget
{
    Q_OBJECT
public:
    struct Preset{
        bool itrEnabled = false;
        int itr = 0;
        bool saltEnabled = false;
        QByteArray salt;
        bool hmacEnabled = false;
        QByteArray hmac;
    };

    explicit PasswordWidget(QWidget *parent = nullptr);
    ~PasswordWidget();
    void init();
    AES256Settings GetAESSettings();

public slots:
    void setEnabled(bool b);

private slots:
    void CheckValidation();

    // advanced
    void SetAcvancedOptionsEnabled(Qt::CheckState checked);
    void ResetIteration();
    // preset
    void TryCopyFromFile();
    void ResetPresetCopy();
    // options
    void SetPresetIterationEnabled(Qt::CheckState checked);
    void SetPresetSaltEnabled(Qt::CheckState checked);
    void SetPresetHmacEnabled(Qt::CheckState checked);

signals:
    void onValidationChange(bool valid);

private:
    Ui::PasswordWidget *ui;

    Preset preset;

    bool lastValidation = false;

    static QString m_styleRed;
    static QString m_styleGrean;
    static QString m_styleLightGray;
    static QString m_styleGray;
    static QString m_styleDarkGray;
    static QString m_styleWhite;
};
