#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>

#include "mainwindow.h"
#include "mbus_protocol.h"
#include "mbus_protocol_aux.h"

#define PAGESIZE 512
#define MODBUSUNITSIZE 20
#define EVENTLOGENABLE (811 - 1)
#define EVENTLOGERASE (812 - 1)
#define DATALOGSTART (21  - 1)
#define DATALOGSTOP (22 - 1)
#define EVENTLOGSTART (23  - 1)
#define EVENTLOGSTOP (24 - 1)

void MainWindow::parseEventLog(QString fileName)
{
    if(modbusDevice->state() == QModbusDevice::UnconnectedState) {
        m_ymodem->port->close();
        m_ymodem->dwnFlagRdy = false;
        modbusDevice->connectDevice();
    }

    if (m_ymodem->port->isOpen()) {
        m_ymodem->port->close();
        m_ymodem->dwnFlagRdy = false;
    }
    modbusDevice->connectDevice();

    QFile *fileStore = new QFile(fileName+"_logger");
    fileStore->open(QFile::ReadWrite);

    QFile *filep = new QFile(fileName);
    if (!filep->open(QFile::ReadOnly | QIODevice::Text))
        return;

    int allTimes = filep->size() / 512;

    for (int i = 0; i < allTimes; i++) {
        filep->seek(i * PAGESIZE);
        QByteArray dataAll;
        dataAll.clear();
        dataAll = filep->read(PAGESIZE);

        quint8 mbusMode = dataAll.at(0);
        quint8 mbusAddress = (uchar) dataAll.at(1);
        // if (!mbusAddress)
            // continue;

         // FIX ME: id
         // if (mbusAddress != ui->mbusReqEdit->text().toInt()) {
             // fileStore->write(dataAll);
             // break;
         // }

        // if (mbusAddress == 0 || mbusAddress == 0xff)
            // break;

        quint64 mbusId = (uchar) dataAll.at(11);
        mbusId = (mbusId << 8) + (uchar)dataAll.at(10);
        mbusId = (mbusId << 8) + (uchar)dataAll.at(13);
        mbusId = (mbusId << 8) + (uchar)dataAll.at(12);

        quint16 mbusManu = (uchar)dataAll.at(15);
        mbusManu = (mbusManu << 8) + (uchar)dataAll.at(14);

        quint16 mbusVersion = (uchar)dataAll.at(17);
        quint16 mbusMedium = (uchar)dataAll.at(16);

        char mbusManuBuf[16];
        mbus_decode_manufacturer((mbusManu & 0xff), ((mbusManu >> 8) & 0xff), mbusManuBuf, sizeof(mbusManuBuf));

        char mbusMediumBuf[16];
        mbus_data_variable_medium_lookup(mbusMedium, mbusMediumBuf, sizeof(mbusMediumBuf));

        for (int j = 0; j < MODBUSUNITSIZE; j++) {
            quint16 temp_2 = (uchar) dataAll.at(40 + 0 + j*20);
            quint64 temp_8 = 0;
            memset(&temp_8, 0, sizeof(temp_8));
            quint64 val_1, val_2;
            val_1 = (uchar)dataAll.at(40 + 9 + j*20);
            val_1 = (val_1 << 8) + (uchar)dataAll.at(40 + 8 + j*20);
            val_1 = (val_1 << 8) + (uchar)dataAll.at(40 + 11 + j*20);
            val_1 = (val_1 << 8) + (uchar)dataAll.at(40 + 10 + j*20);

            val_2 = (uchar)dataAll.at(40 + 5 + j*20);
            val_2 = (val_2 << 8) + (uchar)dataAll.at(40 + 4 + j*20);
            val_2 = (val_2 << 8) + (uchar)dataAll.at(40 + 7 + j*20);
            val_2 = (val_2 << 8) + (uchar)dataAll.at(40 + 6 + j*20);

            temp_8 = (quint64) (val_1 | val_2 << 32);

            int vif = temp_2;
            double value_in = temp_8, value_out;
            char *unit_str, *quantity_str;
            mbus_vif_unit_normalize(vif, value_in, &unit_str, &value_out, &quantity_str);

            quint32 timeStampDate;

            timeStampDate = (uchar)dataAll.at(40 + 57 + j*20);
            timeStampDate = (timeStampDate << 8) + (uchar)dataAll.at(40 + 56 + j*20);
            timeStampDate = (timeStampDate << 8) + (uchar)dataAll.at(40 + 59 + j*20);
            timeStampDate = (timeStampDate << 8) + (uchar)dataAll.at(40 + 58 + j*20);
            QDateTime time = QDateTime::fromTime_t(timeStampDate);
            QString StrCurrentTime = time.toString("yyyy-MM-dd hh:mm:ss ddd");

            QString writeLine = QString::number(mbusAddress) + ","\
                                + QString::number(mbusId) + ","\
                                + mbusManuBuf + ","\
                                + QString::number(mbusVersion) + ","\
                                + mbusMediumBuf + ","\
                                + QString::number(value_out) + ","\
                                + unit_str + ","\
                                + quantity_str;
                                // + StrCurrentTime;

            fileStore->write(writeLine.toLocal8Bit());
            fileStore->write("\r\n");
        }

        quint16 batteryVoltage = dataAll.at(441);
        batteryVoltage = (batteryVoltage << 8) + dataAll.at(440);

        quint16 batteryStatus = dataAll.at(443);
        batteryStatus = (batteryStatus << 8) + dataAll.at(442);
        QString bS = "";
        if (batteryStatus == 0)
            bS = "Normal";
        else
            bS = "UnNormal";

        quint32 timeStampDate;
        timeStampDate = (uchar)dataAll.at(447);
        timeStampDate = (timeStampDate << 8) + (uchar)dataAll.at(446);
        timeStampDate = (timeStampDate << 8) + (uchar)dataAll.at(445);
        timeStampDate = (timeStampDate << 8) + (uchar)dataAll.at(444);
        QDateTime time = QDateTime::fromTime_t(timeStampDate);
        QString currentTime = time.toString("yyyy-MM-dd hh:mm:ss ddd");

        QString lastLine = QString::number(batteryVoltage) + "mv" + "," + bS + "," + currentTime;
        fileStore->write(lastLine.toLocal8Bit());
        fileStore->write("\r\n");
    }

    filep->close();
    fileStore->close();

    // filep->remove();
    delete(filep);
    delete(fileStore);

    // ui->eventLogEnableRadioButton->setChecked(false);
    // on_eventLogEnableRadioButton_clicked();
}

