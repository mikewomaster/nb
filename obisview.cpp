#include "dlms_model.h"
#include "dlms.h"
#include "mainwindow.h"

#include <QAbstractTableModel>
#include <QMessageBox>
#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>

static int times = 0;

void MainWindow::obis_view_model_init()
{
    m_pdlmsModel = new dlmsModel(this);

    ui->obisView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->obisView->setShowGrid(true);
    ui->obisView->setFrameShape(QFrame::Box);
    ui->obisView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->obisView->setModel(m_pdlmsModel);
    ui->obisView->setColumnWidth(0, 20);
    ui->obisView->setColumnWidth(1, 250);
    ui->obisView->setColumnWidth(2, 300);
    ui->obisView->setColumnWidth(3, 200);
    ui->obisView->setColumnWidth(4, 120);
    ui->obisView->setContextMenuPolicy(Qt::CustomContextMenu);

    popMenu = new QMenu(ui->obisView);
    QAction *actionUpdateInfo = new QAction();
    QAction *actionDelInfo = new QAction();
    actionUpdateInfo ->setText(QString("Edit"));
    actionDelInfo ->setText(QString("Delete"));
    popMenu->addAction(actionUpdateInfo);
    popMenu->addAction(actionDelInfo);
    connect(ui->obisView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotContextMent(QPoint)));
    connect(actionUpdateInfo, &QAction::triggered, this, &MainWindow::modelEdit);
    connect(actionDelInfo, &QAction::triggered, this, &MainWindow::obis_del);

    Dlms record;
    for (int i = 0; i < OBISTABLENUM; i++) {
        record.id =  i;
        record.obis_code = "";
        record.obis_value = "";
        record.status = "";
        record.ts = "";
        dlmsRecordList.append(record);
    }

    m_pdlmsModel->updateData(dlmsRecordList);

    for (int i = 0; i < OBISTABLENUM; i++) {
        if (dlmsRecordList[i].obis_code == "")
            ui->obisView->setRowHidden(i, true);
    }
    ui->obisView->setColumnHidden(ID_COLUMN, true);
}

void MainWindow::on_obisAdd_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    int term = 0;
    QStringList s = ui->obisCode->text().split(' ');
    QVector<quint16> values;
    QString obis_code_txt = "";

    for (int i = 0; i < s.size(); ++i) {
        if ((i+1) % 2 == 0){
            quint16 temp = s.at(i - 1).toInt(nullptr,16);
            temp = (temp << 8) + s.at(i).toInt(nullptr,16);
            values.push_back(temp);
        }

        if (i == 0) {
            obis_code_txt += QString::number(s.at(i).toInt(nullptr, 16));
        }
        else {
            obis_code_txt += ".";
            obis_code_txt += QString::number(s.at(i).toInt(nullptr, 16));
        }
    }

    for (int i = 0; i < OBISTABLENUM; i++) {
        if (dlmsRecordList[i].obis_code == "") {
            term = i;
            dlmsRecordList[i].obis_code = obis_code_txt;
            ui->obisView->setRowHidden(i, false);
            break;
        }
    }

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, OBISCODEADDR + term * OBISTABLEUNIT, OBISCODEENTRIES);
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

    m_pdlmsModel->updateData(dlmsRecordList);
}

void MainWindow::obis_del()
{
    if (!modbusDevice)
        return;

    int row = ui->obisView->currentIndex().row();
    QAbstractItemModel *model = ui->obisView->model();
    QModelIndex index = model->index(row, ID_COLUMN);
    QVariant data = model->data(index);
    int term = data.toInt();

    dlmsRecordList[term].obis_code = "";
    dlmsRecordList[term].obis_value = "";
    dlmsRecordList[term].status = "";
    dlmsRecordList[term].ts = "";

    QVector<quint16> values;
    for (int i = 0; i < OBISCODEENTRIES; ++i) {
        quint16 temp = 0x0000;
        values.push_back(temp);
    }

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, OBISCODEADDR + term * OBISTABLEUNIT, OBISCODEENTRIES);

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
    ui->obisView->setRowHidden(term, true);
}

