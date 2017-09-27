#ifndef XYCALCULATETHREAD_H
#define XYCALCULATETHREAD_H

#include <QThread>
#include <QTreeWidget>

class XYCalculateThread : public QThread
{
    Q_OBJECT
public:
    enum TYPE {FINDRESULTS, FINDSAME};
    explicit XYCalculateThread(QObject *parent = 0);
    ~XYCalculateThread();
    void start(bool include3,
               bool include4,
               bool include5,
               bool include6,
               int resultValue,
               int maxValue);
    void start(QTreeWidget *tree, int colums);
    void isOKGroup(const QList<int> &values);
    void calculateTwoNumbers(int a, int b, QString &text, const QList<int> &values, int index);

signals:
    void addString(const QString &oneResult, const QString &userData);
    void addSameString(const QString &sameText);

protected:
    void run();

private:
    void findResultes();
    void findTreeSameColumns();
    bool findColumnSameItem(int column, const QString &data, QString &sameData);

private:
    TYPE type;
    bool include3;
    bool include4;
    bool include5;
    bool include6;

    int  maxValue;
    int  resultValue;

    QTreeWidget *tree;
    int columns;
};

#endif // XYCALCULATETHREAD_H
