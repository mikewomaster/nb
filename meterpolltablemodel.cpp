#include "meterpolltablemodel.h"

meterPollTableModel::meterPollTableModel(){}

meterPollTableModel::meterPollTableModel(QObject *parent): QAbstractTableModel(parent){}

void meterPollTableModel::updateData(QList<meterPoll> recordList)
{
    m_recordList = recordList;
    beginResetModel();
    endResetModel();
}

void meterPollTableModel::clearDate()
{
    QList<meterPoll> recordList;

    for (int i = 1; i < 15; ++i)
    {
        meterPoll record;
        record.attribute = "";
        record.value = "";
        record.magnitude = "";
        recordList.append(record);
    }

    updateData(recordList);
}

int meterPollTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_recordList.count();
}

int meterPollTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

bool meterPollTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    int nColumn = index.column();
    meterPoll record = m_recordList.at(index.row());

    switch (role)
    {
        case Qt::EditRole:
        {

        }
        default:
            return false;
    }

    return false;
}

QVariant meterPollTableModel::data(const QModelIndex &index, int role) const
{
        if (!index.isValid())
            return QVariant();

        int nRow = index.row();
        int nColumn = index.column();
        meterPoll record = m_recordList.at(nRow);

        switch (role)
        {
            case Qt::TextAlignmentRole:
                return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
            case Qt::DisplayRole:
            {
                if (nColumn == 0)
                    return record.attribute;
                if (nColumn == 1)
                    return record.value;
                if (nColumn == 2)
                    return record.magnitude;

                return "";
            }
            default:
                return QVariant();
        }
        return QVariant();
}

Qt::ItemFlags meterPollTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return QAbstractItemModel::flags(index);

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    return flags;
}

QVariant meterPollTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
        case Qt::TextAlignmentRole:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);

        case Qt::DisplayRole:
        {
            if (orientation == Qt::Horizontal)
            {
                if (section == 0)
                    return QStringLiteral("Attribute");

                if (section == 1)
                    return QStringLiteral("Value");

                if (section == 2)
                    return QStringLiteral("Magnitude");
            }
        }
        default:
            return QVariant();
    }
    return QVariant();
}
