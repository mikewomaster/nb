#include "sensor.h"
#include "mqtt.h"
#include "mainwindow.h"
#include <QAbstractTableModel>
#include <QMessageBox>
#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>

static int times = 1;

void MainWindow::sensorAddModbus(sen unit, int term)
{
    if (!modbusDevice)
        return;

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, RTUSENSORADDR + term*RTUSENSORNUM, RTUSENSORNUM);
    QVector<quint16> values;

    int i = 0;
    for (i = 0; i < unit.type.size(); i++) {
        if ((i+1) % 2 == 0) {
            quint16 temp = unit.type.at(i - 1).toLatin1();
            temp = (temp << 8) + unit.type.at(i).toLatin1();
            values.push_back(temp);
        }
    }

    if (i % 2) {
        quint16 temp = unit.type.at(i-1).toLatin1();
        temp = temp << 8;
        values.push_back(temp);
        i++;
    }

    for (i = (i / 2); i < 4; i++) {
        values.push_back(0x0000);
    }

    values.push_back(unit.id);
    values.push_back(unit.reg_addr);
    values.push_back(unit.len);
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

void MainWindow::on_sensorAddPushButton_clicked()
{
    statusBar()->clearMessage();

    sen unit;
    unit.type = ui->sensorTypeLineEdit->text();
    if (unit.type.length() > 8) {
        QMessageBox::information(NULL, "Error", "Please set RTU name with maximum 8 characters.");
        return;
    }

    if (unit.type.length() == 0) {
        QMessageBox::information(NULL, "Error", "Please set RTU number.");
        return;
    }

    unit.id = ui->sensorSlaveId->text().toShort();
    if (unit.id < 0 || unit.id >255) {
        QMessageBox::information(NULL, "Error", "Please set ID number between 0 and 255.");
        return;
    }

    /*
        unit.len = ui->sensorLength->text().toInt();
        if (unit.len < 1 || unit.len > 11) {
            QMessageBox::information(NULL, "Error", "Please set Length number between 1 and 10.");
            return;
        }
    */
    unit.len = 0;

    unit.reg_addr = ui->sensorPLCAddress->text().toInt();
    if (unit.reg_addr < 1 || unit.reg_addr > 10001) {
        QMessageBox::information(NULL, "Error", "Please set Register Address between 1 and 10001.");
        return;
    }

    int i = 0;
    for (i = 0; i < 20; i++) {
        if (sensorRecordList[i].type == "") {
            int term = i;
            sensorAddModbus(unit, term);
            unit.seq = i;
            sensorRecordList[i] = unit;
            break;
        }
    }
    if (i == 20)
        QMessageBox::information(nullptr, "Warn", "Maximum 20 Modbus RTU Supported");

    m_sensorModel->updateData(sensorRecordList);
    ui->sensorTableView->setRowHidden(i, false);
}

void MainWindow::on_sensorClearPushButton_clicked()
{
    for (int i =0; i < 20; i++) {
        sensorRecordList[i].type = "";
        sensorRecordList[i].type_ = 0;
        sensorRecordList[i].id = 0;
        sensorRecordList[i].reg_addr = 0;
        sensorRecordList[i].len = 0;
        ui->sensorTableView->setRowHidden(i, true);
    }
}

// MAGIC NUMBER: 2 times, each 25 structs, 3 values (number, id, reg_addr), total 75 bytes
void MainWindow::sensorUpdateReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        m_sensorModel->clearDate(sensorRecordList);
        const QModbusDataUnit unit = reply->result();

        for (int i = 0; i < 10; i++) {
            if (times == 1) {
                QString s = "";
                int stringNumber = 0;
                for (uint j = 7*i + 0; j < 7*i + 4; j++) {
                    if ((unit.value(j) >> 8) == 0x00)
                        break;
                    s[2*stringNumber] = unit.value(j) >> 8;
                    if ((unit.value(j) & 0x00ff) == 0x00)
                        break;
                    s[(2*stringNumber) +1] = unit.value(j) & 0x00ff;
                    stringNumber ++;
                }
                sensorRecordList[i].type = s;
                sensorRecordList[i].id = unit.value(7*i + 4);
                sensorRecordList[i].reg_addr = unit.value(7*i + 5);
                sensorRecordList[i].len = unit.value(7*i + 6);
            } else {
                QString s = "";
                int stringNumber = 0;
                for (uint j = 7*i + 0; j < 7*i + 4; j++) {
                    if ((unit.value(j) >> 8) == 0x00)
                        break;
                    s[2*stringNumber] = unit.value(j) >> 8;
                    if ((unit.value(j) & 0x00ff) == 0x00)
                        break;
                    s[(2*stringNumber) +1] = unit.value(j) & 0x00ff;
                    stringNumber ++;
                }

                sensorRecordList[10+i].type = s;
                sensorRecordList[10+i].id = unit.value(7*i + 4);
                sensorRecordList[10+i].reg_addr = unit.value(7*i + 5);
                sensorRecordList[10+i].len = unit.value(7*i + 6);
            }
        }

        times == 1 ? times = 2 : times = 1;
        sensorFlag = true;
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

