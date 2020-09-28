#ifndef MBUSDEVICESET_H
#define MBUSDEVICESET_H
#include <QVector>

#define mbusPrimaryAddressBase (2001 - 1)
#define mbusSecondaaryAddressBase (2002 -1)
#define mbusReadoutIntervalBase (2006 - 1)
#define mbusActiveBitMapBase (2008 - 1)
#define mbusTimeStampBase (2010 - 1)

#define mbusPrimaryAddressEntries 1
#define mbusSecondarAddressEntries 4
#define mbusReadoutIntervalEntries 2
#define mbusActiveBitMapEntries 2
#define mbusTimeStampMapEntries 2

typedef struct mbusSet {
    unsigned short s_primary;
    QVector<quint16> s_secondary;
    QVector<quint16> s_interval;
    QVector<quint16> s_timestamp;
}MbusSet;

#endif // MBUSDEVICESET_H
