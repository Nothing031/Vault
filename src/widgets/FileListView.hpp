#pragma once

#include <QListView>
#include <QPair>
#include "src/core/vault/Vault.hpp"
#include "src/models/FileListModel.hpp"


class FileListView : public QListView
{
    Q_OBJECT
public:
    explicit FileListView(QWidget* parent = nullptr);

signals:
    void onSelectionChange(int plains, int ciphers);

public:
    //QVector<FileInfo*> SelectedFiles();
    void SetModelVault(Vault* vault);
    QVector<FileInfo*> GetSelectedFiles();

private:
    //void setModel(QAbstractListModel* model);

protected:
    bool event(QEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    FileListModel* model;
    QPair<int, int> getSelectionSizes();

};

