#ifndef LISTMODELBASE_H
#define LISTMODELBASE_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QList>
#include <QtCore/QVariant>

/***
 Abstract list model class, uses std::shared_pointer to objects it stores.

 ItemType Requirements:
 -ItemType should have a dataChanged() signal which is emitted whenever data
  changes, except when it changed from within setData() then it is optional.

 -ItemType should inherit from public std::enable_shared_from_this<ItemType>.
*/

template <class ItemType>  class ListModelBase : public QAbstractListModel
{

public:
    explicit ListModelBase(QObject *parent=nullptr);
    virtual ~ListModelBase();
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    void appendRow(ItemType * item);
    void appendRows(const QList<ItemType *> &items);
    void insertRow(int row, ItemType *item);
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    void replaceData(const QList<ItemType *> &newData);
    ItemType *takeRow(int row);
    ItemType *find(const QString &id) const;
    QModelIndex indexFromItem(const ItemType *item) const;
    void clear();

    bool isEmpty() const { return m_dataList.isEmpty(); }
    ItemType *at(int index) const { return m_dataList.at(index); }
    int count() const { return m_dataList.count(); }

protected:
    QList<ItemType *> m_dataList;

private slots:
    void itemDataChanged();
};

#include "listmodelbase.cpp"

#endif // LISTMODELBASE_H
