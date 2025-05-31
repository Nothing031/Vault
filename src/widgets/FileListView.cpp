#include "FilelistView.hpp"

#include <QListView>
#include <QEvent>
#include <QMouseEvent>
#include <QToolTip>
#include <QDesktopServices>
#include "src/core/fileinfo/FileInfo.hpp"
#include "src/widgets/FileInfoTooltipWidget.hpp"
#include "Menu.hpp"

FileListView::FileListView(QWidget* parent):
    QListView(parent),
    model(new FileListModel(this))
{
    QListView::setModel(model);
}

void FileListView::SetModelVault(std::shared_ptr<Vault> vault)
{
    model->setVault(vault);
}

QVector<std::shared_ptr<FileInfo>> FileListView::GetSelectedFiles()
{
    auto items = this->selectedIndexes();
    QVector<std::shared_ptr<FileInfo>> list;
    for (auto& item : items){
        auto file = item.data(Qt::UserRole).value<std::shared_ptr<FileInfo>>();
        if (file){
            list.append(file);
        }else{
            qDebug() << "GetSelectedFiles() variant conversion error, got nullptr";
        }
    }
    return list;
}

bool FileListView::event(QEvent* event){

    if (event->type() == QEvent::KeyRelease){
        QPair<int, int> sizes = getSelectionSizes();
        emit onSelectionChange(sizes.first, sizes.second);
    }

    if (event->type() == QEvent::ToolTip){
        auto* helpEvent = static_cast<QHelpEvent*>(event);
        auto item = indexAt(helpEvent->pos());
        if (item.isValid()){
            auto f = item.data(Qt::UserRole).value<std::shared_ptr<FileInfo>>();
            if (!f){
                qDebug() << "error got nullptr from QVariant to std::shared_ptr<FileInfo> conversion";
                return QListView::event(event);
            }
            FileInfoTooltipWidget* widget = new FileInfoTooltipWidget(f->path.absolutepath, nullptr);
            widget->move(helpEvent->globalPos() - QPoint(+6, +6));
            widget->show();
        }
    }
    return QListView::event(event);
}

void FileListView::mouseReleaseEvent(QMouseEvent *event)
{
    QListView::mouseReleaseEvent(event);
    QPair<int, int> sizes = getSelectionSizes();
    emit onSelectionChange(sizes.first, sizes.second);
}

void FileListView::mousePressEvent(QMouseEvent *event)
{
    QListView::mousePressEvent(event);
    auto item = indexAt(event->pos());
    if (event->button() & Qt::RightButton && item.isValid()){
        clearSelection();
        QPair<int, int> sizes = getSelectionSizes();
        emit onSelectionChange(sizes.first, sizes.second);

        Menu menu;
        QAction* openPathAction = new QAction("open", &menu);
        menu.addAction(openPathAction);

        connect(openPathAction, &QAction::triggered, this, [item](){
            auto file = (FileInfo*)item.data(Qt::UserRole).value<void*>();
            QDesktopServices::openUrl(QUrl(file->path.absolutepath));
        });

        menu.exec(event->globalPosition().toPoint());
    }
}

QPair<int, int> FileListView::getSelectionSizes()
{
    auto items = this->selectedIndexes();
    int plains = 0, ciphers = 0;
    for (auto& items : std::as_const(items)){
        auto file = items.data(Qt::UserRole).value<std::shared_ptr<FileInfo>>();
        if (!file) return QPair<int, int>(0, 0);
        if (file->state == FileInfo::PLAIN_GOOD)
            plains++;
        else
            ciphers++;
    }
    return QPair<int, int>(plains, ciphers);
}



