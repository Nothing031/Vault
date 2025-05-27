#include "FileInfoTooltipWidget.hpp"


#include <QWidget>
#include <QThread>


FileInfoTooltipWidget::FileInfoTooltipWidget(QWidget *parent)
    : QWidget(parent)
{





    bool *flag = new bool(true);
    flagPtr = &flag;
    thread = QThread::create([this, flag](){
        // load data

        if (flag){
            // set ui

        }
    });
}

FileInfoTooltipWidget::~FileInfoTooltipWidget()
{
    **flagPtr = false;

}

void FileInfoTooltipWidget::leaveEvent(QEvent *event)
{
    close();
    this->deleteLater();
}





