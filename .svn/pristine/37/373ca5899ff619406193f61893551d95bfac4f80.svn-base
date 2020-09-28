#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>

#include "ui_mainwindow.h"
#include "mainwindow.h"


void MainWindow::on_currentOutputPushButton_clicked()
{
     if (!modbusDevice)
         return;
     statusBar()->clearMessage();

     quint16 channel = ui->currentOutputComboBox->currentText().toInt();
     quint16 ADDR = currentOutputAddr + channel;
     QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, IOEntries);

     //quint16 currentOutputValue = ui->currentOutputLineEdit->text().toDouble() * 1000;
     float temp =  ui->currentOutputLineEdit->text().toFloat();
     uint currentOutputValue = temp * 1000;

     if (currentOutputValue < 4000) {
         statusBar()->showMessage("Set Current Error: current output must bigger than 4mA");
         return;
      }

     writeUnit.setValue(0, currentOutputValue);
     writeSingleHoldingRegister(writeUnit);
}

void MainWindow::on_voltageOutputPushButton_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    quint16 channel = ui->voltageOutputcomboBox->currentText().toInt();
    quint16 ADDR = voltageOutputAddr + channel;
    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, IOEntries);
    quint16 voltageOutputValue = ui->voltageOutputLineEdit->text().toDouble();
    if (voltageOutputValue > 30000) {
        statusBar()->showMessage("Set Voltage Error: voltage output must less than 10000mv");
        return;
    }
    writeUnit.setValue(0, voltageOutputValue);
    writeSingleHoldingRegister(writeUnit);
}

void MainWindow::on_PWMOutputPushButton_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    quint16 channel = ui->PWMOutputComboBox->currentText().toInt();
    quint16 ADDR = PWMOutputAddr + channel*2;
    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, PWMIOEntries);

    quint32 fre = ui->PWMOutputlineEdit->text().toULong();
    if (fre > 100000) {
        statusBar()->showMessage("Set Frequency Error: frequency output must less than 100000Hz");
        return;
    }

    quint8 ocu = ui->PWMOutputLineEdit2->text().toInt();
    if (ocu > 100){
        statusBar()->showMessage("Set Occupu Error: occupy output must less than 100%");
        return;
    }

    // occupy 3 bytes for fre.
    // quint16 valueOne = (fre >> 8);
    // quint16 valuetwo = ((fre & 0x000000ff) << 8) + ocu;

    quint16 valueOne = fre;
    quint16 valuetwo = ocu;

    writeUnit.setValue(0, valueOne);
    writeUnit.setValue(1, valuetwo);

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

void MainWindow::currentReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        const QString entry = QString::number(double(unit.value(0)/1000.0),'g', 5);
        ui->currentInputLineEdit->setText(entry);
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

void MainWindow::on_currentInputPushButton_clicked()
{
    if (!modbusDevice)
        return;
    ui->currentInputLineEdit->clear();
    statusBar()->clearMessage();

    quint16 channel = ui->currentInputComboBox->currentText().toInt();
    quint16 ADDR = currentInputAddr + channel;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, IOEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::currentReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::voltageReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        const QString entry = QString::number(unit.value(0));
        ui->voltageInputLineEdit->setText(entry);
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

void MainWindow::on_voltageInputPushButton_clicked()
{
    if (!modbusDevice)
        return;
    ui->voltageInputLineEdit->clear();
    statusBar()->clearMessage();

    quint16 channel = ui->voltageInputComboBox->currentText().toInt();
    quint16 ADDR = voltageInputAddr + channel;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, IOEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::voltageReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::PWMReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        //occupy 3 digits
        //quint32 tempOne = unit.value(0);
        //quint16 tempTwo = unit.value(1);
        //quint32 valueOne = (tempOne << 8) + (tempTwo & 0xff00);
        //quint16 valueTwo = tempTwo & 0x00ff;

        quint16 valueOne = unit.value(0);
        quint16 valueTwo = unit.value(1);
        QString entryOne = QString::number(valueOne);
        QString entryTwo = QString::number(valueTwo);
        ui->PWMInputlineEdit->setText(entryOne);
        ui->PWMInputlineEdit2->setText(entryTwo);

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

void MainWindow::on_PWMInputPushButton_clicked()
{
    if (!modbusDevice)
        return;
    ui->PWMInputlineEdit->clear();
    statusBar()->clearMessage();

    quint16 channel = ui->PWMInputComboBox->currentText().toInt();
    quint16 ADDR = PWMInputAddr + channel*2;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, PWMIOEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::PWMReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::thermocoupleReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        const QString entry = QString::number(unit.value(0));
        ui->thermocopuleLineEdit->setText(entry);
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

void MainWindow::on_thermocoupleInputPushButton_clicked()
{
    if (!modbusDevice)
        return;
    ui->thermocopuleLineEdit->clear();
    statusBar()->clearMessage();

    quint16 channel = ui->thermocoupleComboBox->currentText().toInt();
    quint16 ADDR = thermocoupleAddr + channel;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, IOEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::thermocoupleReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::currentOutputReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        const QString entry = QString::number(double(unit.value(0)/1000.0),'g',5);
        ui->currentOutputLineEdit->setText(entry);
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

void MainWindow::on_currentOutputReadPushButton_clicked()
{
    if (!modbusDevice)
        return;
    ui->currentOutputLineEdit->clear();
    statusBar()->clearMessage();

    quint16 channel = ui->currentOutputComboBox->currentText().toInt();
    quint16 ADDR = currentOutputAddr + channel;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, IOEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::currentOutputReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::PWMOutputReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
            const QModbusDataUnit unit = reply->result();
            // occupy 3 digits
            //quint32 tempOne = unit.value(0);
            //quint16 tempTwo = unit.value(1);
            //quint32 valueOne = (tempOne << 8) + (tempTwo & 0xff00);
            //quint16 valueTwo = tempTwo & 0x00ff;

            quint16 valueOne = unit.value(0);
            quint16 valueTwo = unit.value(1);

            QString entryOne = QString::number(valueOne);
            QString entryTwo = QString::number(valueTwo);
            ui->PWMOutputlineEdit->setText(entryOne);
            ui->PWMOutputLineEdit2->setText(entryTwo);
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

void MainWindow::on_PwmOutputReadPushButton_clicked()
{
    if (!modbusDevice)
        return;
    ui->PWMOutputlineEdit->clear();
    statusBar()->clearMessage();

    quint16 channel = ui->PWMOutputComboBox->currentText().toInt();
    quint16 ADDR = PWMOutputAddr + channel * 2;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, PWMIOEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::PWMOutputReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::voltageOutputReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        const QString entry = QString::number(unit.value(0));
        ui->voltageOutputLineEdit->setText(entry);
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

void MainWindow::on_rvPshBtn_clicked()
{
    if (!modbusDevice)
        return;
    ui->voltageOutputLineEdit->clear();
    statusBar()->clearMessage();

    quint16 channel = ui->voltageOutputcomboBox->currentText().toInt();
    quint16 ADDR = voltageOutputAddr + channel;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, IOEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::voltageOutputReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}
