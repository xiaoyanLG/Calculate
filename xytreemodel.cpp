#include "xytreemodel.h"
#include <QDebug>

XYTreeModel *XYTreeModel::instance = NULL;
//! [0]
XYTreeModel *XYTreeModel::getInstance()
{
    return instance;
}

XYTreeModel::XYTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    instance = this;
}
//! [0]

//! [1]
XYTreeModel::~XYTreeModel()
{

}

void XYTreeModel::clear()
{
    headerTexts.clear();
    mmapAllData.clear();
    mmapAllUserData.clear();
}

QMap<int, QStringList> &XYTreeModel::getUserDatas()
{
    return mmapAllUserData;
}

QStringList &XYTreeModel::getColumnDatas(int column)
{
    static QStringList err;
    if (mmapAllData.find(column) == mmapAllData.end())
    {
        return err;
    }
    return mmapAllData[column];
}

QStringList &XYTreeModel::getColumnUserDatas(int column)
{
    static QStringList err;
    if (mmapAllUserData.find(column) == mmapAllUserData.end())
    {
        return err;
    }
    return mmapAllUserData[column];
}

int XYTreeModel::addColumn(const QString &headerText, const QStringList &alldata, const QStringList &allUserdata)
{
    int column = mmapAllData.size();
    headerTexts.append(headerText);
    mmapAllData.insert(mmapAllData.size(), alldata);
    mmapAllUserData.insert(mmapAllUserData.size(), allUserdata);
    return column;
}

void XYTreeModel::appendColumnData(int column, const QString &data, const QString &userData)
{
    if (mmapAllData.find(column) != mmapAllData.end())
    {
        mmapAllData[column].append(data);
    }
    if (mmapAllUserData.find(column) != mmapAllUserData.end())
    {
        mmapAllUserData[column].append(userData);
    }
}

void XYTreeModel::appendColumnData(int column, const QStringList &datas, const QStringList &userDatas)
{
    if (mmapAllData.find(column) != mmapAllData.end())
    {
        mmapAllData[column].append(datas);
    }
    if (mmapAllUserData.find(column) != mmapAllUserData.end())
    {
        mmapAllUserData[column].append(datas);
    }
}

bool XYTreeModel::hasChildren(const QModelIndex &parent) const
{
    return !parent.isValid();
}
//! [1]

//! [2]
int XYTreeModel::columnCount(const QModelIndex & /* parent */) const
{
    return headerTexts.size();
}
//! [2]

QVariant XYTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (index.row() < mmapAllData[index.column()].size())
        {
            return mmapAllData[index.column()].at(index.row());
        }
    }
    else if (role == Qt::UserRole)
    {
        if (index.row() < mmapAllUserData[index.column()].size())
        {
            return mmapAllUserData[index.column()].at(index.row());
        }
    }

    return QVariant();
}

//! [3]
Qt::ItemFlags XYTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}
//! [3]

QVariant XYTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return headerTexts.at(section);

    return QVariant();
}

//! [5]
QModelIndex XYTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();
//! [5]

//! [6]
    if (column < headerTexts.size() && row < rowCount())
    {
        return createIndex(row, column);
    }
    else
    {
        return QModelIndex();
    }
}
//! [6]

//! [7]
QModelIndex XYTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    return QModelIndex();
}
//! [7]

//! [8]
int XYTreeModel::rowCount(const QModelIndex &parent) const
{
    auto it = mmapAllData.begin();
    int rows = 0;
    while (it != mmapAllData.end())
    {
        rows = it.value().size() > rows ? it.value().size() : rows;
        ++it;
    }

    return rows;
}
//! [8]