void MainWindow::on_sensorUpdatePushButton_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();
    on_sensorClearPushButton_clicked();
    _sleep(1000);

    ui->sensorUpdatePushButton->setEnabled(false);
    // MAGIC NUMBER: 2 times, each 10 structs, with 7 units, total 70 units
    for (int j = 0; j < 2; j++) {
        quint16 ADDR = RTUSENSORADDR + 70 * j;
        QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, 70);

        if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
            if (!reply->isFinished()) {
                connect(reply, &QModbusReply::finished, this, &MainWindow::sensorUpdateReadReady);
            }
            else
                delete reply;
        } else {
            statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
        }
        _sleep(1000);
    }
    _sleep(3000);
    QMessageBox::information(nullptr, "Read Value", "Read sensor-Configuration done, thanks for patience.");
    m_sensorModel->updateData(sensorRecordList);

    for (int i = 0; i < 20; i++) {
         if (sensorRecordList[i].type != "")
            ui->sensorTableView->setRowHidden(i, false);
    }
    ui->sensorUpdatePushButton->setEnabled(true);
}

void MainWindow::sensorCheckReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    sen s;
    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        for (int i = 0; i < 50; i++) {
            sensorRecordList[i].value = QString::number(unit.value(i));
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

void MainWindow::on_sensorCheckPushButton_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    if (!sensorFlag) {
        QMessageBox::information(NULL, "Information", "Please Update Sensor Configure Firstly");
        return;
    }

    quint16 ADDR = RTUSENSORVALUE;
    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, RTUVALUEENTRIES);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, &MainWindow::sensorCheckReadReady);
        }
        else
            delete reply;
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }

    _sleep(100);
    QMessageBox::information(nullptr, "Read Value", "Read sensor-data done, thanks for patience.");
    m_sensorModel->updateData(sensorRecordList);
    for (int i = 0; i < 50; i++) {
         if (sensorRecordList[i].type != "")
            ui->sensorTableView->setRowHidden(i, false);
    }
}

void MainWindow::sensMensu(QPoint pos)
{
    QModelIndex index = ui->sensorTableView->indexAt(pos);
    if (index.isValid()) {
        senpopMenu->exec(QCursor::pos());
    }
}

void MainWindow::sensEdit()
{
    int row = ui->sensorTableView->currentIndex().row();
    QAbstractItemModel *model = ui->sensorTableView->model();
    QModelIndex index = model->index(row, 4);
    QVariant data = model->data(index);
    int term = data.toInt();
    if (!modbusDevice)
        return;

    m_sensor_dialog = new sensor_edit(this);

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, RTUSENSORADDR + term*RTUSENSORNUM, RTUSENSORNUM);
    m_sensor_dialog->seq = term;
    m_sensor_dialog->updateDateFromMainWindow();
    m_sensor_dialog->show();
    m_sensor_dialog->setWindowTitle("Sensor Setting");

    while(!sensor_edit_flag){
        _sleep(500);
    }

    QVector<quint16> values;

    int i = 0;
    for (i = 0; i < sensorRecordList[term].type.size(); i++) {
        if ((i+1) % 2 == 0) {
            quint16 temp = sensorRecordList[term].type.at(i - 1).toLatin1();
            temp = (temp << 8) + sensorRecordList[term].type.at(i).toLatin1();
            values.push_back(temp);
        }
    }

    if (i % 2) {
        quint16 temp = sensorRecordList[term].type.at(i-1).toLatin1();
        temp = temp << 8;
        values.push_back(temp);
        i++;
    }

    for (i = (i / 2); i < 4; i++) {
        values.push_back(0x0000);
    }

    values.push_back(sensorRecordList[term].id);
    values.push_back(sensorRecordList[term].reg_addr);
    values.push_back(sensorRecordList[term].len);

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
    m_sensorModel->updateData(sensorRecordList);
    sensor_edit_flag = false;

    delete(m_sensor_dialog);
}

