#include "sensor.h"
#include "mainwindow.h"
#include <QAbstractTableModel>
#include <QMessageBox>
#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>

#define SENSORTYPE 0
#define SENSORID 1
#define SENSORREGADDR 2
#define SENSORLENGTH 3
#define SENSORSEQ 4

#define SENSORNUM (50 - 1)
#define TOTALCOLUMN 5

sensor::sensor(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int sensor::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return TOTALCOLUMN;
}

int sensor::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_recordList.count();
}

QVariant sensor::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
        case Qt::TextAlignmentRole:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);

        case Qt::DisplayRole:
        {
            if (orientation == Qt::Horizontal)
            {
                if (section == SENSORTYPE)
                    return QStringLiteral("RTU Name");

                if (section == SENSORID)
                    return QStringLiteral("Modbus Slave ID");

                if (section == SENSORREGADDR)
                    return QStringLiteral("Start Address");

                if (section == SENSORLENGTH)
                    return QStringLiteral("Value");

                if (section == SENSORSEQ)
                    return QStringLiteral("Seqeuence Debug");
            }
        }
        default:
            return QVariant();
    }
    return QVariant();
}

Qt::ItemFlags sensor::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return QAbstractItemModel::flags(index);

    int nColumn = index.column();
    Qt::ItemFlags flags;
    if (nColumn != SENSORLENGTH)
        flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    else
        flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    return flags;
}

QVariant sensor::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int nRow = index.row();
    int nColumn = index.column();
    sen record = m_recordList.at(nRow);

    switch (role)
    {
        case Qt::TextAlignmentRole:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        case Qt::DisplayRole:
        {
                if (nColumn == SENSORTYPE)
                    return record.type;
                if (nColumn == SENSORID)
                    return record.id;
                if (nColumn == SENSORREGADDR)
                    return record.reg_addr;
                if (nColumn == SENSORLENGTH)
                {
                    return record.len == -1 ? "INACTIVE": QString::number(record.len);
                }
                if (nColumn == SENSORSEQ)
                    return record.seq;

                return QVariant();
        }
        default:
            return QVariant();
    }
    return QVariant();
}

void sensor::updateData(QList<sen> recordList)
{
    m_recordList = recordList;
    beginResetModel();
    endResetModel();
}

void sensor::clearDate(QList<sen> recordList)
{
    updateData(recordList);
}

bool sensor::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    int nColumn = index.column();
    sen record = m_recordList.at(index.row());

    switch (role)
    {
        case Qt::EditRole:
        {
            if (nColumn == SENSORTYPE)
            {
                record.type = value.toString();
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
