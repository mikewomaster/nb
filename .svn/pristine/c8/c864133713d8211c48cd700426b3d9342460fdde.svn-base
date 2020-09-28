#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QDebug>

#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "lorawan.h"

// abp Dev Addr
void MainWindow::on_abpDevAddrWrite_clicked()
{
    QStringList s = ui->abpDevAddrLine->text().split(' ');
    QVector<quint16> values;

    for (int i = 0; i < s.size(); ++i) {
        if ((i+1) % 2 == 0){
            quint16 temp = s.at(i - 1).toInt(nullptr,16);
            temp = (temp << 8) + s.at(i).toInt(nullptr,16);
            values.push_back(temp);
        }
    }

    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ABPDevAddr, abpDevEntries);
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

void MainWindow::abpDevAddrReadReady()
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
        ui->abpDevAddrLine->setText(s);
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

void MainWindow::on_abpDevAddrRead_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    quint16 ADDR = ABPDevAddr;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, abpDevEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::abpDevAddrReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

// abp APPs Key
void MainWindow::on_abpAppsKeyWrite_clicked()
{
    QStringList s = ui->abpAppsKeyLine->text().split(' ');
    QVector<quint16> values;

    for (int i = 0; i < s.size(); ++i) {
        if ((i+1) % 2 == 0){
            quint16 temp = s.at(i - 1).toInt(nullptr,16);
            temp = (temp << 8) + s.at(i).toInt(nullptr,16);
            values.push_back(temp);
        }
    }

    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ABPAppsKeyAddr, abpAppsKeyEntries);
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

void MainWindow::abpAppsKeyReadReady()
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
        ui->abpAppsKeyLine->setText(s);
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

void MainWindow::on_abpAppsKeyRead_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    quint16 ADDR = ABPAppsKeyAddr;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, abpAppsKeyEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::abpAppsKeyReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

// abp nwks key
void MainWindow::on_abpNwksKeyWrite_clicked()
{
    QStringList s = ui->abpNswksKeyLine->text().split(' ');
    QVector<quint16> values;

    for (int i = 0; i < s.size(); ++i) {
        if ((i+1) % 2 == 0){
            quint16 temp = s.at(i - 1).toInt(nullptr,16);
            temp = (temp << 8) + s.at(i).toInt(nullptr,16);
            values.push_back(temp);
        }
    }

    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ABPNwksKeyAddr, abpNwksKeyEntries);
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

void MainWindow::abpNwksKeyReadReady()
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
        ui->abpNswksKeyLine->setText(s);
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

void MainWindow::on_abpNwksKeyRead_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    quint16 ADDR = ABPNwksKeyAddr;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, abpNwksKeyEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::abpNwksKeyReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

//otaa dev eui
void MainWindow::on_otaaDevEuiWrite_clicked()
{
    QStringList s = ui->otaaDevEuiLine->text().split(' ');
    QVector<quint16> values;

    for (int i = 0; i < s.size(); ++i) {
        if ((i+1) % 2 == 0){
            quint16 temp = s.at(i - 1).toInt(nullptr,16);
            temp = (temp << 8) + s.at(i).toInt(nullptr,16);
            values.push_back(temp);
        }
    }

    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, OTAADevEuiAddr, otaaDevEuiEntries);
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

void MainWindow::otaaDevEuiReadReady()
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
        ui->otaaDevEuiLine->setText(s);
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

void MainWindow::on_otaaDevEuiRead_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    quint16 ADDR = OTAADevEuiAddr;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, otaaDevEuiEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::otaaDevEuiReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

//otaa app eui
void MainWindow::on_otaaAppEuiWrite_clicked()
{
    QStringList s = ui->otaaAppEuiLine->text().split(' ');
    QVector<quint16> values;

    for (int i = 0; i < s.size(); ++i) {
        if ((i+1) % 2 == 0){
            quint16 temp = s.at(i - 1).toInt(nullptr,16);
            temp = (temp << 8) + s.at(i).toInt(nullptr,16);
            values.push_back(temp);
        }
    }

    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, OTAAAppEuiAddr, otaaAppEuiEntries);
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

void MainWindow::otaaAppEuiReadReady()
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
        ui->otaaAppEuiLine->setText(s);
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

void MainWindow::on_otaaAppEuiRead_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    quint16 ADDR = OTAAAppEuiAddr;
    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, otaaAppEuiEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::otaaAppEuiReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

//otaa app key
void MainWindow::on_otaaAppKeyWrite_clicked()
{
    QStringList s = ui->otaaAppKeyLine->text().split(' ');
    QVector<quint16> values;

    for (int i = 0; i < s.size(); ++i) {
        if ((i+1) % 2 == 0){
            quint16 temp = s.at(i - 1).toInt(nullptr,16);
            temp = (temp << 8) + s.at(i).toInt(nullptr,16);
            values.push_back(temp);
        }
    }

    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, OTAAAppKeyAddr, otaaAppKeyEntries);
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

void MainWindow::otaaAppKeyReadReady()
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
        ui->otaaAppKeyLine->setText(s);
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

void MainWindow::on_otaaAppKeyRead_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    quint16 ADDR = OTAAAppKeyAddr;
    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, otaaAppKeyEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::otaaAppKeyReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}
