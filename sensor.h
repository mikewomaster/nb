#ifndef SENSOR_H
#define SENSOR_H

#include <QObject>
#include <QAbstractTableModel>

#define RTUSENSORADDR (501 - 1)
#define RTUSENSORNUM 7

#define RTUSENSORVALUE (701 - 1)
#define RTUVALUEENTRIES 50

#define RTUSENSORCLOCK  (801 - 1)
#define RTUSENSORINTERVAL (803 - 1)

#define SENSORCOUNTDOWNTIME (804 - 1)
#define SENSORTRANSFORMANT (805 - 1)
#define SENSORTS (806 - 1)

static const QStringList sl ={
      "",
      "ES101-LL",
      "IS101-FL-NB-BT8",
      "ES101-NL",
      "WS102-BRM",
      "WS102-CL",
      "WS102-ORP",
      "WS102-DOS",
      "WS102-PH",
      "ES101-NO2-2000",
      "ES101-NO-2000",
      "ES101-PM",
      "ES101-CO2-5000",
      "ES101-SO2-2000",
      "ES101-O3",
      "ES101-CO-2000",
      "ES101-RG",
      "ES101-WD",
      "ES101-WS",
      "ES101-AT",
      "ES102-TH",
      "ES104"
};

typedef struct sen{
    QString type;
    QString value;
    short type_;
    short id;
    int reg_addr;
    short len;
    int seq;
    bool used;
}sen;

class sensor: public QAbstractTableModel
{
public:
    sensor(QObject *parent = nullptr);
    void updateData(QList<sen> recordList);
    void clearDate(QList<sen>);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
private:
    QList<sen> m_recordList;
};

#endif // SENSOR_H
