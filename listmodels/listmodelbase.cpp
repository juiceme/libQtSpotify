template <class ItemType>
ListModelBase<ItemType>::ListModelBase(QObject *parent)
    : QAbstractListModel(parent)
{ }

template <class ItemType>
int ListModelBase<ItemType>::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_dataList.size();
}

template <class ItemType>
ListModelBase<ItemType>::~ListModelBase()
{
    clear();
}

template <class ItemType>
void ListModelBase<ItemType>::appendRow(ItemType *item)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    connect(item, &QSpotifyObject::dataChanged, this, &ListModelBase<ItemType>::itemDataChanged);
    m_dataList.append(item);
    endInsertRows();
}

template <class ItemType>
void ListModelBase<ItemType>::appendRows(const QList<ItemType *> &items)
{
    if (items.isEmpty()) return;
    beginInsertRows(QModelIndex(),rowCount(),rowCount()+items.size()-1);
    for(auto item : items) {
        connect(item, &QSpotifyObject::dataChanged, this, &ListModelBase<ItemType>::itemDataChanged);
        m_dataList.append(item);
    }
    endInsertRows();
}

template <class ItemType>
void ListModelBase<ItemType>::insertRow(int row, ItemType *item)
{
    beginInsertRows(QModelIndex(),row,row);
    connect(item, &QSpotifyObject::dataChanged, this, &ListModelBase<ItemType>::itemDataChanged);
    m_dataList.insert(row,item);
    endInsertRows();
}

template <class ItemType>
ItemType *ListModelBase<ItemType>::find(const QString &id) const
{
    for(auto item : m_dataList)
        if(item->getId() == id) return item;
    return 0;
}

template <class ItemType>
QModelIndex ListModelBase<ItemType>::indexFromItem(const ItemType *item) const
{
    Q_ASSERT(item);
    for(int row=0; row<m_dataList.size();++row)
        if(m_dataList.at(row) == item) return index(row);

    return QModelIndex();
}

template <class ItemType>
void ListModelBase<ItemType>::clear()
{
    if (m_dataList.isEmpty()) return;
    beginRemoveRows(QModelIndex(),0, m_dataList.size()-1);
    while(!m_dataList.isEmpty()) {
        auto i = m_dataList.takeFirst();
        disconnect(i, &QSpotifyObject::dataChanged, this, &ListModelBase<ItemType>::itemDataChanged);
        i->release();
    }
    endRemoveRows();
}

template <class ItemType>
bool ListModelBase<ItemType>::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if (count == 0) return true;
    if(row < 0 || (row+count) >= m_dataList.size()) return false;
    beginRemoveRows(QModelIndex(),row,row+count-1);
    for(int i=0; i<count; ++i){
        auto i = m_dataList.takeAt(row);
        disconnect(i, &QSpotifyObject::dataChanged, this, &ListModelBase<ItemType>::itemDataChanged);
    }
    endRemoveRows();
    return true;

}

template <class ItemType>
ItemType *ListModelBase<ItemType>::takeRow(int row){
    beginRemoveRows(QModelIndex(),row,row);
    auto item = m_dataList.takeAt(row);
    disconnect(item, &QSpotifyObject::dataChanged, this, &ListModelBase<ItemType>::itemDataChanged);
    endRemoveRows();
    return item;
}

template <class ItemType>
void ListModelBase<ItemType>::replaceData(const QList<ItemType *> &newData)
{
    clear();
    appendRows(newData);
}

template <class ItemType>
void ListModelBase<ItemType>::itemDataChanged()
{
    auto sndr = dynamic_cast<ItemType *>(sender());
    if(sndr) {
        int idx = m_dataList.indexOf(sndr);
        if(idx > -1 && idx < count()) {
            auto modelIdx = index(idx);
            emit dataChanged(modelIdx, modelIdx);
        }
    }
}