void MainWindow::sensDelete()
{
    int row = ui->sensorTableView->currentIndex().row();
    QAbstractItemModel *model = ui->sensorTableView->model();
    QModelIndex index = model->index(row, 4);
    QVariant data = model->data(index);
    int term = data.toInt();

    sensorRecordList[term].type = "";
    sensorRecordList[term].type_ = 0;
    sensorRecordList[term].id = 0;
    sensorRecordList[term].reg_addr = 0;
    sensorRecordList[term].len = 0;

    if (!modbusDevice)
        return;

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, RTUSENSORADDR + term*RTUSENSORNUM, RTUSENSORNUM);

    for (int i = 0; i < 7; i++)
        writeUnit.setValue(i, 0);

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
    ui->sensorTableView->setRowHidden(term, true);
}

void MainWindow::meterViewModelInit()
{
    m_meterViewControl = new meterModelViewControl(this);

    ui->meterTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->meterTableView->setShowGrid(true);
    ui->meterTableView->setFrameShape(QFrame::Box);
    ui->meterTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->meterTableView->setModel(m_meterViewControl);
    ui->meterTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->meterTableView->setColumnWidth(0, 160);
    ui->meterTableView->setColumnWidth(1, 130);
    ui->meterTableView->setColumnWidth(2, 130);
}

void MainWindow::sensor_view_model()
{
    m_sensorModel = new sensor(this);
    ui->sensorTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->sensorTableView->setShowGrid(true);
    ui->sensorTableView->setFrameShape(QFrame::Box);
    ui->sensorTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->sensorTableView->setModel(m_sensorModel);
    ui->sensorTableView->setColumnWidth(0, 120);
    ui->sensorTableView->setColumnWidth(1, 140);
    ui->sensorTableView->setColumnWidth(2, 120);
    ui->sensorTableView->setColumnWidth(3, 110);
    ui->sensorTableView->setColumnWidth(4, 70);
    ui->sensorTableView->setContextMenuPolicy(Qt::CustomContextMenu);

    senpopMenu = new QMenu(ui->obisView);
    QAction *actionUpdateSensInfo = new QAction();
    QAction *actionDelSensInfo = new QAction();
    actionUpdateSensInfo ->setText(QString("Edit"));
    actionDelSensInfo ->setText(QString("Delete"));
    senpopMenu->addAction(actionUpdateSensInfo);
    senpopMenu->addAction(actionDelSensInfo);
    connect(ui->sensorTableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(sensMensu(QPoint)));
    connect(actionUpdateSensInfo, &QAction::triggered, this, &MainWindow::sensEdit);
    connect(actionDelSensInfo, &QAction::triggered, this, &MainWindow::sensDelete);

    sensorRecordList.clear();
    sen record;
    for (int i = 0; i < 20; i++) {
        record.id = 0;
        record.reg_addr = 0;
        record.type_ = 0;
        record.len = 0;
        record.seq = i;
        sensorRecordList.append(record);
    }
    m_sensorModel->updateData(sensorRecordList);
    for (int i = 0; i < 50; i++) {
        if (!record.type_)
            ui->sensorTableView->setRowHidden(i, true);
    }
    ui->sensorTableView->setColumnHidden(4, true);
}

void MainWindow::on_sensorIntervalSetPushButton_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, RTUSENSORINTERVAL, 1);
    uint value = ui->sensorIntervalLineEdit->text().toInt();

    writeUnit.setValue(0, value);
    writeSingleHoldingRegister(writeUnit);
}

void MainWindow::sensorIntervalReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        const QString entry = QString::number(unit.value(0));
        ui->sensorIntervalLineEdit->setText(entry);
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

