#pragma once

#include <QWidget>
#include <QHBoxLayout>

#include "src/core/fileinfo/FileHeader.hpp"

class FileInfoTooltipWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FileInfoTooltipWidget(QString path ,QWidget *parent = nullptr);
    ~FileInfoTooltipWidget();


private slots:
    void SetData(FileInfo::State state, FileHeader info, void* caller);


protected:
    void leaveEvent(QEvent* event) override;

signals:

private:
    QVBoxLayout *baseLayout = nullptr;
    QHBoxLayout *headerTempLayout = nullptr;
};
