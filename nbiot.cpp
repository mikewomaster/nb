#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QDebug>
#include <QMessageBox>

#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "nbiot.h"
#include "mqtt.h"

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

    if (addr != mqttTopicAddress) {
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

void MainWindow::apnReadReady()
{
    nb_handle_read_ready(ui->apnLineEdit);
}

void MainWindow::on_apnRead_clicked()
{
    handle_read(APNAddress, APNEntries, &apnReadReady);
}

void MainWindow::on_apnWrite_clicked()
{
   nb_handle_write(ui->apnLineEdit, APNAddress, APNEntries);
}

// user
void MainWindow::on_userWrite_clicked()
{
    nb_handle_write(ui->userLineEdit, UserAddress, UserEntries);
}

void MainWindow::userReadReady()
{
    nb_handle_read_ready(ui->userLineEdit);
}

void MainWindow::on_userRead_clicked()
{
    handle_read(UserAddress, UserEntries, &userReadReady);
}

// password
void MainWindow::on_passwordWrite_clicked()
{
    nb_handle_write(ui->passwordLineEdit, NBPwdAddress, NBPwdEntries);
}

void MainWindow::passwordReadReady()
{
    nb_handle_read_ready(ui->passwordLineEdit);
}

void MainWindow::on_passwordRead_clicked()
{
    handle_read(NBPwdAddress, NBPwdEntries, &passwordReadReady);
}

// status
void MainWindow::on_nbStatusWrite_clicked()
{
    handle_write(ui->nbStatusLineEdit, NBStatusAddress);
}

void MainWindow::statusReadReady()
{
    handle_read_ready(ui->nbStatusLineEdit);
}

void MainWindow::on_nbStatusRead_clicked()
{
    handle_read(NBStatusAddress, &statusReadReady);
}

// ip
void MainWindow::on_ipWrite_clicked()
{
    nb_handle_write(ui->ipLineEdit, NBIPAddress, NBIPEntries);
}

void MainWindow::ipReadReady()
{
    nb_handle_read_ready(ui->ipLineEdit);
}

void MainWindow::on_ipRead_clicked()
{
    handle_read(NBIPAddress, NBIPEntries, &ipReadReady);
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

void MainWindow::nbModelReadReady()
{
    handle_read_ready(ui->nbmodelCombox);
}

void MainWindow::on_nbModelRead_clicked()
{
    handle_read(NBIoTModelAddress, &nbModelReadReady);
}

void MainWindow::handle_write(QComboBox *cb, quint16 add)
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, add, NBIoTEntries);
    quint16 currentOutputValue =  cb->currentIndex();

    writeUnit.setValue(0, currentOutputValue);
    writeSingleHoldingRegister(writeUnit);
}

void MainWindow::on_nbModeWrite_clicked()
{
    handle_write(ui->nbmodelCombox, NBIoTModelAddress);
}

void MainWindow::on_nbApply_clicked()
{
    // emit(m_ymodem->finRcv("MeterLog"));
    // return;

    ui->nbApply->setEnabled(false);

    emit on_nbModeWrite_clicked();
        _sleep(2000);

    // if (ui->apnLineEdit->text() != "") {
        emit on_apnWrite_clicked();
        _sleep(2000);
    //}

    //if (ui->userLineEdit->text() != "") {
        emit on_userWrite_clicked();
        _sleep(2000);
    //}

    // if (ui->passwordLineEdit->text() != "") {
        emit on_passwordWrite_clicked();
        _sleep(2000);
    //}

    if (ui->ipLineEdit->text() != "") {
        emit on_ipWrite_clicked();
        _sleep(2000);
    }

    emit on_plmnWrite_clicked();
    _sleep(2000);

    ui->nbApply->setEnabled(true);
}

void MainWindow::on_nbReload_clicked()
{
    ui->nbReload->setEnabled(false);
    emit on_nbModelRead_clicked();
    _sleep(2000);
    emit on_apnRead_clicked();
    _sleep(2000);
    emit on_userRead_clicked();
    _sleep(2000);
    emit on_passwordRead_clicked();
    _sleep(2000);
    emit on_ipRead_clicked();
    _sleep(2000);
    emit on_nbStatusRead_clicked();
    _sleep(2000);
    emit on_plmnRead_clicked();
    _sleep(2000);

    if (!ui->portEdit_3->text().contains("485")) {
        nbEnableRead(); 
    }
    ui->nbReload->setEnabled(true);
}

void MainWindow::nbEnableReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        int result = unit.value(0);
        result == 1 ? ui->nbEnableRadioButton->setChecked(true) : ui->nbEnableRadioButton->setChecked(false);
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

void MainWindow::nbEnableRead()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, NBEnableAddress, NBEnableEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, nbEnableReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::on_nbEnableRadioButton_clicked()
{
    if (!modbusDevice)
        return;

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, NBEnableAddress, NBEnableEntries);

    if (ui->nbEnableRadioButton->isChecked())
        writeUnit.setValue(0, 1);
    else
        writeUnit.setValue(0, 0);

    handle_write(writeUnit);
}

void MainWindow::on_plmnWrite_clicked()
{
     nb_handle_write(ui->plmnLineEdit, NBPLMNAddress, NBPLMNEntries);
}

void MainWindow::plmnReadReady()
{
    nb_handle_read_ready(ui->plmnLineEdit);
}

void MainWindow::on_plmnRead_clicked()
{
    handle_read(NBPLMNAddress, NBPLMNEntries, &plmnReadReady);
}

void MainWindow::on_cellularEnableButton_clicked()
{
    if (ui->nbEnableRadioButton->isChecked()) {
        ui->nbEnableRadioButton->setChecked(false);
        ui->cellularEnableButton->setText("Connect");
    }
    else {
        ui->nbEnableRadioButton->setChecked(true);
        ui->cellularEnableButton->setText("Disconnect");
    }

    on_nbEnableRadioButton_clicked();
}
