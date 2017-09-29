#include "xycalculatethread.h"
#include <QDebug>

int add2(int a, int b, bool *ok)
{
    *ok = true;
    return a + b;
}

int sub2(int a, int b, bool *ok)
{
    *ok = true;
    return a - b;
}

int ride2(int a, int b, bool *ok)
{
    *ok = true;
    return a * b;
}

int divides2(int a, int b, bool *ok)
{
    *ok = true;
    if (b == 0)
    {
        *ok = false;
        return -1;
    }
    if (a % b != 0)
    {
        *ok = false;
    }
    return a / b;
}

typedef int (* OperateGroup)(int, int, bool *);

XYCalculateThread::XYCalculateThread(QObject *parent)
    : QThread(parent)
{

}

XYCalculateThread::~XYCalculateThread()
{

}

void XYCalculateThread::start(bool include3,
                              bool include4,
                              bool include5,
                              bool include6,
                              bool includeBracket,
                              int resultValue,
                              int maxValue)
{
    type = FINDRESULTS;
    this->okGroupNumbers = 0;
    this->include3 = include3;
    this->include4 = include4;
    this->include5 = include5;
    this->include6 = include6;
    this->includeBracket = includeBracket;
    this->resultValue = resultValue;
    this->maxValue = maxValue;

    QThread::start();
}

void XYCalculateThread::start(const QList<int> &values,
                              bool includeBracket,
                              int resultValue)
{
    type = FINDRESULTSWITHUSER;
    this->okGroupNumbers = 0;
    this->userValues = values;
    this->includeBracket = includeBracket;
    this->resultValue = resultValue;

    QThread::start();
}

XYCalculateThread::TYPE XYCalculateThread::getStartType()
{
    return type;
}

void XYCalculateThread::start(XYTreeModel *tree, int colums)
{
    type = FINDSAME;
    this->allSameTexts.clear();
    this->tree = tree;
    this->columns = colums;

    QThread::start();
}

void XYCalculateThread::start(bool include, const QStringList &keys, const QStringList &all)
{
    type = FINDWITHKEY;
    this->include = include;
    this->keys = keys;
    this->allSameTexts = all;

    QThread::start();
}

void XYCalculateThread::isOKGroup(const QList<int> &values)
{
    if (values.size() < 2)
    {
        return;
    }
    QString text;
    if (!this->includeBracket)
    {
        calculateTwoNumbers(values.at(0), values.at(1), text, values, 1);
    }
    else
    {
        calculateGroups(values, true);
    }
}

void XYCalculateThread::calculateTwoNumbers(int a, int b, QString &text, const QList<int> &values, int index)
{
    static QList<QString> operateStrings;
    static QList<OperateGroup> operates;
    if (operateStrings.isEmpty())
    {
        operateStrings << "+" << "-" << "*" << "/";
        operates << add2 << sub2 << ride2 << divides2;
    }
    bool ok = false;
    if (index == values.size() - 1)
    {
        if (text.isEmpty())
        {
            text = QString::number(a);
        }
        for (int i = 0; i < operates.size(); ++i)
        {
            int retB = operates.at(i)(a, b, &ok);
            if (ok && retB == resultValue)
            {
                QString userData;
                for (int j = 0; j < values.size(); ++j)
                {
                    userData += QString::number(values.at(j)) + " ";
                }
                emit addString(QString("%1 %2 %3 %4 %5").arg(text)
                               .arg(operateStrings.at(i))
                               .arg(b)
                               .arg("=")
                               .arg(resultValue), userData);
                okGroupNumbers++;
                if (okGroupNumbers % 5000 == 0)
                {
                    emit updateTree();
                }
            }
        }
    }
    else if (index < values.size() - 1)
    {
        ++index;
        for (int i = 0; i < operates.size(); ++i)
        {
            int retB = operates.at(i)(a, b, &ok);
            if (ok)
            {
                QString tempText = text;
                if (tempText.isEmpty())
                {
                    tempText = QString("%1 %2 %3 ").arg(a).arg(operateStrings.at(i)).arg(b);
                }
                else
                {
                    tempText += QString(" %1 %2 ").arg(operateStrings.at(i)).arg(b);
                }
                calculateTwoNumbers(retB, values.at(index), tempText, values, index);
            }
        }
    }
}