void MainWindow::on_eventLogEnableRadioButton_clicked()
{
     if (ui->eventLogEnableRadioButton->isChecked()) {
         ui->ELogChkPushButton->setEnabled(true);
     }
     else {
          ui->ELogChkPushButton->setEnabled(false);
     }

     if (!modbusDevice)
         return;

     if(modbusDevice->state() == QModbusDevice::UnconnectedState) {
         if (!m_ymodem->port)
             m_ymodem->port->close();
         modbusDevice->connectDevice();
         m_ymodem->dwnFlagRdy = false;
     }

     QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, EVENTLOGENABLE, 1);

     if (ui->eventLogEnableRadioButton->isChecked())
         writeUnit.setValue(0, 1);
     else
         writeUnit.setValue(0, 0);

     handle_write(writeUnit);
}

void MainWindow::on_logErasePushButton_clicked()
{
    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, EVENTLOGERASE, 1);

    writeUnit.setValue(0, 1);
    handle_write(writeUnit);
    ui->logErasePushButton->setEnabled(false);
    _sleep(30000);
    ui->logErasePushButton->setEnabled(true);
}

void MainWindow::on_ELogChkPushButton_clicked()
{
    // if (!ui->eventLogEnableRadioButton->isChecked())
        // return;

    if (modbusDevice->state() == QModbusDevice::ConnectedState)
        modbusDevice->disconnectDevice();

    if (m_serial == nullptr) {
        m_serial = new QSerialPort();
        m_serial->setPortName(ui->portComboBox->currentText());
        m_serial->setBaudRate(QSerialPort::Baud115200);
        m_serial->setDataBits(QSerialPort::Data8);
        m_serial->setParity(QSerialPort::NoParity);
        m_serial->setStopBits(QSerialPort::OneStop);
        m_serial->setFlowControl(QSerialPort::NoFlowControl);
        m_serial->open(QIODevice::ReadWrite);
    } else {
        m_serial->setPortName(ui->portComboBox->currentText());
        m_serial->setBaudRate(QSerialPort::Baud115200);
        m_serial->setDataBits(QSerialPort::Data8);
        m_serial->setParity(QSerialPort::NoParity);
        m_serial->setStopBits(QSerialPort::OneStop);
        m_serial->setFlowControl(QSerialPort::NoFlowControl);
        m_serial->open(QIODevice::ReadWrite);
    }
    statusBar()->clearMessage();
    ui->ELogChkPushButton->setEnabled(false);

    m_ymodem->setPort(m_serial);
    if (!m_ymodem->pro)
        m_ymodem->pro = new QProgressDialog();
    connect(m_ymodem->pro, SIGNAL(canceled()), this, SLOT(ymodemCancelButtonCliked()));

    m_ymodem->pro->setLabelText(tr("Processing... Please wait..."));
    m_ymodem->pro->setRange(0, 100);
    m_ymodem->pro->setModal(true);
    m_ymodem->pro->setCancelButtonText(tr("Cancel"));
    m_ymodem->pro->setValue(0);

    int i = 20;
    while(!m_ymodem->dwnFlagRdy && i--) {
        if (m_ymodem->port->isOpen())
            m_ymodem->startDownloadSingle();
        else
            break;
        _sleep(3000);
    }
}


// download log
void MainWindow::serialMbusSwitch(int addr)
{
    if (!modbusDevice)
        return;

    if(modbusDevice->state() == QModbusDevice::UnconnectedState) {
        if (!m_ymodem->port)
            m_ymodem->port->close();
        modbusDevice->connectDevice();
        m_ymodem->dwnFlagRdy = false;
    }

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, addr, 1);
    writeUnit.setValue(0, 1);
    handle_write(writeUnit);
}

void MainWindow::datalogStart()
{
    serialMbusSwitch(DATALOGSTART);
}

void MainWindow::datalogStop()
{
    serialMbusSwitch(DATALOGSTOP);
}

void MainWindow::eventlogStart()
{
    serialMbusSwitch(EVENTLOGSTART);
}

void MainWindow::eventlogStop()
{
    serialMbusSwitch(EVENTLOGSTOP);
}

void MainWindow::on_dataLogPushButton_clicked()
{
    datalogStart();
    _sleep(2000);
    emit on_ELogChkPushButton_clicked();
    _sleep(500);
    datalogStop();
}

void MainWindow::on_eventLogPushButton_clicked()
{
    eventlogStart();
    _sleep(2000);
    emit on_ELogChkPushButton_clicked ();
    _sleep(500);
    eventlogStop();
}
