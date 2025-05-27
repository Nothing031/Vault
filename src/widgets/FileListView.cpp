#include "FilelistView.hpp"

#include <QListView>
#include <QEvent>
#include <QMouseEvent>


FileListView::FileListView(QWidget* parent):
    QListView(parent)
{

}

bool FileListView::event(QEvent* event){
    // if (event->type() != QEvent::Paint && event->type() != QEvent::FontChange){
    //     qDebug() << "[event] type =" << event->type();
    // }


    return QListView::event(event);
}

void FileListView::mouseReleaseEvent(QMouseEvent *event)
{
    QListView::mouseReleaseEvent(event);
    int count = this->selectedIndexes().size();
    if (count){
        if (count == 1)
            emit onSelectionChange(Single, count);
        else
            emit onSelectionChange(Multi, count);
    }else{
        emit onSelectionChange(None, count);
    }
}



