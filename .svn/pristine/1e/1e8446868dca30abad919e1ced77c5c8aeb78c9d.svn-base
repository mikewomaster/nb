#include "netmodel.h"
#include <QAbstractTableModel>

#define CHECK_BOX_COLUMN 0
#define ID_COLUMN 1
// #define File_PATH_COLUMN 2

NetModel::NetModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

void NetModel::updateData(QList<Device> recordList)
{
    m_recordList = recordList;
    beginResetModel();
    endResetModel();
}

void NetModel::clearDate()
{
    QList<Device> recordList;
    for (int i = 1; i <= 0xfa; ++i)
    {
        Device record;
        record.bChecked = false;
        record.id = i;
        recordList.append(record);
    }
    updateData(recordList);
}

// 行数
int NetModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_recordList.count();
}

// 列数
int NetModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

// 设置表格项数据
bool NetModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    int nColumn = index.column();
    Device record = m_recordList.at(index.row());
    switch (role)
    {
    case Qt::EditRole:
    {
        /*
            if (nColumn == File_PATH_COLUMN)
            {
                record.devDesc = value.toString();
                m_recordList.replace(index.row(), record);
                emit dataChanged(index, index);
                return true;
            }
        */
    }
    case Qt::CheckStateRole:
    {
        if (nColumn == CHECK_BOX_COLUMN)
        {
            record.bChecked = (value.toInt() == Qt::Checked);
            m_recordList.replace(index.row(), record);
            emit dataChanged(index, index);
            return true;
        }
    }
    default:
        return false;
    }
    return false;
}

// 表格项数据
QVariant NetModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int nRow = index.row();
    int nColumn = index.column();
    Device record = m_recordList.at(nRow);

    switch (role)
    {

    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    case Qt::DisplayRole:
    {
        //if (nColumn == File_PATH_COLUMN)
          //  return record.devDesc;
        if (nColumn == ID_COLUMN)
            return record.id;
        return "";
    }
    case Qt::CheckStateRole:
    {
        if (nColumn == CHECK_BOX_COLUMN)
            return record.bChecked ? Qt::Checked : Qt::Unchecked;
    }
    default:
        return QVariant();
    }

    return QVariant();
}

// 表头数据
QVariant NetModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
        case Qt::TextAlignmentRole:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);

        case Qt::DisplayRole:
        {
            if (orientation == Qt::Horizontal)
            {
                if (section == CHECK_BOX_COLUMN)
                    return QStringLiteral("Status");

                if (section == ID_COLUMN)
                    return QStringLiteral("ID");

                // if (section == File_PATH_COLUMN)
                //     return QStringLiteral("Description");
            }
        }
        default:
            return QVariant();
    }

    return QVariant();
}

// 表格可选中、可复选
Qt::ItemFlags NetModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return QAbstractItemModel::flags(index);

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    if (index.column() == CHECK_BOX_COLUMN)
        flags |= Qt::ItemIsUserCheckable;

    return flags;
}
