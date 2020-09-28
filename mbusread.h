#ifndef MBUSREAD_H
#define MBUSREAD_H

#define mbusReadDeviceRAMAddr (3001 - 1)
#define mbusReadDeviceRAMEntries 220
#define mbusReadDeviceHead 20
#define mbusReadDeviceValue 50

typedef struct MBusDeviceStruct {
    unsigned char priAddr;
    unsigned char priMode;
    unsigned long long secAddr;
    unsigned int SNID;
    unsigned short manufr;
    unsigned char version;
    unsigned char medium;
    unsigned int status;
    unsigned int timestampSet;
    unsigned short reserved[7];
    unsigned char type;
    unsigned char unit;
    unsigned short scale;
    unsigned long long meterValueInt;
    unsigned int meterValueFloat;
    unsigned int timestampUpdate;
}MBusDevice;

#endif // MBUSREAD_H
