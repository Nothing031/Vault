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
    Vault __EmptyVault;
    Vault* pVault = &__EmptyVault;

public slots:
    void reset(){
        beginResetModel();
        this->pVault = &__EmptyVault;
        endResetModel();
    }

public:
    FileListModel(QObject *parent = nullptr) : QAbstractListModel(parent) {

    }

    void update(){
        beginResetModel();
        endResetModel();
    }

    void loadVault(Vault* pVault){
        beginResetModel();
        this->pVault = pVault;
        endResetModel();
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return pVault->files.size();
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= pVault->files.size())
            return QVariant();

        if (role == Qt::DisplayRole) {
            return pVault->files.at(index.row())->info.displayPath;
        }
        else if (role == Qt::ForegroundRole){
            const FileMetadata* file = pVault->files.at(index.row());
            if (file == nullptr){
                qDebug() << "nullptr";
                return QVariant();
            }
            if (file->info.state == FileMetadata::PlainData){
                return QBrush(QColor(200, 200, 200));
            }else if (file->info.state == FileMetadata::CipherData){
                return QBrush(QColor(55, 255, 55));
            }
        }
        return QVariant();
    }
};

#endif // FILELISTMODEL_HPP
