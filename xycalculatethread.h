#ifndef XYCALCULATETHREAD_H
#define XYCALCULATETHREAD_H

#include <QThread>
#include <QTreeWidget>
#include "xytreemodel.h"

class XYCalculateThread : public QThread
{
    Q_OBJECT
public:
    enum TYPE {FINDRESULTS, FINDSAME, FINDWITHKEY};
    explicit XYCalculateThread(QObject *parent = 0);
    ~XYCalculateThread();
    TYPE getStartType();
    void start(bool include3,
               bool include4,
               bool include5,
               bool include6,
               int resultValue,
               int maxValue);
    void start(XYTreeModel *tree, int colums);
    void start(bool include, const QStringList &keys, const QStringList &all);
    void isOKGroup(const QList<int> &values);
    void calculateTwoNumbers(int a, int b, QString &text, const QList<int> &values, int index);

signals:
    void addString(const QString &oneResult, const QString &userData);
    void addSameString(const QString &sameText);
    void updateTree();
    void updateList(QStringList datas, bool all);

protected:
    void run();

private:
    void findResultes();
    void findTreeSameColumns();
    void findKeys();
    bool findColumnSameItem(int column, const QString &data, QString &sameData);

private:
    TYPE type;
    bool include3;
    bool include4;
    bool include5;
    bool include6;

    int  maxValue;
    int  resultValue;

    XYTreeModel *tree;
    int columns;
    int okGroupNumbers;
    QStringList allSameTexts;

    bool include;
    QStringList keys;
};

#endif // XYCALCULATETHREAD_H
