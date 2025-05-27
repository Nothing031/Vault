#pragma once

#include <QListView>

class FileListView : public QListView
{
    Q_OBJECT
public:
    enum SelectionMode{
        Single,
        Multi,
        None,
    };
    explicit FileListView(QWidget* parent = nullptr);

signals:
    void onSelectionChange(SelectionMode mode, int count);

protected:
    bool event(QEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

};

