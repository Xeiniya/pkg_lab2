#include "widget.h"
#include "ui_widget.h"
#include <QElapsedTimer>

using namespace std;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    QPalette f = this->palette();
    f.setColor(QPalette::Window, QColor(252, 251, 224));
    this->setAutoFillBackground(true);
    this->setPalette(f);

    ui->setupUi(this);
    model = new QFileSystemModel(this);
    model->setFilter(QDir::AllEntries);
    model->setRootPath(QDir::rootPath());
    ui->listView->setModel(model);
    ui->lineEdit->setText(" ");
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_listView_doubleClicked(const QModelIndex &index)
{
    QListView* listView = (QListView*)sender();
    QFileInfo fileInfo = model->fileInfo(index);
    ui->lineEdit->setText(model->filePath(index));
    if (fileInfo.fileName() == "..")
    {
        QDir dir = fileInfo.dir();
        dir.cdUp();
        listView->setRootIndex(model->index(dir.absolutePath()));
    }
    else if (fileInfo.fileName() == ".")
    {
        listView->setRootIndex(model->index(QDir::rootPath()));
    }
    else if (fileInfo.isDir())
    {
        listView->setRootIndex(index);
    }
    else if (!fileInfo.isDir())
    {
        displayImageInfo(index, fileInfo);
    }
}

void Widget::displayImageInfo(const QModelIndex &index, const QFileInfo &fileInfo)
{
    QImageWriter writer(model->filePath(index));
    QImage img(model->filePath(index));

    QString extension = fileInfo.suffix().toLower();

    if (extension == "jpg" || extension == "jpeg" || extension == "gif" ||
        extension == "tif" || extension == "bmp" || extension == "png" ||
        extension == "pcx")
    {
        ui->FileName->setText(fileInfo.fileName());
        ui->Size->setText(QString("%1x%2").arg(img.size().width()).arg(img.size().height()));
        ui->Contraction->setText(QString::number(writer.compression()));
        ui->Color_depth->setText(QString::number(img.bitPlaneCount()));
        ui->Resolution->setText(QString::number(img.physicalDpiX()));
    }
}

void Widget::dialogClose()
{
    this->show();
}

void Widget::on_multiChoice_clicked()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, "Выберите файлы", QDir::homePath(),
                                                          "Images (*.jpg *.jpeg *.gif *.tif *.bmp *.png *.pcx)");
    if (fileNames.isEmpty()) {
        return;
    }

    QElapsedTimer timer;
    timer.start();

    QDialog *table = new QDialog();
    QGridLayout *tableLayout = new QGridLayout(table);
    table->setMinimumSize(1000, 700);
    twInfo = new QTableWidget(table);
    tableLayout->addWidget(twInfo);
    twInfo->setColumnCount(6);
    twInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    twInfo->setRowCount(fileNames.size());
    setTableHeaders();

    for (int i = 0; i < fileNames.size(); i++)
    {
        populateTableRow(fileNames[i], i);
    }

    if (twInfo->rowCount() == 0)
    {
        return;
    }

    qint64 elapsedTime = timer.elapsed();

    int totalRow = twInfo->rowCount();
    twInfo->insertRow(totalRow);
    twInfo->setItem(totalRow, 0, new QTableWidgetItem("Общее время обработки"));
    twInfo->setItem(totalRow, 1, new QTableWidgetItem(QString::number(elapsedTime) + " мс"));


    twInfo->setSpan(totalRow, 1, 2, 5);
    twInfo->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    table->show();
    connect(table, &QDialog::rejected, this, &Widget::dialogClose);
    this->hide();
}

void Widget::on_selectFolderButton_clicked()
{

    QString directory = QFileDialog::getExistingDirectory(this, "Выберите папку", QDir::homePath());
    if (directory.isEmpty()) {
        return;
    }

    QElapsedTimer timer;
    timer.start();

    QDir dir(directory);
    QStringList filters;
    filters << "*.jpg" << "*.jpeg" << "*.gif" << "*.tif" << "*.bmp" << "*.png" << "*.pcx";
    QStringList files = dir.entryList(filters, QDir::Files | QDir::NoDotAndDotDot);

    QDialog *table = new QDialog();
    QGridLayout *tableLayout = new QGridLayout(table);
    table->setMinimumSize(1000, 700);
    twInfo = new QTableWidget(table);
    tableLayout->addWidget(twInfo);
    twInfo->setColumnCount(6);
    twInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    twInfo->setRowCount(files.size());
    setTableHeaders();

    for (int i = 0; i < files.size(); i++)
    {
        populateTableRow(dir.filePath(files[i]), i);
    }

    if (twInfo->rowCount() == 0)
    {
        return;
    }

    qint64 elapsedTime = timer.elapsed();

    int totalRow = twInfo->rowCount();
    twInfo->insertRow(totalRow);
    twInfo->setItem(totalRow, 0, new QTableWidgetItem("Общее время обработки"));
    twInfo->setItem(totalRow, 1, new QTableWidgetItem(QString::number(elapsedTime) + " мс"));
    twInfo->setSpan(totalRow, 1, 2, 5);

    table->show();
    connect(table, &QDialog::rejected, this, &Widget::dialogClose);
    this->hide();
}


void Widget::setTableHeaders()
{
    twInfo->setHorizontalHeaderItem(0, new QTableWidgetItem("Имя файла"));
    twInfo->setHorizontalHeaderItem(1, new QTableWidgetItem("Расширение"));
    twInfo->setHorizontalHeaderItem(2, new QTableWidgetItem("Размер изображения"));
    twInfo->setHorizontalHeaderItem(3, new QTableWidgetItem("Разрешение"));
    twInfo->setHorizontalHeaderItem(4, new QTableWidgetItem("Глубина цвета"));
    twInfo->setHorizontalHeaderItem(5, new QTableWidgetItem("Сжатие"));
    twInfo->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void Widget::populateTableRow(const QString &filePath, int rowIndex)
{
    QFileInfo fileInfo(filePath);
    QString extension = fileInfo.suffix().toLower();

    if (isValidImageExtension(extension)) {
        QImage img(filePath);
        QImageWriter writer(filePath);

        twInfo->setItem(rowIndex, 0, new QTableWidgetItem(fileInfo.fileName()));
        twInfo->setItem(rowIndex, 1, new QTableWidgetItem(extension));
        twInfo->setItem(rowIndex, 2, new QTableWidgetItem(QString::number(img.size().width()) + "x" + QString::number(img.size().height())));
        twInfo->setItem(rowIndex, 3, new QTableWidgetItem(QString::number(img.physicalDpiX())));
        twInfo->setItem(rowIndex, 4, new QTableWidgetItem(QString::number(img.bitPlaneCount())));
        twInfo->setItem(rowIndex, 5, new QTableWidgetItem(QString::number(writer.compression())));
    }
}


bool Widget::isValidImageExtension(const QString &extension)
{
    return extension == "jpg" || extension == "jpeg" || extension == "gif" ||
           extension == "tif" || extension == "bmp" || extension == "png" ||
           extension == "pcx";
}
