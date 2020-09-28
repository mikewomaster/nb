#ifndef NETMODEL_H
#define NETMODEL_H

#include <QObject>
#include <QAbstractTableModel>

typedef struct Device{
    int id;
    bool bChecked;
}Device;

class NetModel: public QAbstractTableModel
{
public:
    NetModel(QObject *parent = nullptr);
    void updateData(QList<Device> recordList);
    void clearDate();
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
private:
    QList<Device> m_recordList;
};

#endif // NETMODEL_H
