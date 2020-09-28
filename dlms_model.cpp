#include "dlms_model.h"
#include <QAbstractTableModel>

dlmsModel::dlmsModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int dlmsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return TOTALCOLUMN;
}

int dlmsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_recordList.count();
}

QVariant dlmsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
        case Qt::TextAlignmentRole:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);

        case Qt::DisplayRole:
        {
            if (orientation == Qt::Horizontal)
            {
                if (section == ID_COLUMN)
                    return QStringLiteral("ID");

                if (section == OBIS_COLUMN)
                    return QStringLiteral("OBIS Code");

                if (section == VALUE_COLUMN)
                    return QStringLiteral("Value");

                if (section == TIMESTAMP_COLUMN)
                    return QStringLiteral("Timestamp");

                if (section == STATUS_COLUMN)
                    return QStringLiteral("Status");
            }
        }
        default:
            return QVariant();
    }
    return QVariant();
}

Qt::ItemFlags dlmsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return QAbstractItemModel::flags(index);

    int nColumn = index.column();
    Qt::ItemFlags flags;
    if (nColumn == OBIS_COLUMN)
        flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    else
        flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    return flags;
}

QVariant dlmsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int nRow = index.row();
    int nColumn = index.column();
    Dlms record = m_recordList.at(nRow);

    switch (role)
    {
        case Qt::TextAlignmentRole:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        case Qt::DisplayRole:
        {
            if (nColumn == ID_COLUMN)
                return record.id;
            if (nColumn == OBIS_COLUMN)
                return record.obis_code;
            if (nColumn == VALUE_COLUMN)
                return record.obis_value;
            if (nColumn == STATUS_COLUMN)
                return record.status;
            if (nColumn == TIMESTAMP_COLUMN)
                return record.ts;
            return "";
        }
        default:
            return QVariant();
    }
    return QVariant();
}

void dlmsModel::updateData(QList<Dlms> recordList)
{
    m_recordList = recordList;
    beginResetModel();
    endResetModel();
}

void dlmsModel::clearDate()
{
    QList<Dlms> recordList;
    for (int i = 1; i <= DLMSNUM; ++i)
    {
        Dlms record;
        record.obis_code = "";
        record.obis_value = "";
        record.ts = "";
        record.status = "";
        recordList.append(record);
    }
    updateData(recordList);
}

bool dlmsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    int nColumn = index.column();
    Dlms record = m_recordList.at(index.row());
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

