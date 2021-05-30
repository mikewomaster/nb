#include "ymodem.h"
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QString>
#include <QTime>

ymodem::ymodem():status(waitCRC), receiveStatus(waitFirstCRC), dwnFlagRdy(false), rcvContent(0), pro(nullptr), rcvTimes4Bar(0)
{
}

void ymodem::setPort(QSerialPort* p)
{
    port = p;
    // connect(port, SIGNAL(readyRead()), this, SLOT(uploadToSlave()));
    connect(port, SIGNAL(readyRead()), this, SLOT(downloadToSlave()));
}

void ymodem::startDownloadSingle()
{
    data.content.clear();
    data.content += CRC16;
    port->write(data.content, data.content.length());
}

void ymodem::rcvFirstSet()
{
    int i = 3;
    QString str;

    while(total[i])
        str += total[i++];

    QDateTime time = QDateTime::currentDateTime();
    QString currentTime = time.toString("yyyyMMddhhmmss");
    str += currentTime;
    fileName = str;
    str.clear();

    i++;
    while(total[i])
        str += total[i++];

    data.totalSize = str.toInt();
    data.transTimes = (data.totalSize / STXLENGTH + 1);
}

void ymodem::writeToFile(int len)
{
    QFile file;
    file.setFileName(fileName);
    file.open(QIODevice::Append);

    const char* ch;
    ch = total.mid(3,len).data();
    /*
        QString string;
        string = QString(total.mid(3,len));

        for (int i = 800; i < len; i++) {
        }
        file.write(total.mid(3, len));
    */

    file.write(ch);
    file.close();
}

void ymodem::downloadToSlave()
{
    QByteArray rcvData = port->readAll();
    total += rcvData;

    switch (receiveStatus) {
        case waitFirstCRC: {
            if (total.length() != SOHLENGTH + RCVHEADFIRST && total.length() != 2 * (SOHLENGTH + RCVHEADFIRST))
                break;

            dwnFlagRdy = true;
            rcvFirstSet();
            total.clear();
            receiveStatus = rcvAckCRCFirst;
            pro->setValue(5);
            data.rcvEstablishContent();
            port->write(data.content, data.content.length());

            break;
        }

        case rcvAckCRCFirst: {
            if (total.length() != 2 * (STXLENGTH + RCVHEADFIRST) && total.length() != (STXLENGTH + RCVHEADFIRST) )
                break;

            // writeToFile(STXLENGTH);
            // rcvContent += STXLENGTH;
            total.clear();
            receiveStatus = rcvACK;
            data.rcvTransmitContent();
            port->write(data.content, data.content.length());

            data.transTimes --;
            pro->setValue(35);
            break;
        }

        case rcvACK: {
            if (total.length() != STXLENGTH + RCVHEADFIRST && total.length() != SOHLENGTH + RCVHEADFIRST && total.length() != (STXLENGTH + RCVHEADFIRST) + 1 && total.length() != (STXLENGTH + RCVHEADFIRST) + 2)
                break;

            if (total.length() == STXLENGTH + RCVHEADFIRST) {
                rcvContent += STXLENGTH;
                writeToFile(STXLENGTH);
            } else {
                rcvContent += SOHLENGTH;
                writeToFile(SOHLENGTH);
            }

            if (rcvContent >= data.totalSize) {
                rcvContent = 0;
                receiveStatus = rcvNAK;
            }

            total.clear();
            data.rcvTransmitContent();
            rcvTimes4Bar ++;
            int progressBarValue = rcvTimes4Bar / 100;
            pro->setValue(45 + progressBarValue);
            port->write(data.content, data.content.length());

            break;
        }

        case rcvNAK: {
            rcvTimes4Bar = 0;
            if (total.length() == 1 && total[0] == EOT) {
                total.clear();
                data.rcvEOTFirstContent();
                pro->setValue(85);
                port->write(data.content, data.content.length());
                receiveStatus = rcvACKCRCSecond;
            }

            break;
        }

        case rcvACKCRCSecond: {
            rcvTimes4Bar = 0;
            total.clear();

            data.rcvEOTSecondContent();
            port->write(data.content, data.content.length());

            receiveStatus = waitFirstCRC;
            port->close();
            _sleep(2000);
            pro->setValue(99);
            pro->close();
            emit(finRcv(fileName));

            if (total.length() == 1 && total[0] == EOT) {
                total.clear();
                data.rcvEOTSecondContent();
                port->write(data.content, data.content.length());

            }

            break;
        }

        case rcvACKSecond: {
            if (total.length() != SOHLENGTH + RCVHEADFIRST)
                break;

            total.clear();
            data.rcvFinContent();
            port->write(data.content, data.content.length());
            emit(finRcv(fileName));

            receiveStatus = waitFirstCRC;
            break;
        }
    }
}

void ymodem::uploadToSlave()
{
    QByteArray rcvData = port->readAll();

    // QString testString = rcvData;
    // qDebug() << testString;

    // FIX ME: Error Message
    if (rcvData.length() == 2 && rcvData.at(0) == CA && rcvData.at(1) == CA) {

    }

    switch (status) {
    case waitCRC:
    {
        if (rcvData.at(0) != 'C')
            break;

        data.establishPrepareContent();
        port->write(data.content, data.content.length());
        status = waitAckCrcFirst;
        break;
    }

    case waitAckCrcFirst:
    {
        if ((rcvData.at(0) == CRC16) || (rcvData.length() == 2 && rcvData.at(0) == ACK && rcvData.at(1) == CRC16)) {
            data.transmitPrepareContent();
            port->write(data.content, data.content.length());
            data.TxDataAll.remove(0, data.content.length() - 5);
            status = waitACK;
        }

        break;
    }

    case waitACK:
    {
       if(rcvData.at(0) == ACK) {
            data.transmitPrepareContent();
            port->write(data.content, data.content.length());
            data.TxDataAll.remove(0, data.content.length() - 5);
            if (data.index == data.transTimes)
                status = waitCRCSecond;
        }

        break;
    }

    case waitCRCSecond:
    {
        if(rcvData.at(0) == CRC16) {
            data.EOTContent();
            port->write(data.content, data.content.length());
            status = waitAckCrcSecond;
        }

        break;
    }

    case waitAckCrcSecond:
    {
        if ((rcvData.length() == 2 && rcvData.at(0) == ACK && rcvData.at(1) == CRC16) || (rcvData.at(0) == CRC16)) {
            data.nullContent();
            port->write(data.content, data.content.length());
            status = WaitACKFinish;
        }

        break;
    }

    case WaitACKFinish:
    {
        if (rcvData.at(0) == ACK) {
            // FIX ME: Show Work Window
            status = waitCRC;
        }

        break;
    }

    default:
            break;

    };
}
