#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QDebug>
#include <QTime>
#include <QMessageBox>

#include "pidtab.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "nettab.h"

// sourceID r/w
void MainWindow::netIdReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        const QString entry = QString::number(double(unit.value(0)));
        ui->srcIDLineEdit->setText(entry);
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

void MainWindow::on_netSIDRead_clicked()
{
    if (!modbusDevice)
        return;
    ui->srcIDLineEdit->clear();
    statusBar()->clearMessage();

    quint16 ADDR = netSourceIDAddr;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, 1);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::netIdReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::on_netSIDWrite_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    quint16 ADDR = netSourceIDAddr;
    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, 1);
    quint16 netsid = ui->srcIDLineEdit->text().toInt();
    writeUnit.setValue(0, netsid);
    writeSingleHoldingRegister(writeUnit);
}

// aes enable r/w
void MainWindow::aesEnableReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        quint32 entry = unit.value(0);
        ui->aesComboBox->setCurrentIndex(entry);
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

void MainWindow::on_netAesEnableRead_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    quint16 ADDR = netAESEnableAddr;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, 1);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::aesEnableReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::on_netAesEnableWrite_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    quint16 ADDR = netAESEnableAddr;
    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, 1);
    quint16 aesEnable = ui->aesComboBox->currentIndex();
    writeUnit.setValue(0, aesEnable);
    writeSingleHoldingRegister(writeUnit);
}

// response timeout r/w
void MainWindow::netTimeoutReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        quint32 entry = unit.value(0);
        ui->ResTOutlineEdit->setText(QString::number(entry));
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

void MainWindow::on_netTimeoutRead_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    quint16 ADDR = netTimeOutAddr;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, 1);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::netTimeoutReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::on_netTimeoutWrite_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    quint16 ADDR = netTimeOutAddr;
    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, 1);
    quint16 value = ui->ResTOutlineEdit->text().toInt();
    writeUnit.setValue(0, value);
    writeSingleHoldingRegister(writeUnit);
}

// aes key r/w
void MainWindow::aesKeyReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        QString s;
        for (uint i = 0; i < unit.valueCount(); i++) {
            s[2*i] = unit.value(i) >> 8;
            s[(2*i) +1] = unit.value(i) & 0x00ff;
        }
        ui->aesLineEdit->setText(s);
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

void MainWindow::on_netAesKeyRead_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    quint16 ADDR = netAESKeyAddr;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, 8);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::aesKeyReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::on_aesComboBox_currentIndexChanged(int index)
{
    if(index == 1){
        int max = 16;//字符串长度
        QString tmp = QString("0123456789abcdefghijklmnopqrstuvwxyz");
        QString str;
        QTime t;
        t = QTime::currentTime();
        qsrand(t.msec()+t.second()*1000);
        for(int i=0;i<max;i++)
        {
           int len = qrand()%tmp.length();
           str[i] = tmp.at(len);
        }

        ui->aesLineEdit->setText(str);
    }
}

void MainWindow::on_netAesKeyWrite_clicked()
{
    QString str = ui->aesLineEdit->text();
    if (str.size() != 16) {
        QMessageBox::information(NULL, "Error","Length of AES Key Should be 16!");
    }
    QVector<quint16> values;
    for (int i = 0; i < str.size(); i++) {
        if ((i+1) % 2 == 0) {
            quint16 temp = str.at(i - 1).toLatin1();
            temp = (temp << 8) + str.at(i).toLatin1();
            values.push_back(temp);
        }
    }

    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, netAESKeyAddr, 8);

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


// bit map r/w
void MainWindow::bitMapReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {

        QList<Device> recordList;
        for (int i = 1; i <= 0xff; ++i)
        {
            Device record;
            record.bChecked = false;
            record.id = i;
            recordList.append(record);
        }

        const QModbusDataUnit unit = reply->result();

        ui->BitMapTextEdit->clear();
        for (uint i = 0; i < unit.valueCount(); i++) {
            if (unit.value(i) != 0) {
                unsigned char byteHigh = unit.value(i) >> 8;
                unsigned char byteLow = unit.value(i);

                for (int j = 0; j < 8; j++) {
                    if ( (byteLow & (1 << j)) != 0 ) {
                        int k = (i*16) + (j+1);
                        Device temp = recordList[k-1];
                        temp.bChecked = true;
                        recordList.replace(k-1,temp);
                        //ui->BitMapTextEdit->append(QString::number(k));
                    }
                }

                for (int j = 0; j < 8; j++) {
                    if ( (byteHigh & (1 << j)) != 0 ) {
                        int k = (i*16) + (j+9);
                        Device temp = recordList[k-1];
                        temp.bChecked = true;
                        recordList.replace(k-1,temp);
                        //ui->BitMapTextEdit->append(QString::number(k));
                    }
                }
            }
        }
        m_pModel->updateData(recordList);
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

void MainWindow::on_netBitMapRead_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    quint16 ADDR = netBitMapAddr;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, 16);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::bitMapReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::on_netBitMapWrite_clicked()
{
    // QString text = ui->BitMapTextEdit->toPlainText();
    // QStringList textList = text.split("\n");

    QStringList textList;
    for(int i = 0; i < 0xff; i++)
    {
        QModelIndex index = m_pModel->index(i, 0);
        if (m_pModel->data(index, Qt::CheckStateRole).toBool()) {
            textList.push_back(QString::number(i + 1));
        }
    }

    unsigned char temp[32];
    memset(temp, 0, 32);
    for (int i = 0; i < textList.size(); i++) {
        int move = textList.at(i).toInt();
        if ((move-1) % 16 < 8) {
            temp[(move-1)/8 + 1] = temp[(move-1)/8 + 1] + (1 << ((move-1)%8)); // 低字节在后
        } else if ((move-1) % 16 >= 8) {
            temp[(move-1)/8 - 1] = temp[(move-1)/8 - 1] + (1 << ((move-1)%8)); // 高字节在前
        }
    }

    QVector<quint16> values;
    for (int i = 0; i < 32; i++) {
        if ((i+1) % 2 == 0) {
            quint16 value = temp[i-1];
            value = (value << 8) + temp[i];
            values.push_back(value);
        }
    }

    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, netBitMapAddr, 16);
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

void MainWindow::SNReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        QString s;
        for (uint i = 0; i < unit.valueCount(); i++) {
            s[2*i] = unit.value(i) >> 8;
            s[(2*i) +1] = unit.value(i) & 0x00ff;
        }
        ui->SNLineEdit->setText(s);
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

void MainWindow::on_SNPushButton_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    quint16 ADDR = SNAddr;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, SNEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::SNReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}


void MainWindow::on_SNPushButtonWrite_clicked()
{
    nb_handle_write(ui->SNLineEdit, SNAddr, SNEntries);
}

void MainWindow::on_abpEnable_clicked()
{
    if (ui->abpEnable->isChecked()){
        ui->otaaEnable->setChecked(false);
        handle_write(ui->abpEnable, loraWANEnable);
    }
}

void MainWindow::on_otaaEnable_clicked()
{
    if (ui->otaaEnable->isChecked()){
        ui->abpEnable->setChecked(false);
        handle_write(ui->otaaEnable, loraWANEnable);
    }
}
