#include "xytreemodel.h"

//! [0]
TreeModel::TreeModel(const QStringList &headers, const QString &data, QObject *parent)
    : QAbstractItemModel(parent)
{

}
//! [0]

//! [1]
TreeModel::~TreeModel()
{

}
//! [1]

//! [2]
int TreeModel::columnCount(const QModelIndex & /* parent */) const
{
    return headerTexts.size();
}
//! [2]

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();


    return allData[index.column()].at(index.row());
}

//! [3]
Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}
//! [3]

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return headerTexts.at(section);

    return QVariant();
}

//! [5]
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();
//! [5]

//! [6]
    if (column < headerTexts.size() && row < allData[column].size())
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
QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    return QModelIndex();
}
//! [7]

//! [8]
int TreeModel::rowCount(const QModelIndex &parent) const
{
    auto it = allData.begin();
    int rows = 0;
    while (it != allData.end())
    {
        rows = it.value().size() > rows ? it.value().size() : rows;
        ++it;
    }

    return rows;
}
//! [8]
