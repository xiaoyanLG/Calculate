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
    ui->lineEdit_2->setValidator(new QIntValidator(0, 99));
    ui->lineEdit_4->setValidator(new QIntValidator(0, 99));
    ui->treeView->header()->setStretchLastSection(false);

    connect(thread, SIGNAL(addString(QString,QString)), this, SLOT(addString(QString,QString)));
    connect(thread, SIGNAL(updateTree()), ui->treeView, SLOT(reset()));
    connect(thread, SIGNAL(updateList(QStringList, bool)), this, SLOT(updateListView(QStringList, bool)));
    connect(thread, SIGNAL(updateTree()), this, SLOT(setColumnWidth()));
    connect(thread, SIGNAL(finished()), this, SLOT(threadFinished()));
    connect(ui->treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(showTreeCurrentRow(QModelIndex)));
    connect(ui->listView, SIGNAL(clicked(QModelIndex)), this, SLOT(showListCurrentRow(QModelIndex)));
    ui->treeView->setModel(new XYTreeModel);
    listModel = new QStringListModel;
    ui->listView->setModel(listModel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addString(const QString &one, const QString &userData)
{
    XYTreeModel::getInstance()->appendColumnData(curColumn, one, userData);
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

void MainWindow::updateListView(QStringList datas, bool all)
{
    listModel->setStringList(datas);
    if (all)
    {
        allSameItems = datas;
    }
}

void MainWindow::threadFinished()
{
    switch (thread->getStartType())
    {
    case XYCalculateThread::FINDRESULTS:
        ui->label_7->setText(QString("%1").arg(QStringLiteral("计算完成！")));
        break;
    case XYCalculateThread::FINDSAME:
        ui->label_8->setText(QString("%1").arg(QStringLiteral("查找相同列完成！")));
        break;
    case XYCalculateThread::FINDWITHKEY:
        ui->label_8->setText(QString("%1").arg(QStringLiteral("筛选完成！")));
        break;
    default:
        break;
    }
}

void MainWindow::on_pushButton_clicked()
{
    if (thread->isRunning())
    {
        QMessageBox::information(this, QStringLiteral("别着急"), QStringLiteral("正在计算，稍等一会。。。"));
        return;
    }
    int result = ui->lineEdit_4->text().toInt();
    int maxValue = ui->lineEdit->text().toInt();
    bool include3 = ui->checkBox->isChecked();
    bool include4 = ui->checkBox_2->isChecked();
    bool include5 = ui->checkBox_3->isChecked();
    bool include6 = ui->checkBox_4->isChecked();
    bool includeBracket = ui->checkBox_5->isChecked();
    bool user = ui->checkBox_6->isChecked();

    if (user)
    {
        QString userValue = ui->lineEdit_5->text();
        QStringList userValues = userValue.split(" ", QString::SkipEmptyParts);
        QList<int > values;
        bool ok = false;
        for (int i = 0; i < userValues.size(); ++i)
        {
            int data = userValues.at(i).toInt(&ok);
            if (ok)
            {
                values.append(data);
            }
        }
        if (!values.isEmpty())
        {
            curColumn = XYTreeModel::getInstance()->addColumn(QString("%1:%2   %3:%4").arg(QStringLiteral("结果"))
                                                  .arg(result)
                                                  .arg(QStringLiteral("自定义"))
                                                  .arg(userValue), QStringList(), QStringList());
            ui->treeView->setColumnWidth(curColumn, 150);
            thread->start(values, includeBracket, result);
            ui->label_7->clear();
        }
    }
    else
    {
        if (!ui->lineEdit_4->text().isEmpty() && !ui->lineEdit->text().isEmpty())
        {
            curColumn = XYTreeModel::getInstance()->addColumn(QString("%1:%2   %3:%4").arg(QStringLiteral("结果"))
                                                  .arg(result)
                                                  .arg(QStringLiteral("最大值"))
                                                  .arg(maxValue), QStringList(), QStringList());
            ui->treeView->setColumnWidth(curColumn, 150);
            thread->start(include3, include4, include5, include6, includeBracket, result, maxValue);
            ui->label_7->clear();
        }
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    XYTreeModel::getInstance()->clear();
    ui->treeView->reset();
    ui->label_5->clear();
    ui->label_7->clear();
}

void MainWindow::on_pushButton_4_clicked()
{
    if (thread->isRunning())
    {
        QMessageBox::information(this, QStringLiteral("别着急"), QStringLiteral("正在计算，稍等一会。。。"));
        return;
    }
    int setTimes = ui->lineEdit_2->text().toInt();

    thread->start(XYTreeModel::getInstance(), setTimes);

    ui->label_8->clear();
}

void MainWindow::on_pushButton_5_clicked()
{
    listModel->setStringList(QStringList());
    listModel->revert();
    ui->label_6->clear();
    ui->label_8->clear();
}

void MainWindow::on_pushButton_3_clicked()
{
    if (thread->isRunning())
    {
        QMessageBox::information(this, QStringLiteral("别着急"), QStringLiteral("正在计算，稍等一会。。。"));
        return;
    }
    QStringList currentItems = listModel->stringList();
    QString key = ui->lineEdit_3->text();
    QStringList keys = key.split(" ", QString::SkipEmptyParts);
    if (ui->radioButton->isChecked())
    {
        thread->start(true, keys, currentItems);
    }
    else if (ui->radioButton_2->isChecked())
    {
        thread->start(false, keys, currentItems);
    }
    ui->label_8->clear();
}

void MainWindow::on_pushButton_6_clicked()
{
    listModel->setStringList(allSameItems);
    listModel->revert();
}
