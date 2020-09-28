#ifndef dlmsModel_H
#define dlmsModel_H

#include <QObject>
#include <QAbstractTableModel>

#define ID_COLUMN 0
#define OBIS_COLUMN 1
#define VALUE_COLUMN 2
#define TIMESTAMP_COLUMN 3
#define STATUS_COLUMN 4

#define DLMSNUM (20 - 1)
#define TOTALCOLUMN 5

typedef struct DLMS{
    int id;
    QString obis_code;
    QString obis_value;
    QString ts;
    QString status;
}Dlms;

class dlmsModel: public QAbstractTableModel
{
public:
    dlmsModel(QObject *parent = nullptr);
    void updateData(QList<Dlms> recordList);
    void clearDate();
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
private:
    QList<Dlms> m_recordList;
};

#endif // dlmsModel_H
