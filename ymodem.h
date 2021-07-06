#ifndef YMODEM_H
#define YMODEM_H
#include "bucket.h"

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QProgressDialog>
#include <QFile>

enum Status {
    waitCRC,
    waitAckCrcFirst,
    waitACK,
    waitCRCSecond,
    waitAckCrcSecond,
    WaitACKFinish
};

enum rcvStatus {
    waitFirstCRC,
    rcvAckCRCFirst,
    rcvACK,
    rcvNAK,
    rcvACKCRCSecond,
    rcvACKSecond
};

#define ACK                     (0x06)  /* acknowledge */
#define NAK                     (0x15)  /* negative acknowledge */
#define CA                      (0x18)  /* two of these in succession aborts transfer */
#define CRC16                   (0x43)  /* 'C' == 0x43, request 16-bit CRC */

#define ABORT1                  (0x41)  /* 'A' == 0x41, abort by user */
#define ABORT2                  (0x61)  /* 'a' == 0x61, abort by user */

#define RCVHEADFIRST 3
#define SOHLENGTH 128
#define STXLENGTH 1024

class ymodem : public QObject
{
    Q_OBJECT

public:
    ymodem();
    void setPort(QSerialPort *p);
    void startDownloadSingle();
    bucket data;
    QByteArray total;
    bool dwnFlagRdy;
    QSerialPort *port;
    QProgressDialog *pro;
    rcvStatus receiveStatus;

private slots:
    void uploadToSlave();
    void downloadToSlave();

signals:
    void finRcv(QString);

private:
    void rcvFirstSet();
    void writeToFile(int);

    QString fileName;
    Status status;

    int rcvContent;
    int rcvTimes4Bar;
};

#endif // YMODEM_H
