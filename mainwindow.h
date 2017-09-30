#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>

namespace Ui {
class MainWindow;
}

class XYCalculateThread;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void addString(const QString &one, const QString &userData);
    void showTreeCurrentRow(const QModelIndex &index);
    void showListCurrentRow(const QModelIndex &index);
    void setColumnWidth();
    void updateListView(QStringList datas, bool all);
    void threadFinished();
    void sortColumn(int column, Qt::SortOrder order);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_6_clicked();

private:
    void addOneColumn();

private:
    Ui::MainWindow *ui;
    XYCalculateThread *thread;
    QStringListModel  *listModel;

    int curRow;
    int curColumn;
    QStringList  allSameItems;
};

#endif // MAINWINDOW_H
