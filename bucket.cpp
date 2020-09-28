#include "bucket.h"
#include "ymodem.h"
#include <QString>

bucket::bucket():transTimes(0), index(0)
{

}

quint16 bucket::getCrc(QByteArray Data)
{
   quint16 mCrc = 0;
   quint16 sLen = Data.size();
   for(quint32 j = 0; j < sLen; j++){
       mCrc = mCrc^(quint16)(Data.at(j)) << 8;
       for (quint32 i=8; i!=0; i--) {
           if (mCrc & 0x8000)
               mCrc = mCrc << 1 ^ 0x1021;
           else
               mCrc = mCrc << 1;
       }
   }
   return mCrc;
}

void bucket::rcvEstablishContent()
{
    content.clear();
    content[0] = ACK;
    content[1] = CRC16;
}

void bucket::rcvTransmitContent()
{
    content.clear();
    content[0] = ACK;
}

void bucket::rcvEOTFirstContent()
{
    content.clear();
    content[0] = NAK;
}

void bucket::rcvEOTSecondContent()
{
    content.clear();
    content[0] = ACK;
    content[1] = CRC16;
}

void bucket::rcvFinContent()
{
    content.clear();
    content[0] = ACK;
}

void bucket::nullContent()
{
    content.clear();
    int cnt = 0;
    content[0] = SOH;
    content[1] = cnt;
    content[2] = ~cnt;

    QByteArray zero(128, 0x00);
    content += zero;

    quint16 crc = getCrc(content.mid(3));
    content += (quint8)(crc >> 8);
    content += (quint8)(crc & 0xff);
}

void bucket::EOTContent()
{
    content.clear();
    content += EOT;
}

void bucket::transmitPrepareContent()
{
    content.clear();

    index ++;

    if (index > transTimes)
        return;

    content[0] = STX;
    content[1] = index;
    content[2] = ~index;

    QByteArray dataOneTime;
    int validLen = TxDataAll.size();

    if(validLen >= LenUnit) {
        dataOneTime = TxDataAll.left(LenUnit);
    } else {
        QByteArray zero(LenUnit - validLen, 0x1A);
        dataOneTime = TxDataAll.left(validLen);
        dataOneTime += zero;
    }

    quint16 crc = getCrc(dataOneTime);
    content += dataOneTime;
    content += (quint8)(crc >> 8);
    content += (quint8)(crc & 0xff);
}

void bucket::establishPrepareContent()
{
    QString fileName = filePath.section('/', -1).toLocal8Bit();;

    content[0] = 0x01;
    content[1] = 0;
    content[2] = 0xff;

    content += fileName.toLocal8Bit();
    content += '\0';
    content += QString::number(TxDataAll.size());

    QByteArray zero(128 - (content.length() - 3), 0x00);
    content += zero;

    quint16 crc = getCrc(content.mid(3));

    content += (quint8)(crc >> 8);
    content += (quint8)(crc & 0xff);
}
