#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QDebug>

#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "mqtt.h"
#include "commanhelper.h"

//srv ip
void MainWindow::on_srvWrite_clicked()
{
    nb_handle_write(ui->srvLineEdit, mqttIPAddress, mqttIPEntries);
}

void MainWindow::srvReadReady()
{
    nb_handle_read_ready(ui->srvLineEdit);
}

void MainWindow::tcpSrvReadReady()
{
    nb_handle_read_ready(ui->tcpServerLineEdit);
}

void MainWindow::on_srvRead_clicked()
{
    handle_read(mqttIPAddress, mqttIPEntries, &srvReadReady);
}

// port
void MainWindow::on_portWrite_clicked()
{
    handle_write(ui->portLineEdit, mqttPortAddress);
}

void MainWindow::portReadReady()
{
    handle_read_ready(ui->portLineEdit);
}

void MainWindow::tcpPortReadReady()
{
    handle_read_ready(ui->tcpPortLineEdit);
}

void MainWindow::on_portRead_clicked()
{
    handle_read(mqttPortAddress, &portReadReady);
}

// topic
void MainWindow::on_topicWrite_clicked()
{
    nb_handle_write(ui->topicLineEdit, mqttTopicAddress, mqttTopicEntreis);
}

void MainWindow::topicReadReady()
{
    nb_handle_read_ready(ui->topicLineEdit);
}

void MainWindow::subTopicalReadReady()
{
    nb_handle_read_ready(ui->subTopicLineEdit);
}

void MainWindow::on_topicRead_clicked()
{
    handle_read(mqttTopicAddress, mqttTopicEntreis, &topicReadReady);
}

// client id
void MainWindow::on_idWrite_clicked()
{
    nb_handle_write(ui->idLineEdit, mqttClinetIDAddress, mqttClinetIDEntries);
}

void MainWindow::idReadReady()
{
    nb_handle_read_ready(ui->idLineEdit);
}

void MainWindow::on_idRead_clicked()
{
     handle_read(mqttClinetIDAddress, mqttClinetIDEntries, &idReadReady);
}

// mqtt user
void MainWindow::mqttUserNameReadReady()
{
    nb_handle_read_ready(ui->mqttUserNameLineEdit);
}

// mqtt password
void MainWindow::mqttPassWordReadReady()
{
    nb_handle_read_ready(ui->mqttPasswordLineEdit);
}

void MainWindow::on_pidButtonWrite_8_clicked()
{
    handle_write(ui->mqttStatusLineEdit, mqttStatusAddress);
}

void MainWindow::mqttStatuReadReady()
{
    handle_read_ready(ui->mqttStatusLineEdit);
}

void MainWindow::on_mqttStatusRead_clicked()
{
    handle_read(mqttStatusAddress, &mqttStatuReadReady);
}

// interval

void MainWindow::intervalReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();

        quint64 data;
        data = unit.value(0);
        data = (data << 16) + unit.value(1);       
        ui->intervalLineEdit->setText(QString::number(data));
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

void MainWindow::on_intervalRead_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    quint16 ADDR = mqttIntervalAddress;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, mqttIntervalEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::intervalReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::on_idWrite_2_clicked()
{
    quint32 timeoutRead = ui->intervalLineEdit->text().toUInt();
    QVector<quint16> values;

    for (int i = 1; i >= 0; i--) {
        quint16 temp = 0;
        temp = (timeoutRead >> (i*2*8)) & 0x0000ffff;
        values.push_back(temp);
    }

    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, mqttIntervalAddress, mqttIntervalEntries);
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

void MainWindow::on_tcpApply_clicked()
{
     if (ui->tcpServerLineEdit->text() != "") {
        nb_handle_write(ui->tcpServerLineEdit, TCPIPAddress, TCPIPEntries);
        _sleep(2000);
     }

    if (ui->tcpPortLineEdit->text() != "") {
        handle_write(ui->tcpPortLineEdit, TCPPortAddress);
        _sleep(2000);
    }
}

void MainWindow::on_tcpReload_clicked()
{
    handle_read(TCPIPAddress, TCPIPEntries, &tcpSrvReadReady);
    _sleep(2000);

    handle_read(TCPPortAddress, &tcpPortReadReady);
    _sleep(2000);
}

void MainWindow::on_mqttApply_clicked()
{
    ui->mqttApply->setEnabled(false);
    if (ui->srvLineEdit->text() != ""){
        emit on_srvWrite_clicked();
        _sleep(2000);
    }

    if (ui->portLineEdit->text() != ""){
        emit on_portWrite_clicked();
        _sleep(2000);
    }

    if (ui->topicLineEdit->text() != ""){
        emit on_topicWrite_clicked();
        _sleep(2000);
    }

    if (ui->subTopicLineEdit->text() != "") {
        nb_handle_write(ui->subTopicLineEdit, mqttSubTopicalAddress, mqttSubTopicalEntries);
        _sleep(2000);
    }

    emit on_idWrite_clicked();
    _sleep(2000);

    nb_handle_write(ui->mqttUserNameLineEdit, mqttUserAddress, mqttUserEntries);
    _sleep(2000);

    nb_handle_write(ui->mqttPasswordLineEdit, mqttPassWordAddress, mqttPassWordEntries);
    _sleep(2000);

    if (ui->intervalLineEdit->text() != ""){
        emit on_idWrite_2_clicked();
        _sleep(2000);
    }
    ui->mqttApply->setEnabled(true);
}

void MainWindow::on_mqttReload_clicked()
{
    ui->mqttReload->setEnabled(false);

    on_srvRead_clicked();
    _sleep(2000);
    on_portRead_clicked();
    _sleep(2000);
    on_topicRead_clicked();
    _sleep(2000);
    handle_read(mqttSubTopicalAddress, mqttSubTopicalEntries, &subTopicalReadReady);
    _sleep(2000);
    on_idRead_clicked();
    _sleep(2000);
    handle_read(mqttUserAddress, mqttUserEntries, &mqttUserNameReadReady);
    _sleep(2000);
    handle_read(mqttPassWordAddress, mqttPassWordEntries, &mqttPassWordReadReady);
    _sleep(2000);
    on_intervalRead_clicked();
    _sleep(2000);
    on_mqttStatusRead_clicked();

    ui->mqttReload->setEnabled(true);
}

void MainWindow::on_mqttTestPushButton_clicked()
{
    if (!modbusDevice)
        return;

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, MQTTTestAddress, MQTTTestEntries);
    writeUnit.setValue(0, 0);
    handle_write(writeUnit);
}

void MainWindow::on_mqttCheckPushButton_clicked()
{
    handle_read(MQTTStatusAddress, &mqttStatuReadReady);
}
