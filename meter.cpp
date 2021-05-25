#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QDebug>
#include <QMessageBox>

#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "meter.h"
#include "commanhelper.h"
#include "metermodelviewcontrol.h"

static QList<meterProfile> meterProfileList;
static int meterProfileListIndex;

void MainWindow::on_meterCreatePushButton_clicked()
{
    bool emptyOwn = false;
    int emptyNum = 0;
    for (emptyNum = 0; emptyNum < meterProfileList.count(); emptyNum++) {
        if (meterProfileList[emptyNum].tag.isEmpty()) {
            emptyOwn = true;
            break;
        }
    }

    if (meterProfileList.count() >= 15 && !emptyOwn)
        return;

    meterProfile mp;
    mp.tag = ui->meterAttributeNameLineEdit->text();
    mp.id = ui->meterDataIndexLineEdit->text().toInt();
    mp.magnitude = ui->meterMagnitudeLineEdit->text();

    if (emptyOwn) {
        meterProfileList[emptyNum].tag = mp.tag;
        meterProfileList[emptyNum].id = mp.id;
        meterProfileList[emptyNum].magnitude = mp.magnitude;
    }else {
        meterProfileList.append(mp);
    }

    m_meterViewControl->updateData(meterProfileList);

    for (int i = 0; i < meterProfileList.size(); i++) {
        if (meterProfileList[i].tag.isEmpty())
            ui->meterTableView->setRowHidden(i, true);
        else
            ui->meterTableView->setRowHidden(i, false);
    }
}

void MainWindow::on_meterErasePushButton_clicked()
{
    QModelIndex i = ui->meterTableView->currentIndex();
    if (i.isValid() == false)
        return;

    int index = i.row();
    int meterNumber = ui->meterNumberLineEdit->text().toInt();
    int times = (meterNumber - 1);

    meterProfileList[index].tag = "";
    meterProfileList[index].id = 0;
    meterProfileList[index].magnitude = "";

    QVector<quint16> valuesBody = meterHeadModbusUnit(meterProfileList[index].tag, 4, (int) meterProfileList[index].id, meterProfileList[index].magnitude, 4);
    int addr2 = meterTagBaseAddress + (times * meterGap);
    int entry2 = 9;
    QModbusDataUnit writeUnit2 = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, addr2, entry2);
    writeUnit2.setValues(valuesBody);

    handle_write(writeUnit2);

    m_meterViewControl->updateData(meterProfileList);
    for (int i = 0; i < meterProfileList.size(); i++) {
        if (meterProfileList[i].tag.isEmpty())
            ui->meterTableView->setRowHidden(i, true);
    }
}

QVector<quint16> MainWindow::meterHeadModbusUnit(QString str1, int entry1, quint16 number, QString str2, int entry2)
{
    QVector<quint16> values;
    int i = 0;
    for (i = 0; i < str1.size(); i++) {
        if ((i+1) % 2 == 0) {
            quint16 temp = str1.at(i - 1).toLatin1();
            temp = (temp << 8) + str1.at(i).toLatin1();
            values.push_back(temp);
        }
    }

    if (i % 2 && i) {
        quint16 temp = str1.at(i-1).toLatin1();
        temp = temp << 8;
        values.push_back(temp);
        i++;
    }

    for (i = (i / 2); i < entry1; i++) {
        values.push_back(0x0000);
    }

    // push back int
    values.push_back(number);

    // push back value2
    i = 0;
    for (i = 0; i < str2.size(); i++) {
        if ((i+1) % 2 == 0) {
            quint16 temp = str2.at(i - 1).toLatin1();
            temp = (temp << 8) + str2.at(i).toLatin1();
            values.push_back(temp);
        }
    }

    if (i % 2 && i) {
        quint16 temp = str2.at(i-1).toLatin1();
        temp = temp << 8;
        values.push_back(temp);
        i++;
    }

    for (i = (i / 2); i < entry2; i++)
    {
        values.push_back(0x0000);
    }

    return values;
}

