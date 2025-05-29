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

    QLabel* pathLabel = new QLabel("path");
    pathLabel->setStyleSheet("max-width: 60px; min-width: 60px; border: 0px");
    pathLabel->setAlignment(Qt::AlignTop);
    QLabel* pathDirLabel = new QLabel(path);
    pathDirLabel->setStyleSheet("border: 0px;");
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
    baseWidget->setMinimumWidth(300);
    baseWidget->setMaximumWidth(300);

    QVBoxLayout* bottomLayout = new QVBoxLayout();
    bottomLayout->addWidget(baseWidget);
    bottomLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(bottomLayout);

    if (path.endsWith(".enc", Qt::CaseInsensitive)){
        QLabel* headerLabel = new QLabel("header");
        headerLabel->setStyleSheet("max-width: 60px; min-width: 60px; border: 0px");
        headerLabel->setAlignment(Qt::AlignTop);
        QLabel* loadingLabel = new QLabel("Loading...");
        loadingLabel->setStyleSheet("border: 0px;");
        headerTempLayout = new QHBoxLayout();
        headerTempLayout->addWidget(headerLabel);
        headerTempLayout->addWidget(loadingLabel);
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
            qDebug() << "deleting layout";
            delete headerTempLayout;
        }
        if (state == FileInfo::CIPHER_HEADERNOTMATCH){
            // header does not match. file has been encrypted with different format
            QLabel* label = new WrappedLabel("Header does not match. file has been encrypted with different password");
            label->setStyleSheet("border: 0px;");
            label->setWordWrap(true);
            baseLayout->addWidget(label);
        }else if (state == FileInfo::UNKNOWN_SIGNATURENOTMATCH){
            // integrity failure;
            QLabel * label = new WrappedLabel("file has been corrupted");
            label->setStyleSheet("border: 0px;");
            label->setWordWrap(true);
            baseLayout->addWidget(label);
        }else{
            QLabel* label = new QLabel("version");
            QLabel* infoLabel = new QLabel(QString(info.version));
            QHBoxLayout* layout = new QHBoxLayout();
            // format version
            label->setStyleSheet("border: 0px");
            infoLabel->setStyleSheet("border: 0px");
            layout->addWidget(label);
            layout->addWidget(infoLabel);
            baseLayout->addLayout(layout);
            // salt
            label = new QLabel("salt");
            infoLabel = new QLabel(info.salt.toBase64());
            label->setStyleSheet("border: 0px");
            infoLabel->setStyleSheet("border: 0px");
            layout->addWidget(label);
            layout->addWidget(infoLabel);
            baseLayout->addLayout(layout);
            // iteration
            label = new QLabel("iterations");
            infoLabel = new QLabel(QString::number(info.iteration));
            label->setStyleSheet("border: 0px");
            infoLabel->setStyleSheet("border: 0px");
            layout->addWidget(label);
            layout->addWidget(infoLabel);
            baseLayout->addLayout(layout);
            // hmac
            label = new QLabel("hmac");
            infoLabel = new QLabel(info.hmac.toBase64());
            label->setStyleSheet("border: 0px");
            infoLabel->setStyleSheet("border: 0px");
            layout->addWidget(label);
            layout->addWidget(infoLabel);
            baseLayout->addLayout(layout);
            // iv
            label = new QLabel("iv");
            infoLabel = new QLabel(info.iv.toBase64());
            label->setStyleSheet("border: 0px");
            infoLabel->setStyleSheet("border: 0px");
            layout->addWidget(label);
            layout->addWidget(infoLabel);
            baseLayout->addLayout(layout);
        }
    }
}

void FileInfoTooltipWidget::leaveEvent(QEvent *event)
{
    close();
}





