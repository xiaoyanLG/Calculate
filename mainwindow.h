#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void addSameString(const QString &sameText);
    void showCurrentRow(const QModelIndex &index);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

private:
    void addOneColumn();

private:
    Ui::MainWindow *ui;
    XYCalculateThread *thread;

    int curRow;
    int curColumn;
};

#endif // MAINWINDOW_H
