#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QDebug>

#include "ui_mainwindow.h"
#include "mainwindow.h"

void MainWindow::nbStatusFill(short res, QLineEdit *le)
{
   switch(res)
   {
    case -1:
      le->setText("CEL_ERROR");
      break;
   case 0:
      le->setText("CEL_NOT_READY");
      break;
   case 1:
      le->setText("CEL_PORT_READY");
      break;
   case 2:
      le->setText("CEL_MODULE_READY");
      break;
   case 3:
      le->setText("CEL_SIM_NOT_READY");
      break;
   case 4:
      le->setText("CEL_PIN_REQUEST");
      break;
   case 5:
      le->setText("CEL_SIM_READY");
      break;
   case 6:
      le->setText("CEL_DISCONNECTED");
      break;
   case 7:
      le->setText("CEL_CONNECTED");
      break;
   }
}

void MainWindow::mqttStatusFill(short res, QLineEdit *le)
{
    switch(res)
    {
        case 0:
            le->setText("DISCONNECTED");
            break;
        case 1:
            le->setText("CONNECTED");
            break;
    }
}

void MainWindow::handle_write(QLineEdit* l, quint16 add)
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, add, 1);
    quint16 currentOutputValue =  l->text().toInt();
    writeUnit.setValue(0, currentOutputValue);
    writeSingleHoldingRegister(writeUnit);
}

void MainWindow::handle_write(QRadioButton *r, quint16 add)
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, add, 1);

    quint16 currentOutputValue;
    if (r == ui->abpEnable){
        currentOutputValue = 0;
    }else if (r == ui->otaaEnable){
        currentOutputValue = 1;
    }

    writeUnit.setValue(0, currentOutputValue);
    writeSingleHoldingRegister(writeUnit);
}

void MainWindow::handle_write(QModbusDataUnit writeUnit)
{
    if (!modbusDevice)
        return;

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
            reply->deleteLater();
        }
    } else {
        statusBar()->showMessage(tr("Write error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::handle_read_ready(QLineEdit* le)
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        short entry = unit.value(0);
        if (le == ui->nbStatusLineEdit) {
            nbStatusFill(entry, ui->nbStatusLineEdit);
        } else if (le == ui->mqttStatusLineEdit){
            mqttStatusFill(entry, ui->mqttStatusLineEdit);
        } else {
              le->setText(QString::number(entry));
        }
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

void MainWindow::handle_read(int addr, void (MainWindow::*fp)())
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, addr, 1);
    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, fp);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::handle_read(int addr, int entry, void (MainWindow::*fp)())
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, addr, entry);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, fp);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::prepareSendModbusUnit(int addressToSend, int entryToSend, quint16 valueToSend)
{
    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, addressToSend, entryToSend);
    writeUnit.setValue(0, valueToSend);
    handle_write(writeUnit);
}
