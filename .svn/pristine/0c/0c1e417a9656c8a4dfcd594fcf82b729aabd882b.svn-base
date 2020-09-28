#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>

#include "pidtab.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"

void MainWindow::on_pidButtonWrite_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, PIDStartAddr, PIDEntries);

    int crl = ui->pidControlComboBox->currentIndex();
    int temperature = ui->temperatureLineEdit->text().toUInt();
    int pidMin = ui->pidMinLineEdit->text().toUInt();
    int pidMax = ui->pidMaxLineEdit->text().toUInt();
    int kp = ui->pidKpLineEdit->text().toUInt();
    int Ti = ui->pidTiLineEdit->text().toUInt();
    int Td = ui->pidTdLineEdit->text().toUInt();

     writeUnit.setValue(0, crl);
     writeUnit.setValue(1, temperature);
     writeUnit.setValue(2, pidMax);
     writeUnit.setValue(3, pidMin);
     writeUnit.setValue(4, kp);
     writeUnit.setValue(5, Ti);
     writeUnit.setValue(6, Td);

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

void MainWindow::on_pidButtonRead_clicked()
{
    if (!modbusDevice)
        return;

    ui->temperatureLineEdit->clear();
    ui->pidMinLineEdit->clear();
    ui->pidMaxLineEdit->clear();
    ui->pidTiLineEdit->clear();
    ui->pidTdLineEdit->clear();
    ui->pidKpLineEdit->clear();

    statusBar()->clearMessage();

    quint16 ADDR = PIDStartAddr;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, PIDEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::PIDReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::PIDReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
            const QModbusDataUnit unit = reply->result();

            quint32 ctrl = unit.value(0);
            ui->pidControlComboBox->setCurrentIndex(ctrl);

            quint32 temperature = unit.value(1);
            ui->temperatureLineEdit->setText(QString::number(temperature));

            quint32 max = unit.value(2);
            ui->pidMaxLineEdit->setText(QString::number(max));

            quint32 min = unit.value(3);
            ui->pidMinLineEdit->setText(QString::number(min));

            quint32 kp = unit.value(4);
            ui->pidKpLineEdit->setText(QString::number(kp));

            quint32 ti = unit.value(5);
            ui->pidTiLineEdit->setText(QString::number(ti));

            quint32 td = unit.value(6);
            ui->pidTdLineEdit->setText(QString::number(td));

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