QMap<int, QString> XYCalculateThread::calculateGroups(const QList<int> &values, bool outer)
{
    static QList<QString> operateStrings;
    static QList<OperateGroup> operates;
    static QStringList allResults;
    if (outer)
    {
        allResults.clear();
    }
    if (operateStrings.isEmpty())
    {
        operateStrings << "+" << "-" << "*" << "/";
        operates << add2 << sub2 << ride2 << divides2;
    }
    bool ok = false;
    QMap<int, QString> curResults;
    if (values.size() == 2)
    {
        for (int i = 0; i < operates.size(); ++i)
        {
            int retB = operates.at(i)(values.at(0), values.at(1), &ok);
            if (ok)
            {
                if (outer)
                {
                    if (retB== resultValue)
                    {
                        QString userData;
                        for (int j = 0; j < values.size(); ++j)
                        {
                            userData += QString::number(values.at(j)) + " ";
                        }
                        if (!allResults.contains(userData))
                        {
                            allResults.append(userData);
                            QString validResult = QString("%1 %2 %3 %4 %5").arg(values.at(0))
                                    .arg(operateStrings.at(i))
                                    .arg(values.at(1))
                                    .arg("=")
                                    .arg(retB);
                            emit addString(validResult, userData);
                            okGroupNumbers++;
                            if (okGroupNumbers % 200 == 0)
                            {
                                emit updateTree();
                            }
                        }
                    }
                }
                else
                {
                    curResults.insertMulti(retB, QString("(%1 %2 %3)").arg(values.at(0))
                                      .arg(operateStrings.at(i))
                                      .arg(values.at(1)));
                }
            }
        }
    }
    else if (values.size() > 2)
    {
        QList<QList<int> > allGroups = getNumberByOrder(values.size());
        for (int groupIndex = 0; groupIndex < allGroups.size(); ++groupIndex)
        {
            QList<int> groups = allGroups.at(groupIndex);
            int curIndex = 0;
            QMap<int, QString> lastValues;
            QMap<int, QString> curValues;
            for (int i = 0; i < groups.size(); ++i)
            {
                curValues.clear();
                int count = groups.at(i);
                if (i == 0)
                {
                    if (count > 1)
                    {
                        lastValues = calculateGroups(values.mid(curIndex, count));
                    }
                    else
                    {
                        int value = values.at(curIndex);
                        lastValues.insertMulti(value, QString::number(value));
                    }
                }
                else
                {
                    if (count > 1)
                    {
                        curValues = calculateGroups(values.mid(curIndex, count));
                    }
                    else
                    {
                        int value = values.at(curIndex);
                        curValues.insertMulti(value, QString::number(value));
                    }
                    QMap<int, QString> tmpValues;
                    auto itLast = lastValues.begin();
                    while (itLast != lastValues.end())
                    {
                        int valueA = itLast.key();
                        QString valueAText = itLast.value();
                        auto itCur = curValues.begin();
                        while (itCur != curValues.end())
                        {
                            int valueB = itCur.key();
                            QString valueBText = itCur.value();
                            for (int j = 0; j < operates.size(); ++j)
                            {
                                int retB = operates.at(j)(valueA, valueB, &ok);
                                if (ok)
                                {
                                    if (outer)
                                    {
                                        tmpValues.insertMulti(retB, QString("%1 %2 %3").arg(valueAText)
                                                          .arg(operateStrings.at(j))
                                                          .arg(valueBText));
                                    }
                                    else
                                    {
                                        tmpValues.insertMulti(retB, QString("(%1 %2 %3)").arg(valueAText)
                                                          .arg(operateStrings.at(j))
                                                          .arg(valueBText));
                                    }
                                }
                            }
                            ++itCur;
                        }
                        ++itLast;
                    }
                    lastValues = tmpValues;
                }
                curIndex += count;
                if (curIndex == values.size())
                {
                    if (outer)
                    {
                        auto it = lastValues.find(resultValue);
                        while (it != lastValues.end())
                        {
                            int res = it.key();
                            QString text = it.value();
                            if (res == resultValue)
                            {
                                QString userData;
                                for (int k = 0; k < values.size(); ++k)
                                {
                                    userData += QString::number(values.at(k)) + " ";
                                }
                                if (!allResults.contains(userData))
                                {
                                    allResults.append(userData);
                                    QString validResult = QString("%1 %2 %3").arg(text)
                                            .arg("=")
                                            .arg(res);
                                    emit addString(validResult, userData);
                                    okGroupNumbers++;
                                    if (okGroupNumbers % 200 == 0)
                                    {
                                        emit updateTree();
                                    }
                                }
                            }
                            else
                            {
                                break;
                            }
                            ++it;
                        }
                    }
                    else
                    {
                        curResults.unite(lastValues);
                    }
                }
            }
        }
    }
    return curResults;
}