void MainWindow::on_sensorIntervalCheckPushButton_clicked()
{
    if (!modbusDevice)
        return;
    ui->sensorIntervalLineEdit->clear();
    statusBar()->clearMessage();

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, RTUSENSORINTERVAL, 1);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::sensorIntervalReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::setCheckBoxLow(quint16 value)
{
   if ((value&0x0001) != 0)
       ui->checkBox_24->setChecked(true);
   if ((value&0x0002) != 0)
       ui->checkBox_1->setChecked(true);
   if ((value&0x0004) != 0)
       ui->checkBox_2->setChecked(true);
   if ((value&0x0008) != 0)
       ui->checkBox_3->setChecked(true);

   if ((value&0x0010) != 0)
       ui->checkBox_4->setChecked(true);
   if ((value&0x0020) != 0)
       ui->checkBox_5->setChecked(true);
   if ((value&0x0040) != 0)
       ui->checkBox_6->setChecked(true);
   if ((value&0x0080) != 0)
       ui->checkBox_7->setChecked(true);

   if ((value&0x0100) != 0)
       ui->checkBox_8->setChecked(true);
   if ((value&0x0200) != 0)
       ui->checkBox_9->setChecked(true);
   if ((value&0x0400) != 0)
       ui->checkBox_10->setChecked(true);
   if ((value&0x0800) != 0)
       ui->checkBox_11->setChecked(true);

   if ((value&0x1000) != 0)
       ui->checkBox_12->setChecked(true);
   if ((value&0x2000) != 0)
       ui->checkBox_13->setChecked(true);
   if ((value&0x4000) != 0)
       ui->checkBox_14->setChecked(true);
   if ((value&0x8000) != 0)
       ui->checkBox_15->setChecked(true);
}

void MainWindow::setCheckBoxHigh(quint16 value)
{
    if ((value&0x0001) != 0)
        ui->checkBox_16->setChecked(true);
    if ((value&0x0002) != 0)
        ui->checkBox_17->setChecked(true);
    if ((value&0x0004) != 0)
        ui->checkBox_18->setChecked(true);
    if ((value&0x0008) != 0)
        ui->checkBox_19->setChecked(true);

    if ((value&0x0010) != 0)
        ui->checkBox_20->setChecked(true);
    if ((value&0x0020) != 0)
        ui->checkBox_21->setChecked(true);
    if ((value&0x0040) != 0)
        ui->checkBox_22->setChecked(true);
    if ((value&0x0080) != 0)
        ui->checkBox_23->setChecked(true);
}

void MainWindow::sensorTimeReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;
    quint16 temp = 0;
    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        temp = unit.value(1);
        setCheckBoxLow(temp);
        temp = unit.value(0);
        setCheckBoxHigh(temp);
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

void MainWindow::checkBoxClear()
{
    ui->checkBox_1->setChecked(false);
    ui->checkBox_2->setChecked(false);
    ui->checkBox_3->setChecked(false);
    ui->checkBox_4->setChecked(false);
    ui->checkBox_5->setChecked(false);
    ui->checkBox_6->setChecked(false);
    ui->checkBox_7->setChecked(false);
    ui->checkBox_8->setChecked(false);
    ui->checkBox_9->setChecked(false);
    ui->checkBox_10->setChecked(false);
    ui->checkBox_11->setChecked(false);
    ui->checkBox_12->setChecked(false);
    ui->checkBox_13->setChecked(false);
    ui->checkBox_14->setChecked(false);
    ui->checkBox_15->setChecked(false);
    ui->checkBox_16->setChecked(false);
    ui->checkBox_17->setChecked(false);
    ui->checkBox_18->setChecked(false);
    ui->checkBox_19->setChecked(false);
    ui->checkBox_20->setChecked(false);
    ui->checkBox_21->setChecked(false);
    ui->checkBox_22->setChecked(false);
    ui->checkBox_23->setChecked(false);
    ui->checkBox_24->setChecked(false);
}

void MainWindow::on_sensorTimeCheckPushButton_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();
    checkBoxClear();

    quint16 ADDR = RTUSENSORCLOCK;
    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, 2);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::sensorTimeReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
    _sleep(20);
    chgrdbtn();
}

