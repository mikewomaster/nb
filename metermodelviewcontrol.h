#ifndef METERMODELVIEWCONTROL_H
#define METERMODELVIEWCONTROL_H

#include <QAbstractTableModel>
#include <QObject>

typedef struct meterProfile{
    QString tag;
    short id;
    QString magnitude;
}meterProfile;

class meterModelViewControl : public QAbstractTableModel
{
public:
    meterModelViewControl();
    meterModelViewControl(QObject *parent = nullptr);
    void updateData(QList<meterProfile> recordList);
    void clearDate();
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
private:
    QList<meterProfile> m_recordList;
};

#endif // METERMODELVIEWCONTROL_H
