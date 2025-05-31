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


    //QVector<FileInfo*> SelectedFiles();
    void SetModelVault(std::shared_ptr<Vault> vault);
    QVector<std::shared_ptr<FileInfo>> GetSelectedFiles();

private:
    void setModel(QAbstractListModel* model) = delete;

signals:
    void onSelectionChange(int plains, int ciphers);

protected:
    bool event(QEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    FileListModel* model;
    QPair<int, int> getSelectionSizes();
};

