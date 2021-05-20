#include "metermodelviewcontrol.h"

meterModelViewControl::meterModelViewControl(){}

meterModelViewControl::meterModelViewControl(QObject *parent): QAbstractTableModel(parent){}

void meterModelViewControl::updateData(QList<meterProfile> recordList)
{
    m_recordList = recordList;
    beginResetModel();
    endResetModel();
}

void meterModelViewControl::clearDate()
{
    QList<meterProfile> recordList;

    for (int i = 1; i < 15; ++i)
    {
        meterProfile record;
        record.id = 0;
        record.tag = "";
        record.magnitude = "";
        recordList.append(record);
    }

    updateData(recordList);
}

int meterModelViewControl::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_recordList.count();
}

int meterModelViewControl::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

bool meterModelViewControl::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    int nColumn = index.column();
    meterProfile record = m_recordList.at(index.row());

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

QVariant meterModelViewControl::data(const QModelIndex &index, int role) const
{
        if (!index.isValid())
            return QVariant();

        int nRow = index.row();
        int nColumn = index.column();
        meterProfile record = m_recordList.at(nRow);

        switch (role)
        {
            case Qt::TextAlignmentRole:
                return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
            case Qt::DisplayRole:
            {
                if (nColumn == 0)
                    return record.tag;
                if (nColumn == 1)
                    if (record.id < 20)
                        return record.id;
                    else
                        return '-';
                if (nColumn == 2)
                    return record.magnitude;

                return "";
            }
            default:
                return QVariant();
        }
        return QVariant();
}

Qt::ItemFlags meterModelViewControl::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return QAbstractItemModel::flags(index);

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    return flags;
}

QVariant meterModelViewControl::headerData(int section, Qt::Orientation orientation, int role) const
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
                    return QStringLiteral("Attribution");

                if (section == 1)
                    return QStringLiteral("Data Index");

                if (section == 2)
                    return QStringLiteral("Magnitude");
            }
        }
        default:
            return QVariant();
    }
    return QVariant();
}
