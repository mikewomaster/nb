#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>

#include "ui_mainwindow.h"
#include "mainwindow.h"
/*
    Serial Configuration Setting
*/
void MainWindow::on_serialBtn_2_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, SerialStartAddr, (SerialEntries - 1));

    int baudRate = ui->s_baudCombo_2->currentIndex();
    int dataBit = ui->s_dataBitsCombo_2->currentText().toInt();
    int stopBit = ui->s_stopBitsCombo_2->currentText().toInt();
    int parity = ui->s_parityCombo_2->currentIndex();

     writeUnit.setValue(0, baudRate);
     writeUnit.setValue(1, dataBit);
     writeUnit.setValue(2, stopBit);
     writeUnit.setValue(3, parity);

    if (auto *reply = modbusDevice->sendWriteRequest(writeUnit, ui->serverEdit->value())) {

        connect(reply, &QModbusReply::finished, this, [this, reply]() {

            if (reply->error() == QModbusDevice::ProtocolError) {
                qDebug() << "1";
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

        if (!reply->isFinished()) {

        } else {
            // broadcast replies return immediately
            reply->deleteLater();
        }
    } else {
        statusBar()->showMessage(tr("Write error: ") + modbusDevice->errorString(), 5000);
    }
}
void MainWindow::serialReadReady2()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
            const QModbusDataUnit unit = reply->result();

            quint32 baudRate = unit.value(0);
            ui->s_baudCombo->setCurrentIndex(baudRate);

            quint32 dataBit = unit.value(1);
            ui->s_dataBitsCombo->setCurrentText(QString::number(dataBit));

            quint32 stopBit = unit.value(2);
            ui->s_stopBitsCombo->setCurrentText(QString::number(stopBit));

            quint32 parity = unit.value(3);
            ui->s_parityCombo->setCurrentIndex(parity);

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

void MainWindow::on_serialBtnRead_2_clicked()
{
    if (!modbusDevice)
        return;

    statusBar()->clearMessage();

    quint16 ADDR = SerialStartAddr;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, (SerialEntries - 1));

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::serialReadReady2);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}
