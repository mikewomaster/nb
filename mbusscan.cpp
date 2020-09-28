#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QDebug>
#include <QTime>
#include <QMessageBox>

#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "mbusscan.h"
#include "settingsdialog.h"

void MainWindow::on_pushButton_6_clicked()
{
    if (modbusDevice)
        modbusDevice->disconnectDevice();

    if (m_serial == nullptr) {
        m_serial = new QSerialPort;
        m_serial->setPortName(ui->portComboBox->currentText());
        m_serial->setBaudRate(QSerialPort::Baud9600);
        m_serial->setDataBits(QSerialPort::Data8);
        m_serial->setParity(QSerialPort::NoParity);
        m_serial->setStopBits(QSerialPort::OneStop);
        m_serial->setFlowControl(QSerialPort::NoFlowControl);

        if (m_serial->open(QIODevice::ReadWrite)) {
            //QMessageBox::information(this, tr("OK"), "open ok");
        } else {
            //QMessageBox::critical(this, tr("Error"), m_serial->errorString());
        }
    } else {
        m_serial->setPortName(ui->portComboBox->currentText());
        m_serial->setBaudRate(QSerialPort::Baud9600);
        m_serial->setDataBits(QSerialPort::Data8);
        m_serial->setParity(QSerialPort::NoParity);
        m_serial->setStopBits(QSerialPort::OneStop);
        m_serial->setFlowControl(QSerialPort::NoFlowControl);

        if (m_serial->open(QIODevice::ReadWrite)) {
            //QMessageBox::information(this, tr("OK"), "open ok");
        } else {
            //QMessageBox::critical(this, tr("Error"), m_serial->errorString());
        }
    }
    statusBar()->clearMessage();

    QByteArray ba;
    ba.resize(4);
    ba[0] = MAGIC_NUMBER;
    ba[1] = GROUP_MBUS + TYPE_REQ;
    ba[2] = 0x01; // length = 0
    ba[3] = ui->mbusReqEdit->text().toInt(); // address number
    int ret = m_serial->write(ba);
    if (ret == 4) {
        QMessageBox::information(this, tr("OK"), "Scan One Meter to Obtain Data. Done");
    }else {
        QMessageBox::information(this, tr("fail"), "Scan One Meter to Obtain Data. Failed");
    }
    _sleep(100);

    m_serial->close();
    modbusDevice->connectDevice();
}

void MainWindow::mbusCheckResultReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        unsigned char temp_2;;

        temp_2 = unit.value(0) >> 8;
        ui->mbusCmdType->setText(QString::number(temp_2, 16).toUpper());

        temp_2 = (unit.value(0) & 0x00ff);
        ui->mbusCmdRet->setText(QString::number(temp_2,16).toUpper());

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

void MainWindow::on_pushButton_7_clicked()
{
    quint16 ADDR = checkResultAddr;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, checkResultEntries);
    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::mbusCheckResultReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::on_pushButton_11_clicked()
{
    if (modbusDevice)
        modbusDevice->disconnectDevice();

    if (m_serial == nullptr) {
        m_serial = new QSerialPort;
        m_serial->setPortName(ui->portComboBox->currentText());
        m_serial->setBaudRate(QSerialPort::Baud9600);
        m_serial->setDataBits(QSerialPort::Data8);
        m_serial->setParity(QSerialPort::NoParity);
        m_serial->setStopBits(QSerialPort::OneStop);
        m_serial->setFlowControl(QSerialPort::NoFlowControl);

        if (m_serial->open(QIODevice::ReadWrite)) {
            //QMessageBox::information(this, tr("OK"), "open ok");
        } else {
            //QMessageBox::critical(this, tr("Error"), m_serial->errorString());
        }

    } else {
        m_serial->setPortName(ui->portComboBox->currentText());
        m_serial->setBaudRate(QSerialPort::Baud9600);
        m_serial->setDataBits(QSerialPort::Data8);
        m_serial->setParity(QSerialPort::NoParity);
        m_serial->setStopBits(QSerialPort::OneStop);
        m_serial->setFlowControl(QSerialPort::NoFlowControl);

        if (m_serial->open(QIODevice::ReadWrite)) {
            //QMessageBox::information(this, tr("OK"), "open ok");
        } else {
            //QMessageBox::critical(this, tr("Error"), m_serial->errorString());
        }
    }
    statusBar()->clearMessage();

    QByteArray ba;
    ba.resize(4);
    ba[0] = MAGIC_NUMBER;
    ba[1] = GROUP_MBUS + TYPE_SCAN;
    ba[2] = 0x00; // length = 0
    ba[3] = 0x00; // no value

    int ret = m_serial->write(ba);
    if (ret == 4) {
        QMessageBox::information(this, tr("OK"), "write cmd ok");
    }else {
        QMessageBox::information(this, tr("fail"), "write fail");
    }
    _sleep(100);

    m_serial->close();
    modbusDevice->connectDevice();
}