void MainWindow::modelEdit()
{
    if (!modbusDevice)
        return;

    int row = ui->obisView->currentIndex().row();
    QAbstractItemModel *model = ui->obisView->model();
    QModelIndex index = model->index(row, ID_COLUMN);
    QVariant data = model->data(index);
    int term = data.toInt();

    m_obis_edit_dialog->id = term;
    m_obis_edit_dialog->show();
}

// MAGIC NUMBER: 10 times, each 2 structs, with 44 bytes, total 88 bytes
void MainWindow::obisUpdatReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    Dlms s;
    if (reply->error() == QModbusDevice::NoError) {
        (times == 9) ? (times = 0) : times++;
        m_pdlmsModel->clearDate();

        const QModbusDataUnit unit = reply->result();
        for (int i = 0; i < 2; i++) {
            if (unit.value(0 + i*44) == 0 && unit.value(1 + i*44) == 0 && unit.value(2 + i*44) == 0)
                continue;
            else {
                // obis code
                QString obis_code_txt = "";
                obis_code_txt += QString::number(unit.value(0 + i*44) >> 8);
                obis_code_txt += ".";
                obis_code_txt += QString::number(unit.value(0 + i*44) & 0x00ff);
                obis_code_txt += ".";
                obis_code_txt += QString::number(unit.value(1 + i*44) >> 8);
                obis_code_txt += ".";
                obis_code_txt += QString::number(unit.value(1 + i*44) & 0x00ff);
                obis_code_txt += ".";
                obis_code_txt += QString::number(unit.value(2 + i*44) >> 8);
                obis_code_txt += ".";
                obis_code_txt += QString::number(unit.value(2 + i*44) & 0x00ff);
                dlmsRecordList[2 * times + i].obis_code = obis_code_txt;

                // value, 3: offset, i*44: unit (2 times)
                QString s;
                for (uint j = 0; j < 16; j++) {
                    if ((unit.value(j + 3 + i*44) >> 8) == 0x00)
                        break;
                    s[2*j] = unit.value(j + 3 + i*44) >> 8;
                    if ((unit.value(j + 3 + i*44) & 0x00ff) == 0x00)
                        break;
                    s[(2*j) +1] = unit.value(j + 3 + i*44) & 0x00ff;
                }
                dlmsRecordList[2 * times + i].obis_value = s;

                // timestamp
                quint64 data;
                data = unit.value(19 + i*44);
                data = (data << 16) + unit.value(20 + i*44);

                QDateTime time = QDateTime::fromTime_t(data);
                QString StrCurrentTime = time.toString("yyyy-MM-dd hh:mm:ss ddd");
                dlmsRecordList[2*times + i].ts = StrCurrentTime;

                // status
                int status = unit.value(21 + i*44);
                if (status == 0)
                    dlmsRecordList[2*times + i].status = "init";
                else if (status == 1)
                    dlmsRecordList[2*times + i].status = "active";
                else if (status == 2)
                    dlmsRecordList[2*times + i].status = "inactive";
            }
         }

         m_pdlmsModel->updateData(dlmsRecordList);
         for (int i = 0; i < OBISTABLENUM; i++) {
             if (dlmsRecordList[i].obis_code == "")
                 ui->obisView->setRowHidden(i, true);
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

void MainWindow::on_obisReload_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    // MAGIC NUMBER: 10 times, each 2 structs with 44 units, total 88 bytes
    for (int j = 0; j < 10; j++) {
        quint16 ADDR = OBISCODEADDR + 44 * j;
        QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, 44);

        if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
            if (!reply->isFinished()) {
                connect(reply, &QModbusReply::finished, this, &MainWindow::obisUpdatReadReady);
            }
            else
                delete reply;
        } else {
            statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
        }

        // delay one second
        _sleep(1000);
    }

    for (int i = 0; i < 50; i++) {
         if (sensorRecordList[i].type != "")
            ui->sensorTableView->setRowHidden(i, false);
    }
}

// enable mouse-right-button click
void MainWindow::slotContextMent(QPoint pos)
{
    QModelIndex index = ui->obisView->indexAt(pos);
    if (index.isValid()) {
        popMenu->exec(QCursor::pos());
    }
}
