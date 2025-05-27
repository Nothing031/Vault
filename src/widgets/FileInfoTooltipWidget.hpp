#pragma once

#include <QWidget>

class FileInfoTooltipWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FileInfoTooltipWidget(QWidget *parent = nullptr);
    ~FileInfoTooltipWidget();




protected:
    void leaveEvent(QEvent* event) override;

signals:



private:
    bool **flagPtr = nullptr;
    QThread *thread = nullptr;
    void loadInfoAsync();
};
