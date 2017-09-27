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
                              int resultValue,
                              int maxValue)
{
    type = FINDRESULTS;
    this->okGroupNumbers = 0;
    this->include3 = include3;
    this->include4 = include4;
    this->include5 = include5;
    this->include6 = include6;
    this->resultValue = resultValue;
    this->maxValue = maxValue;

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
    QString text;
    if (values.size() >= 2)
    {
        calculateTwoNumbers(values.at(0), values.at(1), text, values, 1);
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
        for (int i = 0; i < 4; ++i)
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
        for (int i = 0; i < 4; ++i)
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
                    tempText += QString("%1 %2 ").arg(operateStrings.at(i)).arg(b);
                }
                calculateTwoNumbers(retB, values.at(index), tempText, values, index);
            }
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

