#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QDebug>
#include <QMessageBox>

#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "mqtt.h"

void MainWindow::regStatusFille(short res, QLineEdit *le)
{
    switch (res) {
    case 1:
        le->setText("Registered");
        break;
    default:
        le->setText("Not_Registered");
        break;
    }
}

void MainWindow::simStatusFill(short res, QLineEdit *le)
{
    switch(res)
    {
     case 0:
       le->setText("SIM_NOT_INSERTED");
       break;
    case 1:
       le->setText("SIM_READY");
       break;
    case 2:
       le->setText("SIM_NOT_READY");
       break;
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
      le->setText("CEL_SIM_ERROR");
      break;
   case 1:
      le->setText("CEL_CONNECTED");
      break;
   case 2:
      le->setText("CEL_SEARCHING");
      break;
   case 3:
      le->setText("CEL_DENIED");
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

void MainWindow::handle_read_ready_divide_ten(QLineEdit * le)
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        double entry = unit.value(0) / 100.0;

        le->setText(QString::number(entry, 'g', 3));
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
        if (le == ui->nbStatusLineEdit) {
            nbStatusFill(entry, ui->nbStatusLineEdit);
        } else if (le == ui->mqttStatusLineEdit){
            mqttStatusFill(entry, ui->mqttStatusLineEdit);
        } else if (le == ui->simStatusLineEdit) {
            simStatusFill(entry, ui->simStatusLineEdit);
        } else if (le == ui->regisLineEdit) {
            regStatusFille(entry, ui->regisLineEdit);
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

void MainWindow::handle_read_ready(QLineEdit *le, int type)
{
    Q_UNUSED(type); // fix me: for more parse purpose

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
        le->setText(StrCurrentTime);
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

void MainWindow::handle_read_ready(QComboBox* cb)
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        int entry = unit.value(0);
        cb->setCurrentIndex(entry);
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

void MainWindow::handle_read_ready(QComboBox *cb, QLineEdit *le)
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        ushort entry = unit.value(0);
        uchar valueOne = entry >> 8;
/*
        uchar valueTwo = entry && 0x00ff;
        QString strMsg = QString::number(entry);
        QString strMsg2 = QString::number(valueTwo);
        QString msg = strMsg + " : " + strMsg2;
        QMessageBox::information(NULL, "Debug", msg, QMessageBox::Yes | QMessageBox::No);
*/
        int valueTwo = entry;
        valueTwo > 100 ? valueTwo = (valueTwo - 256) : valueTwo = valueTwo;
        cb->setCurrentIndex(valueOne);
        le->setText(QString::number(valueTwo));

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

void MainWindow::handle_read_ready(QComboBox *cb, QComboBox *cb2)
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        ushort entry = unit.value(0);
        uchar valueOne = entry >> 8;
        uchar valueTwo = entry && 0x00ff;

        cb->setCurrentText(QString::number(valueOne));
        cb2->setCurrentText(QString::number(valueTwo));

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

void MainWindow::nb_handle_write(QLineEdit* le, int addr, int entry)
{
    QString str = le->text();

    QVector<quint16> values;

    int i = 0;
    for (i = 0; i < str.size(); i++) {
        if ((i+1) % 2 == 0) {
            quint16 temp = str.at(i - 1).toLatin1();
            temp = (temp << 8) + str.at(i).toLatin1();
            values.push_back(temp);
        }
    }

    if (i % 2 && i) {
        quint16 temp = str.at(i-1).toLatin1();
        temp = temp << 8;
        values.push_back(temp);
        i++;
    }

    if (addr != mqttTopicAddress && addr != MQTTSubTopicAddress) {
        for (i = (i / 2); i < entry; i++) {
            values.push_back(0x0000);
        }
    }

    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, addr, entry);

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

    if (le == ui->idLineEdit)
        qDebug() << values.size();
}

void MainWindow::nb_handle_read_ready(QLineEdit *le)
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
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
        le->setText(s);
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

void MainWindow::prepareSendModbusUnit(int addressToSend, int entryToSend, quint16 valueToSend)
{
    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, addressToSend, entryToSend);
    writeUnit.setValue(0, valueToSend);
    handle_write(writeUnit);
}