void MainWindow::setClock(quint32 *clock)
{
    if (ui->checkBox_24->isChecked())
        *clock += (1 << 0);

    if (ui->checkBox_1->isChecked())
        *clock += (1 << 1);

    if (ui->checkBox_2->isChecked())
        *clock += (1 << 2);

    if (ui->checkBox_3->isChecked())
        *clock += (1 << 3);

    if (ui->checkBox_4->isChecked())
        *clock += (1 << 4);

    if (ui->checkBox_5->isChecked())
        *clock += (1 << 5);

    if (ui->checkBox_6->isChecked())
        *clock += (1 << 6);

    if (ui->checkBox_7->isChecked())
        *clock += (1 << 7);

    if (ui->checkBox_8->isChecked())
        *clock += (1 << 8);

    if (ui->checkBox_9->isChecked())
        *clock += (1 << 9);

    if (ui->checkBox_10->isChecked())
        *clock += (1 << 10);

    if (ui->checkBox_11->isChecked())
        *clock += (1 << 11);

    if (ui->checkBox_12->isChecked())
        *clock += (1 << 12);

    if (ui->checkBox_13->isChecked())
        *clock += (1 << 13);

    if (ui->checkBox_14->isChecked())
        *clock += (1 << 14);

    if (ui->checkBox_15->isChecked())
        *clock += (1 << 15);

    if (ui->checkBox_16->isChecked())
        *clock += (1 << 16);

    if (ui->checkBox_17->isChecked())
        *clock += (1 << 17);

    if (ui->checkBox_18->isChecked())
        *clock += (1 << 18);

    if (ui->checkBox_19->isChecked())
        *clock += (1 << 19);

    if (ui->checkBox_20->isChecked())
        *clock += (1 << 20);

    if (ui->checkBox_21->isChecked())
        *clock += (1 << 21);

    if (ui->checkBox_22->isChecked())
        *clock += (1 << 22);

    if (ui->checkBox_23->isChecked())
        *clock += (1 << 23);
}

void MainWindow::on_sensorTimeSetPushButton_clicked()
{
    quint32 clock = 0;
    setClock(&clock);
    QVector<quint16> values;
    values.push_back((clock >> 16));
    values.push_back((clock & 0xffff));

    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, RTUSENSORCLOCK, 2);
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

void MainWindow::sensorTimeStampReadReady()
{
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
        ui->sensorTSLineEdit->setText(StrCurrentTime);
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

void MainWindow::on_sensorTSChkButton_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    quint16 ADDR = SENSORTS;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, 2);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::sensorTimeStampReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::on_sensorTSSetBtn_clicked()
{
    QDateTime time = QDateTime::currentDateTime();
    int timeT = time.toTime_t();
    QString StrCurrentTime = time.toString("yyyy-MM-dd hh:mm:ss ddd");
    ui->sensorTSLineEdit->setText(StrCurrentTime);

    quint32 timeoutStamp = timeT;
    QVector<quint16> values;

    for (int i = 1; i >= 0; i--) {
        quint16 temp = 0;
        temp = (timeoutStamp >> (i*2*8)) & 0x0000ffff;
        values.push_back(temp);
    }

    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, SENSORTS, 2);
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

void MainWindow::sensorCountDownReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        quint32 data;
        data = unit.value(0);
        ui->sensorCountDownLineEdit->setText(QString::number(data));
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

void MainWindow::on_sensorCountdownCheckButton_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    quint16 ADDR = SENSORCOUNTDOWNTIME;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, 1);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::sensorCountDownReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
    _sleep(20);
    chgrdbtn();
}

void MainWindow::on_sensorIntervalSetPushButton_2_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, SENSORCOUNTDOWNTIME, 1);
    quint16 currentOutputValue =  ui->sensorCountDownLineEdit->text().toInt();

    writeUnit.setValue(0, currentOutputValue);
    writeSingleHoldingRegister(writeUnit);
}

void MainWindow::on_sensorCTRadioButton_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, SENSORTRANSFORMANT, 1);
    quint16 currentOutputValue =  0;

    writeUnit.setValue(0, currentOutputValue);
    writeSingleHoldingRegister(writeUnit);
}

void MainWindow::on_sensorSTradioButton_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, SENSORTRANSFORMANT, 1);
    quint16 currentOutputValue =  1;

    writeUnit.setValue(0, currentOutputValue);
    writeSingleHoldingRegister(writeUnit);
}

void MainWindow::sensorFormatReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        ui->sensorCTRadioButton->setChecked(false);
        ui->sensorSTradioButton->setChecked(false);
        const QModbusDataUnit unit = reply->result();
        quint32 data;
        data = unit.value(0);
        if (data == 0){
            ui->sensorCTRadioButton->setChecked(true);
        }else{
            ui->sensorSTradioButton->setChecked(true);
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

void MainWindow::chgrdbtn()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    quint16 ADDR = SENSORTRANSFORMANT;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, 1);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::sensorFormatReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::rtuTimeoutReadReady()
{
    handle_read_ready(ui->rtuTimeOutLineEdit);
}

void MainWindow::on_rtuTimeOutRead_clicked()
{
    handle_read(mqttRtuTimeOutAddress, &rtuTimeoutReadReady);
}

void MainWindow::on_rtuTimeOutWrite_clicked()
{
    handle_write(ui->rtuTimeOutLineEdit, mqttRtuTimeOutAddress);
}
