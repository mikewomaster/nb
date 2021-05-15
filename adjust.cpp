#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>

#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "adjust.h"

void MainWindow::handle_write(int currentOutputValue, quint16 add)
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, add, 1);
    writeUnit.setValue(0, currentOutputValue);
    writeSingleHoldingRegister(writeUnit);
}

void MainWindow::handle_write(QLineEdit* l, quint16 add)
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, add, AdjustEntries);
    quint16 currentOutputValue =  l->text().toInt();
    qDebug() << add;
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
void MainWindow::handle_read_ready(QList<meterProfile> mpList, int mpIndex, int mpCol)
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        if (unit.valueCount() == 1) {
            mpList[mpIndex].id = unit.value(0);
        }
        else {
            QString s;
            for (uint i = 0; i < unit.valueCount(); i++) {
                if ((unit.value(i) >> 8) == 0x00)
                    break;
                s[2*i] = unit.value(i) >> 8;
                if ((unit.value(i) & 0x00ff) == 0x00)
                    break;
                s[(2*i) +1] = unit.value(i) & 0x00ff;
            }
            s.remove('\"');

            if (mpCol == 0)
                mpList[mpIndex].tag = s;
            else
                mpList[mpIndex].magnitude = s;
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
void MainWindow::handle_read_ready(QLineEdit* le)
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        short entry = unit.value(0);
        qDebug() << entry;
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

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, addr, AdjustEntries);
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

// VIN Reference Voltage
void MainWindow::vinRefReadReady()
{
    handle_read_ready(ui->vinLineEdit);
}

void MainWindow::on_vinRead_clicked()
{
    handle_read(vinRef, &vinRefReadReady);
}

void MainWindow::on_vinWrite_clicked()
{
    handle_write(ui->vinLineEdit, vinRef);
}

// VOut Reference Voltage
void MainWindow::voutRefReadReady()
{
    handle_read_ready(ui->voutLineEdit);
}

void MainWindow::on_voutWrite_clicked()
{
    handle_write(ui->voutLineEdit, voutRef);
}

void MainWindow::on_voutRead_clicked()
{
    handle_read(voutRef, &voutRefReadReady);
}

//IInp Reference Voltage
void MainWindow::iinRefReadReady()
{
    handle_read_ready(ui->iinLineEdit);
}

void MainWindow::on_iinWrite_clicked()
{
    handle_write(ui->iinLineEdit, iinRef);
}

void MainWindow::on_iinRead_clicked()
{
    handle_read(iinRef, &iinRefReadReady);
}


// Iout Reference Voltage
void MainWindow::on_ioutWrite_clicked()
{
    handle_write(ui->ioutLineEdit, ioutRef);
}

void MainWindow::ioutReadReady()
{
    handle_read_ready(ui->ioutLineEdit);
}

void MainWindow::on_ioutRead_clicked()
{
    handle_read(ioutRef, &ioutReadReady);
}

// factor 1
void MainWindow::on_factor1Write_clicked()
{
    handle_write(ui->factor1LineEdit, adjust1);
}

void MainWindow::factor1ReadReady()
{
    handle_read_ready(ui->factor1LineEdit);
}

void MainWindow::on_factor1Read_clicked()
{
    handle_read(adjust1, &factor1ReadReady);
}

// factor 2
void MainWindow::on_factor2Write_clicked()
{
    handle_write(ui->factor2LineEdit, adjust2);
}

void MainWindow::factor2ReadReady()
{
    handle_read_ready(ui->factor2LineEdit);
}

void MainWindow::on_factor2Read_clicked()
{
    handle_read(adjust2, &factor2ReadReady);
}

// factor 3
void MainWindow::on_factor3Write_clicked()
{
    handle_write(ui->factor3LineEdit, adjust3);
}

void MainWindow::factor3ReadReady()
{
    handle_read_ready(ui->factor3LineEdit);
}

void MainWindow::on_factor3Read_clicked()
{
    handle_read(adjust3, &factor3ReadReady);
}

// factor 4
void MainWindow::on_factor4Write_clicked()
{
    handle_write(ui->factor4LineEdit, adjust4);
}

void MainWindow::factor4ReadReady()
{
    handle_read_ready(ui->factor4LineEdit);
}

void MainWindow::on_factor4Read_clicked()
{
    handle_read(adjust4, &factor4ReadReady);
}

// factor 5
void MainWindow::on_factor5Write_clicked()
{
    handle_write(ui->factor5LineEdit, adjust5);
}

void MainWindow::factor5ReadReady()
{
    handle_read_ready(ui->factor5LineEdit);
}

void MainWindow::on_factor5Read_clicked()
{
    handle_read(adjust5, &factor5ReadReady);
}

// factor 6
void MainWindow::on_factor6Write_clicked()
{
    handle_write(ui->factor6LineEdit, adjust6);
}

void MainWindow::factor6ReadReady()
{
    handle_read_ready(ui->factor6LineEdit);
}

void MainWindow::on_factor6Read_clicked()
{
    handle_read(adjust6, &factor6ReadReady);
}
