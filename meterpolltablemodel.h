#ifndef METERPOLLTABLEMODEL_H
#define METERPOLLTABLEMODEL_H

#include <QAbstractTableModel>
#include <QObject>

typedef struct meterPoll{
    QString attribute;
    QString value;
    QString magnitude;
}meterPoll;

class meterPollTableModel : public QAbstractTableModel
{
public:
    meterPollTableModel();
    meterPollTableModel(QObject *parent = nullptr);
    void updateData(QList<meterPoll> recordList);
    void clearDate();
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
private:
    QList<meterPoll> m_recordList;
};

#endif // METERPOLLTABLEMODEL_H
