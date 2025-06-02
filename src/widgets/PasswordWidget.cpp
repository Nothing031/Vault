#include "PasswordWidget.hpp"
#include "ui_PasswordWidget.h"

#include <QValidator>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

#include "src/core/fileinfo/FileHeader.hpp"

QString PasswordWidget::m_styleRed          = R"( QWidget{ color: rgb(255, 55, 55); } )";
QString PasswordWidget::m_styleGrean        = R"( QWidget{ color: rgb(55, 255, 55); } )";
QString PasswordWidget::m_styleLightGray    = R"( QWidget{ color: rgb(200, 200, 200); } )";
QString PasswordWidget::m_styleGray         = R"( QWidget{ color: rgb(150, 150, 150); } )";
QString PasswordWidget::m_styleDarkGray     = R"( QWidget{ color: rgb(100, 100, 100); } )";
QString PasswordWidget::m_styleWhite        = R"( QWidget{ color: rgb(255, 255, 255); } )";




PasswordWidget::PasswordWidget(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::PasswordWidget)
{
    ui->setupUi(this);

    QRegularExpression passwordReg(R"(^[A-Za-z0-9!@#$%^&*()_+\-=\[\]{};':\"\\|,.<>\\/?]*$)");
    QRegularExpressionValidator *passwordValidator = new QRegularExpressionValidator(passwordReg, this);
    QIntValidator *iterationValidator = new QIntValidator(1, 2000000, this);
    ui->passwordLineEdit->setValidator(passwordValidator);
    ui->confirmLineEdit->setValidator(passwordValidator);
    ui->iterationLineEdit->setValidator(iterationValidator);


    // password
    connect(ui->passwordLineEdit, &QLineEdit::textEdited, this, &PasswordWidget::CheckValidation);
    connect(ui->confirmLineEdit, &QLineEdit::textEdited, this, &PasswordWidget::CheckValidation);
    connect(ui->passwordVisibilityButton, &QPushButton::toggled, this, [this](bool b){
        ui->passwordLineEdit->setEchoMode(b ? QLineEdit::Normal : QLineEdit::Password);
    });
    connect(ui->confirmVisibilityButton, &QPushButton::toggled, this, [this](bool b){
        ui->confirmLineEdit->setEchoMode(b ? QLineEdit::Normal : QLineEdit::Password);
    });

    //--- advanced
    connect(ui->advancedOptionsCheckbox, &QCheckBox::checkStateChanged, this, &PasswordWidget::SetAcvancedOptionsEnabled);
    connect(ui->iterationResetButton, &QPushButton::clicked, this, &PasswordWidget::ResetIteration);

    //------ preset
    connect(ui->preset_copyButton, &QPushButton::clicked, this, &PasswordWidget::TryCopyFromFile);
    connect(ui->preset_resetButton, &QPushButton::clicked, this, &PasswordWidget::ResetPresetCopy);
    connect(ui->preset_iterationCheckbox, &QCheckBox::checkStateChanged, this, &PasswordWidget::SetPresetIterationEnabled);
    connect(ui->preset_saltCheckbox, &QCheckBox::checkStateChanged, this, &PasswordWidget::SetPresetSaltEnabled);
    connect(ui->preset_hmacCheckbox, &QCheckBox::checkStateChanged, this, &PasswordWidget::SetPresetHmacEnabled);
    connect(ui->preset_hmacCheckbox, &QCheckBox::checkStateChanged, this, &PasswordWidget::CheckValidation);
}

PasswordWidget::~PasswordWidget()
{
    delete ui;
}

void PasswordWidget::CheckValidation()
{
    QString password = ui->passwordLineEdit->text();
    QString confirm  = ui->confirmLineEdit->text();
    bool pwValid = password.size() >= 4 ? true : false;
    bool confirmValid = password == confirm ? true : false;
    // stylesheet
    ui->passwordInfoLabel->setStyleSheet(pwValid ? m_styleLightGray : m_styleRed);
    // show aditional label
    if (confirmValid && ui->confirmInfoLabel->sizeHint().height() == ui->confirmInfoLabel->size().height()){
        ui->confirmInfoLabel->setMaximumHeight(0);
    }else if (!confirmValid && ui->confirmInfoLabel->sizeHint().height() != ui->confirmInfoLabel->size().height()){
        ui->confirmInfoLabel->setMaximumHeight(sizeHint().height());
    }
    bool isPasswordValid =  pwValid && confirmValid;

    bool newValidation = preset.hmacEnabled ? true : isPasswordValid;
    // emit
    if (lastValidation != newValidation)
        emit onValidationChange(newValidation);
    lastValidation = newValidation;
}

