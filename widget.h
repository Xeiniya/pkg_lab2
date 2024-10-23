#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFileSystemModel>
#include <QImageWriter>
#include <QTableWidget>
#include <QFileDialog>
#include <QGridLayout>
#include <QHeaderView>
#include <QImage>
#include <QListView>
#include <QPalette>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_listView_doubleClicked(const QModelIndex &index);
    void dialogClose();
    void on_multiChoice_clicked();
    void on_selectFolderButton_clicked();

private:
    Ui::Widget *ui;
    QFileSystemModel *model;
    QTableWidget *twInfo;
    int sortcolumns[6];
    std::function<bool(QTableWidgetItem*, QTableWidgetItem*)> functions[6];

    void fillSortColumns();
    void displayImageInfo(const QModelIndex &index, const QFileInfo &fileInfo);
    void setTableHeaders();
    void populateTableRow(const QString &filePath, int rowIndex);
    bool isValidImageExtension(const QString &extension);
};

#endif // WIDGET_H
