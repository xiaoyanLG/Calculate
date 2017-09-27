#ifndef XYTreeModel_H
#define XYTreeModel_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

//! [0]
class XYTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    static XYTreeModel *getInstance();
    XYTreeModel(QObject *parent = 0);
    ~XYTreeModel();

    void clear();
    QMap<int, QStringList> &getUserDatas();
    QStringList &getColumnDatas(int column);
    QStringList &getColumnUserDatas(int column);
    int addColumn(const QString &headerText, const QStringList &alldata, const QStringList &allUserdata);
    void appendColumnData(int column, const QString &data, const QString &userData);
    void appendColumnData(int column, const QStringList &datas, const QStringList &userDatas);
//! [0] //! [1]

    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
//! [1]

//! [2]
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    static XYTreeModel *instance;
    QStringList   headerTexts;

    QMap<int, QStringList>  mmapAllData;
    QMap<int, QStringList>  mmapAllUserData;
};

#endif // XYTreeModel_H