void PasswordWidget::init()
{
    ui->passwordLineEdit->setEnabled(true);
    ui->confirmLineEdit->setEnabled(true);

}

void PasswordWidget::SetAcvancedOptionsEnabled(Qt::CheckState checked)
{
    bool b = checked == Qt::Checked ? true : false;

    if (b){
        ui->advancedOptionsLayoutWidget->setMaximumHeight(1000);
    }else{
        ui->advancedOptionsLayoutWidget->setMaximumHeight(0);
        ResetIteration();
        ResetPresetCopy();
    }
}

// advanced
void PasswordWidget::ResetIteration()
{
    ui->iterationLineEdit->setText(QString::number(ITERATION));
}

// preset
void PasswordWidget::TryCopyFromFile()
{
    QString path = QFileDialog::getOpenFileName(this, "Select encrypted file", QDir::rootPath(), "Enc file (*.enc)");
    QFile f(path);
    if (f.exists()){
        if (f.open(QFile::ReadOnly) ){
            if (f.size() >= FileHeader::Sizes::total){
                QByteArray data = f.read(FileHeader::Sizes::total);
                FileHeader header;
                FileInfo::State state = header.Deserialize(data);
                if (state == FileInfo::CIPHER_GOOD){
                    preset.hmac = header.hmac;
                    preset.salt = header.salt;
                    preset.itr = header.iteration;
                    ui->presetOptionsLayoutWidget->setMaximumHeight(1000);
                }else{
                    QMessageBox::warning(this, "Error", "Failed to extract preset data from file.");
                }
            }else{
                QMessageBox::warning(this, "Error", "Failed to extract preset data from file.");
            }
        }else{
            QMessageBox::warning(this, "Error", "Failed to read file.");
        }
    }
}

void PasswordWidget::ResetPresetCopy()
{
    preset.hmacEnabled = false;
    preset.itrEnabled = false;
    preset.saltEnabled = false;
    ui->preset_saltCheckbox->setCheckState(Qt::Unchecked);
    ui->preset_iterationCheckbox->setCheckState(Qt::Unchecked);
    ui->preset_hmacCheckbox->setCheckState(Qt::Unchecked);
    ui->presetOptionsLayoutWidget->setMaximumHeight(0);
}

// options
void PasswordWidget::SetPresetIterationEnabled(Qt::CheckState checked)
{
    bool b = checked == Qt::Checked ? true : false;
    if (b){
        ui->iterationLineEdit->setEnabled(false);
    }else{
        ui->iterationLineEdit->setEnabled(true);
        ui->preset_hmacCheckbox->setCheckState(Qt::Unchecked);
    }
    preset.itrEnabled = b;
}

void PasswordWidget::SetPresetSaltEnabled(Qt::CheckState checked)
{
    bool b = checked == Qt::Checked ? true : false;
    preset.saltEnabled = b;
    if (b){

    }else{
        ui->preset_hmacCheckbox->setCheckState(Qt::Unchecked);
    }
}

void PasswordWidget::SetPresetHmacEnabled(Qt::CheckState checked)
{
    bool b = checked == Qt::Checked ? true : false;
    if (b){
        ui->preset_iterationCheckbox->setCheckState(Qt::Checked);
        ui->preset_saltCheckbox->setCheckState(Qt::Checked);

        ui->passwordLineEdit->setEnabled(false);
        ui->confirmLineEdit->setEnabled(false);
    }else{
        ui->passwordLineEdit->setEnabled(true);
        ui->confirmLineEdit->setEnabled(true);
    }
    preset.hmacEnabled = b;
}


