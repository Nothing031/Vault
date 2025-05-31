#include "FileInfoTooltipWidget.hpp"


#include <QWidget>
#include <QThread>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "WrappedLabel.hpp"

#include "src/core/fileinfo/FileInfoLoader.hpp"

// TODO : setdata bug fix

FileInfoTooltipWidget::FileInfoTooltipWidget(QString path, QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setStyleSheet(R"(
    QWidget{
        background-color: rgb(50, 50, 50);
        color: rgb(240, 240, 240);
        font: 10pt;
        border-radius: 3px;
        border: 1px solid rgb(70, 70, 70);
    }
    )");

    QLabel* pathLabel = CreateLeftLabel("path");
    QLabel* pathDirLabel = CreateRightLabel(path);
    pathDirLabel->setWordWrap(true);

    QHBoxLayout* pathLayout = new QHBoxLayout();
    pathLayout->addWidget(pathLabel);
    pathLayout->addWidget(pathDirLabel);

    baseLayout = new QVBoxLayout();
    baseLayout->setContentsMargins(10, 5, 10, 5);
    baseLayout->addLayout(pathLayout);

    QWidget* baseWidget = new QWidget();
    baseWidget->setContentsMargins(0, 0, 0, 0);
    baseWidget->setLayout(baseLayout);
    baseWidget->setMinimumWidth(440);
    baseWidget->setMaximumWidth(440);

    QVBoxLayout* bottomLayout = new QVBoxLayout();
    bottomLayout->addWidget(baseWidget);
    bottomLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(bottomLayout);

    if (path.endsWith(".enc", Qt::CaseInsensitive)){
        headerTempLayout = new QHBoxLayout();
        headerTempLayout->addWidget(CreateLeftLabel("header"));
        headerTempLayout->addWidget(CreateRightLabel("Loading..."));
        headerTempLayout->setObjectName("headerLayout");
        baseLayout->addLayout(headerTempLayout);

        // load data
        QThread* thread = QThread::create([path, this](){
            FileInfoLoader::GetInstance().LoadInfoSingle(path, (void*)this);
        });
        connect(&FileInfoLoader::GetInstance(), &FileInfoLoader::onSingleInfoLoaded, this, &FileInfoTooltipWidget::SetData);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        thread->start();
    }
}

FileInfoTooltipWidget::~FileInfoTooltipWidget()
{


}

void FileInfoTooltipWidget::SetData(FileInfo::State state, FileHeader info, void* caller)
{
    if (caller == this){
        if (headerTempLayout){
            baseLayout->removeItem(headerTempLayout);
        }
        if (state == FileInfo::CIPHER_HEADERNOTMATCH){
            // header does not match. file has been encrypted with different format
            QLabel* label = new WrappedLabel("Header does not match. file has been encrypted with different password", nullptr);
            label->setStyleSheet("border: 0px;");
            label->setWordWrap(true);
            baseLayout->addWidget(label);
        }else if (state == FileInfo::UNKNOWN_SIGNATURENOTMATCH){
            // integrity failure;
            QLabel * label = new WrappedLabel("file has been corrupted", nullptr);
            label->setStyleSheet("border: 0px;");
            label->setWordWrap(true);
            baseLayout->addWidget(label);
        }else{
            // format version
            QHBoxLayout* layout = new QHBoxLayout();
            layout->addWidget(CreateLeftLabel("version"));
            layout->addWidget(CreateRightLabel(QString(info.version)));
            baseLayout->addLayout(layout);
            // salt
            layout = new QHBoxLayout();
            layout->addWidget(CreateLeftLabel("salt"));
            layout->addWidget(CreateRightLabel(info.salt.toBase64()));
            baseLayout->addLayout(layout);
            // iteration
            layout = new QHBoxLayout();
            layout->addWidget(CreateLeftLabel("iteration"));
            layout->addWidget(CreateRightLabel(QString::number(info.iteration)));
            baseLayout->addLayout(layout);
            // hmac
            layout = new QHBoxLayout();
            layout->addWidget(CreateLeftLabel("hmac"));
            layout->addWidget(CreateRightLabel(info.hmac.toBase64()));
            baseLayout->addLayout(layout);
            // iv
            layout = new QHBoxLayout();
            layout->addWidget(CreateLeftLabel("iv"));
            layout->addWidget(CreateRightLabel(info.iv.toBase64()));
            baseLayout->addLayout(layout);
        }
    }
    this->adjustSize();
    this->show();
}

QLabel *FileInfoTooltipWidget::CreateLeftLabel(const QString &str)
{
    QLabel* label = new QLabel(str, nullptr);
    label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    label->setStyleSheet("max-width: 60px; min-width: 60px; border: 0px");
    return label;
}

QLabel *FileInfoTooltipWidget::CreateRightLabel(const QString &str)
{
    QLabel* label = new QLabel(str, nullptr);
    label->setStyleSheet("border: 0px; color: rgb(220, 220, 220);");
    label->setWordWrap(true);
    return label;
}

void FileInfoTooltipWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    close();
}





