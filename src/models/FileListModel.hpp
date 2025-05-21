#pragma once

#include <QObject>
#include <QAbstractListModel>
#include <QVector>
#include <QBrush>
#include <QColor>
#include <QFile>
#include <QFileInfo>

#include "src/core/vault/Vault.hpp"
#include "src/core/FileInfo.hpp"

class FileListModel : public QAbstractListModel {
    Q_OBJECT

private:
    Vault* pVault = nullptr;

    inline static const QBrush m_brushDarkRed = QBrush(QColor(150, 0, 0));
    inline static const QBrush m_brushRed = QBrush(QColor(255, 55, 55));
    inline static const QBrush m_brushGray = QBrush(QColor(200, 200, 200));
    inline static const QBrush m_brushGreen = QBrush(QColor(55, 255, 55));

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

        if (role == Qt::ForegroundRole){
            const FileInfo* file = pVault->files.at(index.row());
            if (!file)                                  return QVariant();
            if (!file->integrity)                       return m_brushDarkRed;
            if (!file->isHeaderMatch)                   return m_brushRed;
            if (file->state == FileInfo::PlainData)     return m_brushGray;
            if (file->state == FileInfo::CipherData)    return m_brushGreen;
        }

        return QVariant();
    }
};

