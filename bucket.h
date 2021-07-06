#ifndef BUCKET_H
#define BUCKET_H
#include <QByteArray>
#include <QFileDialog>

#define SOH                     (0x01)  /* start of 128-byte data packet */
#define STX                     (0x02)  /* start of 1024-byte data packet */
#define EOT                     (0x04)  /* end of transmission */
#define LenUnit                 (1024)

class bucket
{

public:
    bucket();
    void establishPrepareContent();
    void transmitPrepareContent();
    void EOTContent();
    void nullContent();

    void rcvEstablishContent();
    void rcvTransmitContent();
    void rcvEOTFirstContent();
    void rcvEOTSecondContent();
    void rcvFinContent();
    QString filePath;
    QByteArray TxDataAll;
    long transTimes;
    int totalSize;
    QByteArray content;
    int index;

private:
    quint16 getCrc(QByteArray);

};

#endif // BUCKET_H
