#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "xycalculatethread.h"
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    thread = new XYCalculateThread(this);
    curRow = 0;
    curColumn = 1;
    ui->setupUi(this);
    ui->lineEdit->setValidator(new QIntValidator(0, 99));
    ui->lineEdit_4->setValidator(new QIntValidator(0, 99));
    ui->treeWidget->header()->setStretchLastSection(false);
    ui->treeWidget->headerItem()->setText(0, "");
    ui->treeWidget->setColumnWidth(0, 150);
    connect(thread, SIGNAL(addString(QString,QString)), this, SLOT(addString(QString,QString)));
    connect(thread, SIGNAL(addSameString(QString)), this, SLOT(addSameString(QString)));
    connect(ui->treeWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(showCurrentRow(QModelIndex)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addString(const QString &one, const QString &userData)
{
    int columns = ui->treeWidget->columnCount();
    int rows = ui->treeWidget->topLevelItemCount();
    QTreeWidgetItem *item = NULL;
    if (curColumn > columns)
    {
        ui->treeWidget->setColumnCount(curColumn);
    }
    if (curRow >= rows)
    {
        item = new QTreeWidgetItem;
        ui->treeWidget->addTopLevelItem(item);
    }
    else
    {
        item = ui->treeWidget->topLevelItem(curRow);
    }

    if (item)
    {
        item->setText(curColumn - 1, one);
        item->setData(curColumn - 1, Qt::UserRole, userData);
    }
    curRow++;
}

void MainWindow::addSameString(const QString &sameText)
{
    ui->listWidget->addItem(sameText);
}

void MainWindow::showCurrentRow(const QModelIndex &index)
{
    ui->label_5->setText(QString("%1: %2").arg(QStringLiteral("当前行")).arg(index.row() + 1));
}

void MainWindow::on_pushButton_clicked()
{
    int result = ui->lineEdit_4->text().toInt();
    int maxValue = ui->lineEdit->text().toInt();
    bool include3 = ui->checkBox->isChecked();
    bool include4 = ui->checkBox_2->isChecked();
    bool include5 = ui->checkBox_3->isChecked();
    bool include6 = ui->checkBox_4->isChecked();
    if (curColumn == ui->treeWidget->columnCount() && curRow != 0)
    {
        addOneColumn();
    }
    if (!ui->lineEdit_4->text().isEmpty() && !ui->lineEdit->text().isEmpty())
    {
        ui->treeWidget->headerItem()->setText(curColumn - 1, QString("%1:%2   %3:%4").arg(QStringLiteral("结果"))
                                                                                   .arg(result)
                                                                                   .arg(QStringLiteral("最大值"))
                                                                                   .arg(maxValue));
        thread->start(include3, include4, include5, include6, result, maxValue);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->treeWidget->clear();
    ui->treeWidget->setColumnCount(1);
    ui->treeWidget->headerItem()->setText(0, "");
    ui->treeWidget->setColumnWidth(0, 150);
    curColumn = 1;
    curRow = 0;
}

void MainWindow::addOneColumn()
{
    int columns = ui->treeWidget->columnCount();
    curRow = 0;
    curColumn++;
    if (curColumn > columns)
    {
        ui->treeWidget->setColumnCount(curColumn);
        ui->treeWidget->headerItem()->setText(curColumn - 1, "");
        ui->treeWidget->setColumnWidth(curColumn - 1, 150);
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    int setTimes = ui->lineEdit_2->text().toInt();

    thread->start(ui->treeWidget, setTimes);
}

void MainWindow::on_pushButton_5_clicked()
{
    ui->listWidget->clear();
}
