#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QDebug>
#include <QTime>

#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "mbusdeviceset.h"

#ifdef STORE
static MbusSet s_mbusset;
#endif

void MainWindow::on_pushButton_3_clicked()
{
    ui->pushButton_3->setEnabled(false);

    if (ui->mbusPrimaryAddressEdit_11->text() != "") {
        emit on_mbusPrimaryWrite_11_clicked();
        _sleep(3000);
    }

#if 0
    if (ui->mbusSecondaryEdit->text() != "") {
        emit on_mbusSecondaryWrite_clicked();
        _sleep(3000);
    }
#endif

    if (ui->mbusReadOutComboBox->currentText() != "") {
        emit on_mbusReadoutWrite_clicked();
        _sleep(3000);
    }

    emit on_mbusTSWrite_clicked();
    _sleep(3000);

    ui->pushButton_3->setEnabled(true);
}


void MainWindow::on_mbusSetReload_clicked()
{
    ui->mbusSetReload->setEnabled(false);

    emit on_mbusPrimaryRead_12_clicked();
    _sleep(3000);

#if 0
    emit on_mbusSecondaryRead_clicked();
    _sleep(3000);
#endif

    emit on_mbusReadoutRead_clicked();
    _sleep(3000);

    emit on_mbusTSRead_clicked();
    _sleep(3000);

    ui->mbusSetReload->setEnabled(true);
}

void MainWindow::mbusPrimaryReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        const QString primAdd = QString::number((unit.value(0) >> 8));
        ui->mbusPrimaryAddressEdit_11->setText(primAdd);

        int index = unit.value(0) & 0x00ff;
        ui->mbusDeviceMode_11->setCurrentIndex(index);
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

void MainWindow::on_mbusPrimaryRead_12_clicked()
{
    if (!modbusDevice)
        return;
    ui->mbusPrimaryAddressEdit_11->clear();
    statusBar()->clearMessage();

    int multiplyTerm = ui->mbusRegister->currentIndex();
    quint16 ADDR = mbusPrimaryAddressBase + multiplyTerm * 20;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, mbusPrimaryAddressEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::mbusPrimaryReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::on_mbusPrimaryWrite_11_clicked()
{
     if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    int multiplyTerm = ui->mbusRegister->currentIndex();
    quint16 ADDR = mbusPrimaryAddressBase + multiplyTerm * 20;

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, mbusPrimaryAddressEntries);
    quint16 unitOne = ((ui->mbusPrimaryAddressEdit_11->text().toInt()) << 8) + ui->mbusDeviceMode_11->currentIndex();

    #ifdef STORE
    if (s_mbusset.s_primary == unitOne){
        return;
    }
    s_mbusset.s_primary = unitOne;
    #endif

    writeUnit.setValue(0, unitOne);

    writeSingleHoldingRegister(writeUnit);
}


void MainWindow::on_mbusSecondaryWrite_clicked()
{
    QStringList s = ui->mbusSecondaryEdit->text().split(' ');
    QVector<quint16> values;

    for (int i = 0; i < s.size(); ++i) {
        if ((i+1) % 2 == 0){
            quint16 temp = s.at(i - 1).toInt(nullptr,16);
            temp = (temp << 8) + s.at(i).toInt(nullptr,16);
            values.push_back(temp);
        }
    }
    int multiplyTerm = ui->mbusRegister->currentIndex();

    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

#ifdef STORE
    // store value
    if (s_mbusset.s_secondary == values) {
        return;
    }
    QVector<quint16> temp(values);
    s_mbusset.s_secondary.swap(temp);
    values.swap(s_mbusset.s_secondary);
#endif

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, multiplyTerm * 20 + mbusSecondaaryAddressBase, mbusSecondarAddressEntries);
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

