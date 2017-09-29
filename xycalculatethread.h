#ifndef XYCALCULATETHREAD_H
#define XYCALCULATETHREAD_H

#include <QThread>
#include <QTreeWidget>
#include "xytreemodel.h"

class XYCalculateThread : public QThread
{
    Q_OBJECT
public:
    enum TYPE {FINDRESULTS, FINDSAME, FINDWITHKEY, FINDRESULTSWITHUSER};
    explicit XYCalculateThread(QObject *parent = 0);
    ~XYCalculateThread();
    TYPE getStartType();
    void start(bool include3,
               bool include4,
               bool include5,
               bool include6,
               bool includeBracket,
               int resultValue,
               int maxValue);
    void start(const QList<int > &values, bool includeBracket, int resultValue);
    void start(XYTreeModel *tree, int colums);
    void start(bool include, const QStringList &keys, const QStringList &all);

    // 查找满足要求的组合
    void isOKGroup(const QList<int> &values);

    // 计算一组数的结果，不包括括号，顺序计算
    void calculateTwoNumbers(int a, int b, QString &text, const QList<int> &values, int index);

    // 计算一组数的结果，包括括号，首先按照括号的顺序计算
    QMap<int, QString> calculateGroups(const QList<int> &values, bool outer = false);

    // 查找一组数是否相加得value的值
    bool isNumbersAddToFixedValue(int a, int b, int value, const QList<int> &values, int index);

    // 获取一组数的排列组合方式
    void getNumbersByOrder(const QList<int> &source, QList<int> &target);

    // 函数获取一个组数不同排列顺序
    QList<QList<int> > getNumberByOrder(int maxNums);

signals:
    void addString(const QString &oneResult, const QString &userData);
    void addSameString(const QString &sameText);
    void updateTree();
    void updateList(QStringList datas, bool all);

protected:
    void run();

private:
    void findResultes();
    void findResultesWithUserValues();
    void findTreeSameColumns();
    void findKeys();
    bool findColumnSameItem(int column, const QString &data, QString &sameData);

private:
    TYPE type;
    bool include3;
    bool include4;
    bool include5;
    bool include6;
    bool includeBracket;

    int  maxValue;
    int  resultValue;

    QList<int>   userValues;
    XYTreeModel *tree;
    int columns;
    int okGroupNumbers;
    QStringList allSameTexts;

    bool include;
    QStringList keys;
};

#endif // XYCALCULATETHREAD_H
