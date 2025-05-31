#pragma once

#include <QWidget>
#include <QHBoxLayout>

#include "src/core/fileinfo/FileHeader.hpp"
#include "WrappedLabel.hpp"

class FileInfoTooltipWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FileInfoTooltipWidget(QString path ,QWidget *parent = nullptr);
    ~FileInfoTooltipWidget();

private slots:
    void SetData(FileInfo::State state, FileHeader info, void* caller);

private:
    QLabel* CreateLeftLabel(const QString& str);
    QLabel* CreateRightLabel(const QString& str);

protected:
    void leaveEvent(QEvent* event) override;

signals:

private:
    QVBoxLayout *baseLayout = nullptr;
    QHBoxLayout *headerTempLayout = nullptr;
};
