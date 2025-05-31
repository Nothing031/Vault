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
    QLabel* CreateLeftLabel(const QString& str, QWidget* parent);
    QLabel* CreateRightLabel(const QString& str, QWidget* parent);

protected:
    void leaveEvent(QEvent* event) override;

signals:

private:
    QWidget *baseWidget = nullptr;
    QLabel *tempLoadingLabel = nullptr;
};