bool XYCalculateThread::isNumbersAddToFixedValue(int a, int b, int value, const QList<int> &values, int index)
{
    static QList<QString> operateStrings;
    static QList<OperateGroup> operates;
    if (operateStrings.isEmpty())
    {
        operateStrings << "+";
        operates << add2;
    }
    bool ok = false;
    if (index == values.size() - 1)
    {
        for (int i = 0; i < operates.size(); ++i)
        {
            int retB = operates.at(i)(a, b, &ok);
            if (ok && retB == value)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    else if (index < values.size() - 1)
    {
        ++index;
        for (int i = 0; i < operates.size(); ++i)
        {
            int retB = operates.at(i)(a, b, &ok);
            if (ok)
            {
                if (isNumbersAddToFixedValue(retB, values.at(index), value, values, index))
                {
                    return true;
                }
            }
            else
            {
                return false;
            }
        }
    }
    return false;
}

void XYCalculateThread::getNumbersByOrder(const QList<int> &source, QList<int> &target)
{
    QList<int > tmps = source;
    if (tmps.size() == 1)
    {
        isOKGroup(target + source);
    }
    else
    {
        for (int i = 0; i < tmps.size(); ++i)
        {
            QList<int > tmpSource = source;
            target.append(tmps.at(i));
            tmpSource.removeAt(i);
            getNumbersByOrder(tmpSource, target);
            target.removeLast();
        }
    }
}

void XYCalculateThread::run()
{
    switch (type)
    {
    case FINDRESULTS:
        findResultes();
        break;
    case FINDSAME:
        findTreeSameColumns();
        break;
    case FINDWITHKEY:
        findKeys();
        break;
    case FINDRESULTSWITHUSER:
        findResultesWithUserValues();
        break;
    default:
        break;
    }
}

void XYCalculateThread::findResultes()
{
    QList<int> allIncludeNumbers;
    if (include3)
    {
        allIncludeNumbers << 3;
    }
    if (include4)
    {
        allIncludeNumbers << 4;
    }
    if (include5)
    {
        allIncludeNumbers << 5;
    }
    if (include6)
    {
        allIncludeNumbers << 6;
    }

    for (int num = 0; num < allIncludeNumbers.size(); ++num)
    {
        int numbers = allIncludeNumbers.at(num);
        QList<int> values;
        for (int i = 0; i <= numbers; ++i)
        {
            if (i == 0 || i == numbers)
            {
                values << 0;
            }
            else
            {
                values << 1;
            }
        }

        while (1)
        {
            int pro = 0;
            values[pro]++;
            for (int j = 0; j < numbers; ++j)
            {
                if (values[pro] > maxValue)
                {
                    values[pro] = 1;
                    pro++;
                    values[pro]++;
                }
                else
                {
                    break;
                }
            }
            if (values[numbers] == 1)
            {
                break;
            }
            else
            {
                QList<int> tmp;
                for (int i = 0; i < values.size() - 1; ++i)
                {
                    tmp.prepend(values.at(i));
                }
                isOKGroup(tmp);
            }
        }
    }
    emit updateTree();
}

void XYCalculateThread::findResultesWithUserValues()
{
    QList<int > target;
    getNumbersByOrder(userValues, target);

    emit updateTree();
}

void XYCalculateThread::findTreeSameColumns()
{
    QStringList allSames;
    int rowcount = tree->rowCount();
    int columncount = tree->columnCount();
    int setTimes = columns;
    if (setTimes == 0)
    {
        setTimes = columncount;
    }

    QStringList &allDatas = tree->getColumnDatas(0);
    QStringList &allUserDatas = tree->getColumnUserDatas(0);
    allSameTexts.clear();
    for (int i = 0; i < rowcount; ++i)
    {
        QString firstText;
        if (i < allDatas.size())
        {
            firstText = allDatas.at(i);
        }
        QString firstData;
        if (i < allUserDatas.size())
        {
            firstData = allUserDatas.at(i);
        }
        if (columncount == 1)
        {
            if (!allSames.contains(firstData) && !firstData.isEmpty())
            {
                allSames.append(firstData);
                allSameTexts.append(firstData);
                if (allSameTexts.size() % 1000 == 0)
                {
                    emit updateList(allSameTexts, false);
                }
            }
        }
        else
        {
            int sametimes = 1;
            QString addText = firstText;
            for (int j = 1; j < columncount; ++j)
            {
                QString sameText;
                if (findColumnSameItem(j, firstData, sameText))
                {
                    sametimes++;
                    addText += QString("    \t%1").arg(sameText);
                }
            }
            if (sametimes == setTimes)
            {
                if (!allSames.contains(firstData))
                {
                    allSames.append(firstData);
                    allSameTexts.append(addText + QString("    \t( %1 )").arg(firstData));
                    if (allSameTexts.size() % 1000 == 0)
                    {
                        emit updateList(allSameTexts, false);
                    }
                }
            }
        }
    }
    emit updateList(allSameTexts, true);
}

void XYCalculateThread::findKeys()
{
    QStringList findItems;
    if (include)
    {
        QStringList currentItems = allSameTexts;
        QStringList keys = this->keys;

        for (int i = 0; i < currentItems.size(); ++i)
        {
            QString value = currentItems.at(i);
            int index = value.lastIndexOf("(");
            value = value.mid( index == -1 ? 0:index);
            QStringList values = value.split(" ", QString::SkipEmptyParts);
            bool contain = true;
            for (int j = 0; j < keys.size(); ++j)
            {
                if (!values.contains(keys.at(j)))
                {
                    contain = false;
                    break;
                }
            }
            if (contain)
            {
                findItems.append(currentItems.at(i));
                if (findItems.size() % 1000 == 0)
                {
                    emit updateList(findItems, false);
                }
            }
        }
    }
    else
    {
        QStringList currentItems = allSameTexts;
        QStringList keys = this->keys;

        for (int i = 0; i < currentItems.size(); ++i)
        {
            QString value = currentItems.at(i);
            int index = value.lastIndexOf("(");
            value = value.mid( index == -1 ? 0:index);
            QStringList values = value.split(" ", QString::SkipEmptyParts);
            int contain = 0;
            for (int j = 0; j < keys.size(); ++j)
            {
                if (!values.contains(keys.at(j)))
                {
                    break;
                }
                contain++;
            }
            if (contain != keys.size())
            {
                findItems.append(currentItems.at(i));
                if (findItems.size() % 1000 == 0)
                {
                    emit updateList(findItems, false);
                }
            }
        }
    }

    emit updateList(findItems, false);
}

bool XYCalculateThread::findColumnSameItem(int column, const QString &data, QString &sameData)
{
    QStringList &allUserDatas = tree->getColumnUserDatas(column);
    QStringList &allDatas = tree->getColumnDatas(column);

    int rowcount = allUserDatas.size();
    for (int i = 0; i < rowcount; ++i)
    {
        QString userData = allUserDatas.at(i);
        if (userData.isEmpty())
        {
            break;
        }
        if (data == userData)
        {
            sameData = allDatas.at(i);
            return true;
        }
    }
    return false;
}

QList<QList<int> > XYCalculateThread::getNumberByOrder(int maxNums)
{
    static QMap<int, QList<QList<int> >> oldMaped;
    QList<QList<int> > curNumbers;
    if (maxNums <= 2)
    {
        return curNumbers;
    }
    else
    {
        auto it = oldMaped.find(maxNums);
        if (it != oldMaped.end())
        {
            return it.value();
        }
    }
    QList<int> allIncludeNumbers;
    for (int i = 2; i <= maxNums; ++i)
    {
        allIncludeNumbers << i;
    }
    for (int num = 0; num < allIncludeNumbers.size(); ++num)
    {
        int numbers = allIncludeNumbers.at(num);
        QList<int> values;
        for (int i = 0; i <= numbers; ++i)
        {
            if (i == 0 || i == numbers)
            {
                values << 0;
            }
            else
            {
                values << 1;
            }
        }

        while (1)
        {
            int pro = 0;
            values[pro]++;
            for (int j = 0; j < numbers; ++j)
            {
                if (values[pro] >= maxNums)
                {
                    values[pro] = 1;
                    pro++;
                    values[pro]++;
                }
                else
                {
                    break;
                }
            }
            if (values[numbers] == 1)
            {
                break;
            }
            else
            {
                QList<int> tmp;
                for (int i = 0; i < values.size() - 1; ++i)
                {
                    tmp.prepend(values.at(i));
                }
                if (isNumbersAddToFixedValue(tmp.at(0), tmp.at(1), maxNums, tmp, 1))
                {
                    curNumbers.append(tmp);
                }
            }
        }
    }
    oldMaped.insert(maxNums, curNumbers);
    return curNumbers;
}

