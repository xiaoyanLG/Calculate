#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "xycalculatethread.h"
#include "xytreemodel.h"
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
    ui->treeView->header()->setStretchLastSection(false);

    connect(thread, SIGNAL(addString(QString,QString)), this, SLOT(addString(QString,QString)));
    connect(thread, SIGNAL(addSameString(QString)), this, SLOT(addSameString(QString)));
    connect(thread, SIGNAL(update()), ui->treeView, SLOT(reset()));
    connect(thread, SIGNAL(update()), this, SLOT(setColumnWidth()));
    connect(ui->treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(showTreeCurrentRow(QModelIndex)));
    connect(ui->listWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(showListCurrentRow(QModelIndex)));
    ui->treeView->setModel(new XYTreeModel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addString(const QString &one, const QString &userData)
{
    XYTreeModel::getInstance()->appendColumnData(curColumn, one, userData);
}

void MainWindow::addSameString(const QString &sameText)
{
    ui->listWidget->addItem(sameText);
}

void MainWindow::showTreeCurrentRow(const QModelIndex &index)
{
    ui->label_5->setText(QString("%1: %2").arg(QStringLiteral("当前行")).arg(index.row() + 1));
}

void MainWindow::showListCurrentRow(const QModelIndex &index)
{
    ui->label_6->setText(QString("%1: %2").arg(QStringLiteral("当前行")).arg(index.row() + 1));
}

void MainWindow::setColumnWidth()
{
    for (int i = 0; i < XYTreeModel::getInstance()->columnCount(); ++i)
    {
        if (ui->treeView->columnWidth(i) < 150)
        {
            ui->treeView->setColumnWidth(i, 150);
        }
    }
}

void MainWindow::on_pushButton_clicked()
{
    int result = ui->lineEdit_4->text().toInt();
    int maxValue = ui->lineEdit->text().toInt();
    bool include3 = ui->checkBox->isChecked();
    bool include4 = ui->checkBox_2->isChecked();
    bool include5 = ui->checkBox_3->isChecked();
    bool include6 = ui->checkBox_4->isChecked();

    if (!ui->lineEdit_4->text().isEmpty() && !ui->lineEdit->text().isEmpty())
    {
        curColumn = XYTreeModel::getInstance()->addColumn(QString("%1:%2   %3:%4").arg(QStringLiteral("结果"))
                                              .arg(result)
                                              .arg(QStringLiteral("最大值"))
                                              .arg(maxValue), QStringList(), QStringList());
        ui->treeView->setColumnWidth(curColumn, 150);
        thread->start(include3, include4, include5, include6, result, maxValue);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    XYTreeModel::getInstance()->clear();
    ui->treeView->reset();
    ui->label_5->clear();
}

void MainWindow::on_pushButton_4_clicked()
{
    int setTimes = ui->lineEdit_2->text().toInt();

    thread->start(XYTreeModel::getInstance(), setTimes);
}

void MainWindow::on_pushButton_5_clicked()
{
    ui->listWidget->clear();
    ui->label_6->clear();
}
