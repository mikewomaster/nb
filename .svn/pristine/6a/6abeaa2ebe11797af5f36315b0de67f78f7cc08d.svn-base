#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QDebug>
#include <QTime>
#include <QMessageBox>

#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "dlms.h"

void MainWindow::on_dlmsApply_clicked()
{
    QVector<quint16> values;
    int i = 0;
    QModbusDataUnit writeUnit;

    // srv addr
    quint16 srvAddr = ui->dlmsSrvAddr->text().toInt();
    values.push_back(srvAddr);

    // client addr
    quint16 cliAddr = ui->dlmsCliAddr->text().toInt();
    values.push_back(cliAddr);

    // logical naming
    quint16 ln = ui->dlmsLogName->currentIndex();
    values.push_back(ln);

    // auth
    quint16 auth = ui->dlmsAuth->currentIndex();
    values.push_back(auth);

    // read out
    quint32 timeoutRead = ui->dlmsReadout->text().toUInt();
    for (i = 1; i >= 0; i--) {
        quint16 temp = 0;
        temp = (timeoutRead >> (i*2*8)) & 0x0000ffff;
        values.push_back(temp);
    }
    if (ui->dlmsAuth->currentIndex() != 0) {
        // password
        QString str = ui->dlmsPwd->text();

        for (i = 0; i < str.size(); i++) {
            if ((i+1) % 2 == 0) {
                quint16 temp = str.at(i - 1).toLatin1();
                temp = (temp << 8) + str.at(i).toLatin1();
                values.push_back(temp);
            }
        }

        if (i % 2) {
            quint16 temp = str.at(i-1).toLatin1();
            temp = temp << 8;
            values.push_back(temp);
            i++;
        }

        for (i = (i / 2); i < 8; i++) {
            values.push_back(0x0000);
        }

        if (!modbusDevice)
            return;
        statusBar()->clearMessage();

        writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, DLMSDATAADDR, DLMSDATANUM);
    }
    else{
        writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, DLMSDATAADDR, DLMSDATANUM - 8);
    }
    writeUnit.setValues(values);

    if (auto *reply = modbusDevice->sendWriteRequest(writeUnit, ui->serverEdit->value())) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                if (reply->error() == QModbusDevice::ProtocolError) {
                    statusBar()->showMessage(tr("Write response error: %1 (Mobus exception: 0x%2)")
                        .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16),
                        5000);
                } else if (reply->error() != QModbusDevice::NoError) {
                    statusBar()->showMessage(tr("Write response error: %1 (code: 0x%2)").
                        arg(reply->errorString()).arg(reply->error(), -1, 16), 5000);
                }
                statusBar()->showMessage(tr("OK!"));
                reply->deleteLater();
            });
        } else {
            // broadcast replies return immediately
            reply->deleteLater();
        }
    } else {
        statusBar()->showMessage(tr("Write error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::dlmsReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        // srv addr
        ui->dlmsSrvAddr->setText(QString::number(unit.value(0)));

        // cli addr
        ui->dlmsCliAddr->setText(QString::number(unit.value(1)));

        // logical naming
        int index = unit.value(2);
        ui->dlmsLogName->setCurrentIndex(index);

        // authentication
        index = unit.value(3);
        ui->dlmsAuth->setCurrentIndex(unit.value(3));

        // read out interval
        quint64 to = unit.value(4);
        to = to << 16;
        to = to + unit.value(5);
        ui->dlmsReadout->setText(QString::number(to));

        // password
        QString s;
        for (uint i = 6; i < 14; i++) {
            if ((unit.value(i) >> 8) == 0x00)
                break;
            s[2*i] = unit.value(i) >> 8;
            if ((unit.value(i) & 0x00ff) == 0x00)
                break;
            s[(2*i) +1] = unit.value(i) & 0x00ff;
        }
        ui->dlmsPwd->setText(s);
        statusBar()->showMessage(tr("OK!"));
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
    } else {
        statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->error(), -1, 16), 5000);
    }
    reply->deleteLater();
}

void MainWindow::dlmsChkReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();

        // timestamp
        quint64 data;
        data = unit.value(0);
        data = (data << 16) + unit.value(1);

        QDateTime time = QDateTime::fromTime_t(data);
        QString StrCurrentTime = time.toString("yyyy-MM-dd hh:mm:ss ddd");
        ui->dlmsLstSeen->setText(StrCurrentTime);

        // status
        if (unit.value(2) == 0)
            ui->dlmsSts->setText("disconnected");
        else
            ui->dlmsSts->setText("connected");

        statusBar()->showMessage(tr("OK!"));
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
    } else {
        statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->error(), -1, 16), 5000);
    }
    reply->deleteLater();
}

void MainWindow::on_dlmsReload_clicked()
{
    if (!modbusDevice)
            return;
        statusBar()->clearMessage();

        quint16 ADDR = DLMSDATAADDR;
        QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, DLMSDATANUM);

        if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
            if (!reply->isFinished())
                connect(reply, &QModbusReply::finished, this, &MainWindow::dlmsReadReady);
            else
                delete reply; // broadcast replies return immediately
        } else {
            statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
        }
}

void MainWindow::on_dlmsChk_clicked()
{
    if (!modbusDevice)
            return;
        statusBar()->clearMessage();

        quint16 ADDR = DLMSCHK;
        QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, DLMSCHKNUM);

        if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
            if (!reply->isFinished())
                connect(reply, &QModbusReply::finished, this, &MainWindow::dlmsChkReadReady);
            else
                delete reply; // broadcast replies return immediately
        } else {
            statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
        }
}

void MainWindow::on_dlmsAuth_currentIndexChanged(int index)
{
    if (index == 0) {
        ui->dlmsPwd->setEnabled(false);
        ui->dlmsPwd->clear();
    }
    else {
       ui->dlmsPwd->setEnabled(true);
       ui->dlmsPwd->clear();
    }
}
