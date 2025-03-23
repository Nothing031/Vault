#ifndef FILELISTMODEL_HPP
#define FILELISTMODEL_HPP

#include <QObject>
#include <QAbstractListModel>
#include <QVector>
#include <QBrush>
#include <QColor>
#include <QFile>
#include <QFileInfo>

#include "vault.hpp"
#include "file_t.hpp"

namespace fs = std::filesystem;

#include "file_t.hpp"

class FileListModel : public QAbstractListModel {
    Q_OBJECT

private:
    QVector<file_t*> items;

public:
    FileListModel(QObject *parent = nullptr) : QAbstractListModel(parent) {
        qRegisterMetaType<file_t*>("file_t*");
    }

    void setData(Vault& vault){
        beginResetModel();

        items.clear();
        for (auto& file : vault.files){
            items.append(&file);
        }

        endResetModel();
    }


    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return items.size();
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= items.size())
            return QVariant();

        if (role == Qt::DisplayRole) {
            return items.at(index.row())->displayPath;
        }
        else if (role == Qt::ForegroundRole){
            const file_t* file = items.at(index.row());
            if (file->state == file_t::PlainData){
                return QBrush(QColor(255, 55 ,55));
            }else if (file->state == file_t::CipherData){
                return QBrush(QColor(55, 255 ,55));
            }
        }
        else if (role == Qt::UserRole){
            QVariant::fromValue(&items.at(index.row()));
        }
        return QVariant();
    }
};



#endif // FILELISTMODEL_HPP
