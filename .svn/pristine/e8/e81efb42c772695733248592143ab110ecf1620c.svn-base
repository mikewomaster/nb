#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QDebug>
#include <QTime>
#include <memory>

#include "coap.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::on_coapApply_clicked()
{
    QVector<quint16> values;
    int i = 0;
    QString str = ui->coapURL->text();

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

    /*
    for (i = (i / 2); i < URLEntries; i++) {
         values.push_back(0x0000);
    }
    */

    // method value
    quint16 coapMethod = ui->coapMethod->currentIndex();
    values.push_back(coapMethod);

    /*
    // interval value
    quint32 timeoutRead = ui->coapInterval->text().toUInt();

    for (int i = 1; i >= 0; i--) {
        quint16 temp = 0;
        temp = (timeoutRead >> (i*2*8)) & 0x0000ffff;
        values.push_back(temp);
    }
    */

    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, URLAddress, URLEntries+CoapMethodEntries);

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

void MainWindow::coapReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        QString s;
        for (uint i = 0; i < URLEntries; i++) {
            if ((unit.value(i) >> 8) == 0x00)
                break;
            s[2*i] = unit.value(i) >> 8;
            if ((unit.value(i) & 0x00ff) == 0x00)
                break;
            s[(2*i) +1] = unit.value(i) & 0x00ff;
        }
        ui->coapURL->setText(s);

        quint32 methodValue = unit.value(32);
        ui->coapMethod->setCurrentIndex(methodValue);

        /*
            quint64 data;
            data = unit.value(33);
            data = (data << 16) + unit.value(34);
            ui->coapInterval->setText(QString::number(data));
        */

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

void MainWindow::on_coapReload_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    quint16 ADDR = URLAddress;
    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, URLEntries+CoapMethodEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::coapReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

// check status
void MainWindow::coapStatusReadReady()
{
    handle_read_ready(ui->coapStatus);
}

void MainWindow::on_mqttApply_3_clicked()
{
    handle_read(CoapStatusAddress, &coapStatusReadReady);
}


void MainWindow::on_dtlsApply_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QVector<quint16> values;
    int i = 0;
    QString str = ui->dtlsURLGetLineEdit->text();

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

    for (i = (i / 2); i < CoapURLGetEntries; i++) {
        values.push_back(0x0000);
    }

    // dtls status value
    quint16 dtlsenabled = ui->dtlsCombox->currentIndex();
    values.push_back(dtlsenabled);

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, CoapURLGetAddress, CoapURLGetEntries + CoapDTLSEntries);
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

    // user_id & psk
    values.clear();
    i = 0;
    str = "";

    // user id
    str = ui->dtlsUSerIDLineEdit->text();
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

    for (i = (i / 2); i < CoapDTLSUserIDEntries; i++) {
        values.push_back(0x0000);
    }

    // psk
    str.clear();
    str = ui->dtlsPSKLineEdit->text();
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

    for (i = (i / 2); i < CoapDTLSPSKEntries; i++) {
        values.push_back(0x0000);
    }

    QModbusDataUnit writeUnit2 = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, CoapDTLSUserIDAddress, CoapDTLSUserIDAddress + CoapDTLSPSKEntries);
    writeUnit2.setValues(values);
    if (auto *reply = modbusDevice->sendWriteRequest(writeUnit2, ui->serverEdit->value())) {
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

void MainWindow::coapReadTwoReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        QString s;

        // user id
        for (uint i = 0; i < CoapDTLSUserIDEntries; i++) {
            if ((unit.value(i) >> 8) == 0x00)
                break;
            s[2*i] = unit.value(i) >> 8;

            if ((unit.value(i) & 0x00ff) == 0x00)
                break;
            s[(2*i) +1] = unit.value(i) & 0x00ff;
        }
        ui->dtlsUSerIDLineEdit->setText(s);

        // psk
        s.clear();
        for (uint i = CoapDTLSUserIDEntries; i < CoapDTLSUserIDEntries + CoapDTLSPSKEntries; i++) {
            if ((unit.value(i) >> 8) == 0x00)
                break;
            s[2*i] = unit.value(i) >> 8;

            if ((unit.value(i) & 0x00ff) == 0x00)
                break;
            s[(2*i) +1] = unit.value(i) & 0x00ff;
        }
        ui->dtlsPSKLineEdit->setText(s);

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

void MainWindow::CoapSetReadTwo()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    quint16 ADDR = CoapDTLSUserIDAddress;
    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, CoapDTLSUserIDEntries + CoapDTLSPSKEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::coapReadTwoReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::coapReadOneReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        QString s;
        for (uint i = 0; i < CoapURLGetEntries; i++) {
            if ((unit.value(i) >> 8) == 0x00)
                break;
            s[2*i] = unit.value(i) >> 8;

            if ((unit.value(i) & 0x00ff) == 0x00)
                break;
            s[(2*i) +1] = unit.value(i) & 0x00ff;
        }
        ui->dtlsURLGetLineEdit->setText(s);

        ui->dtlsCombox->setCurrentIndex(unit.value(41));

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

void MainWindow::CoapSetReadOne()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    quint16 ADDR = CoapURLGetAddress;
    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, CoapURLGetEntries + CoapDTLSEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::coapReadOneReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::on_dtlsReload_clicked()
{
    CoapSetReadOne();
    _sleep(2000);
    CoapSetReadTwo();
}

void MainWindow::coapDtlsChkReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();

        ui->URLGetLineEdit->setText(QString::number(unit.value(0)));

        QString s;
        for (uint i = 1; i < 1 + CoapURLGetPayLoadEntries; i++) {
            if ((unit.value(i) >> 8) == 0x00)
                break;
            s[2*i] = unit.value(i) >> 8;

            if ((unit.value(i) & 0x00ff) == 0x00)
                break;
            s[(2*i) +1] = unit.value(i) & 0x00ff;
        }
        ui->payloadLineEdit->setText(s);

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

void MainWindow::on_dtlsChekPushButton_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    quint16 ADDR = CoapURLGetStatusAddress;
    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, CoapURLGetStatusEntries + CoapURLGetPayLoadEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::coapDtlsChkReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}
