#include "obis_edit.h"
#include "ui_obis_edit.h"
#include "mainwindow.h"
#include "dlms.h"

#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>

obis_edit::obis_edit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::obis_edit)
{
    ui->setupUi(this);
    id = 0;
}

obis_edit::~obis_edit()
{
    delete ui;
}

void obis_edit::on_obisAdd_clicked()
{
    MainWindow *w = (MainWindow*) parentWidget();

    if (!w->modbusDevice)
        return;
    w->statusBar()->clearMessage();

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

    w->dlmsRecordList[id].obis_code = obis_code_txt;
    w->dlmsRecordList[id].obis_value = "";
    w->dlmsRecordList[id].ts = "";
    w->dlmsRecordList[id].status = "";

    w->ui->obisView->setRowHidden(id, false);
    w->m_pdlmsModel->updateData(w->dlmsRecordList);

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, OBISCODEADDR + id * OBISTABLEUNIT, OBISCODEENTRIES);
    writeUnit.setValues(values);

    if (auto *reply = w->modbusDevice->sendWriteRequest(writeUnit, w->ui->serverEdit->value())) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                MainWindow *w = (MainWindow*) parentWidget();
                if (reply->error() == QModbusDevice::ProtocolError) {
                    w->statusBar()->showMessage(tr("Write response error: %1 (Mobus exception: 0x%2)")
                        .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16),
                        5000);
                } else if (reply->error() != QModbusDevice::NoError) {
                    w->statusBar()->showMessage(tr("Write response error: %1 (code: 0x%2)").
                        arg(reply->errorString()).arg(reply->error(), -1, 16), 5000);
                }
                w->statusBar()->showMessage(tr("OK!"));
                reply->deleteLater();
            });
        } else {
            // broadcast replies return immediately
            reply->deleteLater();
        }
    } else {
        w->statusBar()->showMessage(tr("Write error: ") + w->modbusDevice->errorString(), 5000);
    }

    close();
}