void MainWindow::on_meterApplyPushButton_clicked()
{
    if (!meterProfileList.count())
        QMessageBox::information(NULL,  "INFO",  "Profile Empty, Please Add Meter Profile Firstly!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

    if (!modbusDevice)
        return;

    if (modbusDevice->state() != QModbusDevice::ConnectedState ) {
        statusBar()->showMessage(tr("Read error: Device not Connected.") , 1000);
        return;
    }

    int meterNumber = ui->meterNumberLineEdit->text().toInt();
    int times = (meterNumber - 1);

    QVector<quint16> values = meterHeadModbusUnit(ui->meterModelLineEdit->text(), 8, ui->meterAddressModelComboBox->currentIndex() + 1, ui->meterAddressLineEdit->text(), 8);
    int addr = meterModelBaseAddress + (times * meterGap);
    int entry = 17;
    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, addr, entry);
    writeUnit.setValues(values);
    handle_write(writeUnit);

    for (int i = 0; i < meterProfileList.count(); i++)
    {
        if (meterProfileList[i].tag.isEmpty())
            break;

        QVector<quint16> valuesBody = meterHeadModbusUnit(meterProfileList[i].tag, 4, (int) meterProfileList[i].id, meterProfileList[i].magnitude, 4);
        int addr2 = meterTagBaseAddress + (times * meterGap) + i * 9;
        int entry2 = 9;
        QModbusDataUnit writeUnit2 = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, addr2, entry2);
        writeUnit2.setValues(valuesBody);

        handle_write(writeUnit2);
        _sleep(2000);
    }
}

void MainWindow::meterHeadReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();

        QString s;
        for (uint i = 0; i < 8; i++) {
            if ((unit.value(i) >> 8) == 0x00)
                break;
            s[2*i] = unit.value(i) >> 8;
            if ((unit.value(i) & 0x00ff) == 0x00)
                break;
            s[(2*i) +1] = unit.value(i) & 0x00ff;
        }
        s.remove('\"');
        ui->meterModelLineEdit->setText(s);
        s.clear();

        ui->meterAddressModelComboBox->setCurrentIndex(unit.value(8) - 1);

        for (uint i = 9; i < 16; i++) {
            if ((unit.value(i) >> 8) == 0x00)
                break;
            s[2*i] = unit.value(i) >> 8;
            if ((unit.value(i) & 0x00ff) == 0x00)
                break;
            s[(2*i) +1] = unit.value(i) & 0x00ff;
        }
        s.remove('\"');
        s.remove(' ');
        ui->meterAddressLineEdit->setText(s);
        s.clear();

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

void MainWindow::meterBodyReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        meterProfile mp;
        const QModbusDataUnit unit = reply->result();

        QString s;
        for (uint i = 0; i < 4; i++) {
            if ((unit.value(i) >> 8) == 0x00)
                break;
            s[2*i] = unit.value(i) >> 8;
            if ((unit.value(i) & 0x00ff) == 0x00)
                break;
            s[(2*i) +1] = unit.value(i) & 0x00ff;
        }
        s.remove('\"');

        // meterProfileList[meterProfileListIndex].tag = s;
        mp.tag = s;
        s.clear();

        // meterProfileList[meterProfileListIndex].id = unit.value(4);
        mp.id = unit.value(4);

        for (uint i = 5; i < 9; i++) {
            if ((unit.value(i) >> 8) == 0x00)
                break;
            s[2*i] = unit.value(i) >> 8;
            if ((unit.value(i) & 0x00ff) == 0x00)
                break;
            s[(2*i) +1] = unit.value(i) & 0x00ff;
        }
        s.remove('\"');
        s.remove(' ');
        // meterProfileList[meterProfileListIndex].magnitude = s;
        mp.magnitude = s;
        s.clear();
        meterProfileList.append(mp);

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

void MainWindow::on_meterLoadPushButton_clicked()
{
    if (!modbusDevice)
        return;

    if (modbusDevice->state() != QModbusDevice::ConnectedState ) {
        statusBar()->showMessage(tr("Read error: Device not Connected.") , 1000);
        return;
    }

    int meterNumber = ui->meterNumberLineEdit->text().toInt();
    int times = (meterNumber - 1);

    handle_read(meterModelBaseAddress + (meterGap * times), 19, &meterHeadReadReady);
    _sleep(2000);

    meterProfileList.clear();
    for (meterProfileListIndex = 0; meterProfileListIndex < 15; meterProfileListIndex++)
    {
        handle_read(meterTagBaseAddress + (meterGap * times) + meterProfileListIndex * 9, 9, &meterBodyReadReady);
        _sleep(2000);
    }

    m_meterViewControl->updateData(meterProfileList);

    for (int i = 0; i < meterProfileList.size(); i++) {
        if (meterProfileList[i].tag.isEmpty() || meterProfileList[i].id > 20)
            ui->meterTableView->setRowHidden(i, true);
    }
}

void MainWindow::on_meterNextPushButton_clicked()
{
    int times = ui->meterNumberLineEdit->text().toInt();

    if (times == 15)
        times = 1;
    else
        times ++;

    ui->meterNumberLineEdit->setText(QString::number(times));

    on_meterLoadPushButton_clicked();
}