void MainWindow::mbusSecReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        QString s = "";
        for (uint i = 0; i < unit.valueCount(); i++) {
            if ((unit.value(i) >> 8) > 0x0f) {
                s.append(QString::number(unit.value(i) >> 8, 16).toUpper());
            } else {
                s.append('0');
                s.append(QString::number(unit.value(i) >> 8, 16).toUpper());
            }
            s.append(' ');
            if ((unit.value(i) & 0x00ff) > 0x0f) {
                s.append(QString::number((unit.value(i) & 0x00ff), 16).toUpper());
            }else {
                s.append('0');
                s.append(QString::number((unit.value(i) & 0x00ff), 16).toUpper());
            }
            if (i != (unit.valueCount() - 1)) {
                s.append(' ');
            }
        }
        ui->mbusSecondaryEdit->setText(s);
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

void MainWindow::on_mbusSecondaryRead_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    int multiplyTerm = ui->mbusRegister->currentIndex();
    quint16 ADDR = mbusSecondaaryAddressBase + 20 * multiplyTerm;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, mbusSecondarAddressEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::mbusSecReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}


void MainWindow::on_mbusReadoutWrite_clicked()
{
    quint32 timeoutRead = (ui->mbusReadOutComboBox->currentText().toInt()) * 60;
    QVector<quint16> values;

    for (int i = 1; i >= 0; i--) {
        quint16 temp = 0;
        temp = (timeoutRead >> (i*2*8)) & 0x0000ffff;
        values.push_back(temp);
    }
    int multiplyTerm = ui->mbusRegister->currentIndex();

    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, multiplyTerm * 20 + mbusReadoutIntervalBase, mbusReadoutIntervalEntries);
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

void MainWindow::mbusReadoutReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();

        quint64 data;
        data = unit.value(0);
        data = (data << 16) + unit.value(1);

        int index = data / 60;
        if (index == 15)
            ui->mbusReadOutComboBox->setCurrentIndex(0);
        else if (index == 20)
            ui->mbusReadOutComboBox->setCurrentIndex(1);
        else if (index == 30)
            ui->mbusReadOutComboBox->setCurrentIndex(2);
        else if (index == 60)
            ui->mbusReadOutComboBox->setCurrentIndex(3);
        else
            ui->mbusReadOutComboBox->setCurrentText(QString::number(index));

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

void MainWindow::on_mbusReadoutRead_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    int multiplyTerm = ui->mbusRegister->currentIndex();
    quint16 ADDR = mbusReadoutIntervalBase + 20 * multiplyTerm;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, mbusReadoutIntervalEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::mbusReadoutReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::on_mbusTSWrite_clicked()
{
    QDateTime time = QDateTime::currentDateTime();
    int timeT = time.toTime_t();
    //qDebug("%x", timeT);
    QString StrCurrentTime = time.toString("yyyy-MM-dd hh:mm:ss ddd");
    ui->mbusTimestampEdit->setText(StrCurrentTime);

    quint32 timeoutStamp = timeT;
    QVector<quint16> values;

    for (int i = 1; i >= 0; i--) {
        quint16 temp = 0;
        temp = (timeoutStamp >> (i*2*8)) & 0x0000ffff;
        values.push_back(temp);
    }
    int multiplyTerm = ui->mbusRegister->currentIndex();

    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, multiplyTerm * 20 + mbusTimeStampBase, mbusTimeStampMapEntries);
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

void MainWindow::mbusTimeStampReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();

        quint64 data;
        data = unit.value(0);
        data = (data << 16) + unit.value(1);

        QDateTime time = QDateTime::fromTime_t(data);
        QString StrCurrentTime = time.toString("yyyy-MM-dd hh:mm:ss ddd");
        ui->mbusTimestampEdit->setText(StrCurrentTime);
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

void MainWindow::on_mbusTSRead_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    int multiplyTerm = ui->mbusRegister->currentIndex();
    quint16 ADDR = mbusTimeStampBase + 20 * multiplyTerm;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, mbusTimeStampMapEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::mbusTimeStampReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

