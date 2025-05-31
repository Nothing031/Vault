#pragma once

#include <QObject>
#include <QAbstractListModel>
#include <QVector>
#include <QBrush>
#include <QColor>
#include <QFile>
#include <QFileInfo>

#include "src/core/vault/Vault.hpp"
#include "src/core/fileinfo/FileInfo.hpp"

class FileListModel : public QAbstractListModel {
    Q_OBJECT

private:
    std::shared_ptr<Vault> vault = nullptr;

    inline static const QBrush m_brushDarkRed = QBrush(QColor(150, 0, 0));
    inline static const QBrush m_brushRed = QBrush(QColor(255, 55, 55));
    inline static const QBrush m_brushGray = QBrush(QColor(200, 200, 200));
    inline static const QBrush m_brushGreen = QBrush(QColor(55, 255, 55));

public slots:
    void reset(){
        beginResetModel();
        vault = nullptr;
        endResetModel();
    }

public:
    FileListModel(QObject *parent = nullptr) : QAbstractListModel(parent) {

    }

    void update(){
        beginResetModel();
        endResetModel();
    }

    void setVault(std::shared_ptr<Vault> vault){
        beginResetModel();
        this->vault = vault;
        endResetModel();
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return vault ? vault->files.size() : 0;
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= vault->files.size() || !vault)
            return QVariant();

        if (role == Qt::DisplayRole)
            return vault->files.at(index.row())->path.displayPath;

        if (role == Qt::ForegroundRole){
            const auto& file = vault->files.at(index.row());
            if (!file)                                                  return QVariant();
            if (file->state == FileInfo::UNKNOWN_SIGNATURENOTMATCH)     return m_brushDarkRed;
            if (file->state == FileInfo::CIPHER_HEADERNOTMATCH)         return m_brushRed;
            if (file->state == FileInfo::PLAIN_GOOD)                    return m_brushGray;
            if (file->state == FileInfo::CIPHER_GOOD)                   return m_brushGreen;
        }

        if (role == Qt::UserRole){
            return QVariant::fromValue(vault->files.at(index.row()));
        }

        return QVariant();
    }
};
