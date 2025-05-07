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
#include "filemetadata.hpp"

class FileListModel : public QAbstractListModel {
    Q_OBJECT

private:
    Vault* pVault = nullptr;

public slots:
    void reset(){
        beginResetModel();
        this->pVault = nullptr;
        endResetModel();
    }

public:
    FileListModel(QObject *parent = nullptr) : QAbstractListModel(parent) {

    }

    void update(){
        beginResetModel();
        endResetModel();
    }

    void setVault(Vault* pVault){
        beginResetModel();
        this->pVault = pVault;
        endResetModel();
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return pVault ? pVault->files.size() : 0;
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= pVault->files.size() || !pVault)
            return QVariant();

        if (role == Qt::DisplayRole) {
            return pVault->files.at(index.row())->path.displayPath;
        }

        else if (role == Qt::ForegroundRole){
            const FileMetadata* file = pVault->files.at(index.row());
            if (file == nullptr){
                qDebug() << "nullptr";
                return QVariant();
            }
            if (file->path.state == FileMetadata::PlainData){
                return QBrush(QColor(200, 200, 200));
            }else if (file->path.state == FileMetadata::CipherData){
                return QBrush(QColor(55, 255, 55));
            }
        }

        return QVariant();
    }
};

#endif // FILELISTMODEL_HPP
