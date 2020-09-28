#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QDebug>

#include "ui_mainwindow.h"
#include "mainwindow.h"

void MainWindow::loraReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
            const QModbusDataUnit unit = reply->result();

            double freq = unit.value(0) / 10.0;
            qDebug() << freq;
            ui->FrequencyComboBox->setCurrentText(QString::number(freq,'g',4));

            quint32 rf = unit.value(1);
            ui->RFPowerComboBox->setCurrentText(QString::number(rf));

            quint32 sf = unit.value(2);
            ui->SFComboBox->setCurrentIndex(sf);

            quint32 bd = unit.value(3);
            ui->BandwidthComboBox->setCurrentIndex(bd);

            quint32 cr = unit.value(4) - 1;
            ui->CodeRateComboBox->setCurrentIndex(cr);

            // hide
            //quint32 or_ = unit.value(5);
            //ui->OptimizeLowRateComboBox->setCurrentIndex(or_);

            // hide
            //quint32 sw = unit.value(6);
            //QString sw_s = QString::number(sw, 16);
            //ui->syncWordLineEdit->setText(sw_s);

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

void MainWindow::on_loraGetButton_clicked()
{
    if (!modbusDevice)
        return;

    statusBar()->clearMessage();

    quint16 ADDR = LoraModbusStartAddr;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, LoraModbusEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::loraReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

/*
    Lora Configuration Setting
*/
void MainWindow::on_loraSetButton_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, LoraModbusStartAddr, LoraModbusEntries);

    int freq = (ui->FrequencyComboBox->currentText().toDouble())*10;
    int rf = ui->RFPowerComboBox->currentText().toFloat();
    int sf = ui->SFComboBox->currentIndex();
    int bandwidth = ui->BandwidthComboBox->currentIndex();
    int cr = ui->CodeRateComboBox->currentIndex() + 1;
    // change to default value
    //int optim_low_rate = ui->OptimizeLowRateComboBox->currentIndex();
    //int sync_word = ui->syncWordLineEdit->text().toUInt(nullptr, 16);
    int optim_low_rate = 0;
    int sync_word = 0x3434;
     //qDebug("%d %d %d %d %d %d %d %d %d", slaveAddressId, masterAddressId, freq, rf, sf, bandwidth, cr, optim_low_rate, sync_word);

     writeUnit.setValue(0, freq);
     writeUnit.setValue(1, rf);
     writeUnit.setValue(2, sf);
     writeUnit.setValue(3, bandwidth);
     writeUnit.setValue(4, cr);
     writeUnit.setValue(5, optim_low_rate);
     writeUnit.setValue(6, sync_word);

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
