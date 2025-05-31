#include "FileInfoTooltipWidget.hpp"


#include <QWidget>
#include <QThread>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "src/core/fileinfo/FileInfoLoader.hpp"

// this may prints "QWindowsWindow::setGeometry: Unable to set geometry" error
// you can fix this with set minimumHeight enough
// but it makes ui horrible
// let me know when you find a way to fix this shit
// im just gonna ignore this print shit
// who cares?

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

    // base
    QVBoxLayout* bottomLayout = new QVBoxLayout();
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(bottomLayout);

    baseWidget = new QWidget(this);
    baseWidget->setContentsMargins(0, 0, 0, 0);
    baseWidget->setMinimumWidth(440);
    // baseWidget->setMaximumWidth(440);

    baseWidget->setLayout(new QVBoxLayout());
    baseWidget->layout()->setContentsMargins(10, 5, 10, 5);
    layout()->addWidget(baseWidget);

    // path label
    QHBoxLayout* pathLayout = new QHBoxLayout();
    QLabel* pathLabel = CreateLeftLabel("path", baseWidget);
    QLabel* pathDirLabel = CreateRightLabel(path, baseWidget);
    pathLayout->addWidget(pathLabel);
    pathLayout->addWidget(pathDirLabel);
    baseWidget->layout()->addItem(pathLayout);

    if (path.endsWith(".enc", Qt::CaseInsensitive)){
        tempLoadingLabel = CreateRightLabel("Loading header...", baseWidget);
        baseWidget->layout()->addWidget(tempLoadingLabel);

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
        if (tempLoadingLabel){
            baseWidget->layout()->removeWidget(tempLoadingLabel);
            tempLoadingLabel->deleteLater();
            tempLoadingLabel = nullptr;
        }
        QVBoxLayout* baseLayout = qobject_cast<QVBoxLayout*>(baseWidget->layout());
        if (state == FileInfo::CIPHER_HEADERNOTMATCH){
            // header does not match. file has been encrypted with different format
            QLabel* label = CreateRightLabel("Header does not match. file has been encrypted with different password", baseWidget);
            baseLayout->addWidget(label);
        }else if (state == FileInfo::UNKNOWN_SIGNATURENOTMATCH){
            // integrity failure;
            QLabel* label = CreateRightLabel("file has been corrupted", baseWidget);
            baseLayout->addWidget(label);
        }else{
            // format version
            QHBoxLayout* layout = new QHBoxLayout();
            layout->setSpacing(0);
            layout->addWidget(CreateLeftLabel("version", baseWidget));
            layout->addWidget(CreateRightLabel(QString(info.version), baseWidget));
            baseLayout->addLayout(layout);
            // salt
            layout = new QHBoxLayout();
            layout->setSpacing(0);
            layout->addWidget(CreateLeftLabel("salt", baseWidget));
            layout->addWidget(CreateRightLabel(info.salt.toBase64(), baseWidget));
            baseLayout->addLayout(layout);
            // iteration
            layout = new QHBoxLayout();
            layout->setSpacing(0);
            layout->addWidget(CreateLeftLabel("iteration", baseWidget));
            layout->addWidget(CreateRightLabel(QString::number(info.iteration), baseWidget));
            baseLayout->addLayout(layout);
            // hmac
            layout = new QHBoxLayout();
            layout->setSpacing(0);
            layout->addWidget(CreateLeftLabel("hmac", baseWidget));
            layout->addWidget(CreateRightLabel(info.hmac.toBase64(), baseWidget));
            baseLayout->addLayout(layout);
            // iv
            layout = new QHBoxLayout();
            layout->setSpacing(0);
            layout->addWidget(CreateLeftLabel("iv", baseWidget));
            layout->addWidget(CreateRightLabel(info.iv.toBase64(), baseWidget));
            baseLayout->addLayout(layout);
        }
    }
}

QLabel *FileInfoTooltipWidget::CreateLeftLabel(const QString &str, QWidget* parent)
{
    QLabel* label = new QLabel(str, parent);
    label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    label->setStyleSheet("max-width: 60px; min-width: 60px; border: 0px");
    return label;
}

QLabel *FileInfoTooltipWidget::CreateRightLabel(const QString &str, QWidget* parent)
{
    QLabel* label = new QLabel(str, parent);
    //label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    label->setStyleSheet("border: 0px; color: rgb(220, 220, 220);");
    label->setWordWrap(true);
    return label;
}

void FileInfoTooltipWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    close();
}





