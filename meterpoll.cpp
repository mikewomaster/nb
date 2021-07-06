#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QDebug>
#include <QMessageBox>

#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "meter.h"
#include "commanhelper.h"
#include "meterpoll.h"

void MainWindow::meterPollModelReadReady()
{
    handle_read_ready(m_meterPollModel, 99);
}

void MainWindow::meterPollSNHeadReadReady()
{
    handle_read_ready(m_meterPollModel, 0);
}

void MainWindow::meterPollAddHeadReadReady()
{
    handle_read_ready(m_meterPollModel, 1);
}

void MainWindow::meterPollPriHeadReadReady()
{
    handle_read_ready(m_meterPollModel, 2);
}

void MainWindow::meterPollSecHeadReadReady()
{
    handle_read_ready(m_meterPollModel, 3);
}

void MainWindow::meterPollStatusHeadReadReady()
{
    handle_read_ready(m_meterPollModel, 4);
}

void MainWindow::meterPollManuHeadReadReady()
{
    handle_read_ready(m_meterPollModel, 5);
}

void MainWindow::meterPollTypeHeadReadReady()
{
    handle_read_ready(m_meterPollModel, 6);
}

void MainWindow::meterPollVerHeadReadReady()
{
    handle_read_ready(m_meterPollModel, 7);
}

void MainWindow::meterPollBaudHeadReadReady()
{
    handle_read_ready(m_meterPollModel, 8);
}

static int meterPollIndex;
static QList<meterPoll> meterPollList;
static bool meterPollFlag = true;

void MainWindow::meterPollReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        if (unit.valueCount() == 1) {
            meterPoll mpTemp;
            mpTemp.attribute = "AlmCode";
            mpTemp.value = QString::number(unit.value(0));
            mpTemp.magnitude = "Error Codes";
            meterPollList.append(mpTemp);
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
            if (s.isEmpty())
            {
                meterPollFlag = false;
                return;
            }

            QStringList strList = s.split('#');

            meterPoll mpTemp;
            mpTemp.attribute = strList[0];
            mpTemp.value = strList[1];
            mpTemp.magnitude = strList[2];
            meterPollList.append(mpTemp);
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

void MainWindow::on_meterPollPushButton_clicked()
{
    /*
    if (!modbusDevice)
        return;

    if (modbusDevice->state() != QModbusDevice::ConnectedState ) {
        statusBar()->showMessage(tr("Read error: Device not Connected.") ,1000);
        return;
    }
    */

    QModbusDataUnit meterPollWriteUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, meterPollStart, meterPollOne);
    meterPollWriteUnit.setValue(0, 1);
    handle_write(meterPollWriteUnit);
    QMessageBox::information(NULL,  "INFO",  "The meter starts reading data, wait a while please.", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    _sleep(5000);
    ui->meterPollNextPushButton->setEnabled(false);

    int meterNumber = ui->meterPollNumLineEdit->text().toInt();
    int times = (meterNumber - 1);

    handle_read(meterModelBaseAddress + (meterPollGap * times), meterEight, &meterPollModelReadReady);
    _sleep(1000);
    handle_read(meterPollSN + (meterPollGap * times), meterEight, &meterPollSNHeadReadReady);
    _sleep(1000);
    handle_read(meterPollAdressMode + (meterPollGap * times), &meterPollAddHeadReadReady);
    _sleep(1000);
    handle_read(meterPollPriAddress + (meterPollGap * times), &meterPollPriHeadReadReady);
    _sleep(1000);
    handle_read(meterPollSecAddress + (meterPollGap * times), meterEight, &meterPollSecHeadReadReady);
    _sleep(1000);
    handle_read(meterPollStatus + (meterPollGap * times), &meterPollStatusHeadReadReady);
    _sleep(1000);
    handle_read(meterPollManu + (meterPollGap * times), meterEight, &meterPollManuHeadReadReady);
    _sleep(1000);
    handle_read(meterPollType + (meterPollGap * times), &meterPollTypeHeadReadReady);
    _sleep(1000);
    handle_read(meterPollVersion + (meterPollGap * times), &meterPollVerHeadReadReady);
    _sleep(1000);
    handle_read(meterPollBaudRate + (meterPollGap * times), meterTwo, &meterPollBaudHeadReadReady);
    _sleep(1000);

    meterPollList.clear();

    // handle_read(meterPollAlarmCode + (meterPollGap * times), &meterPollReadReady);
    // _sleep(2000);

    QProgressDialog pro;
    pro.setLabelText(tr("Processing... Please wait..."));
    pro.setRange(0, 100);
    pro.setModal(true);
    pro.setCancelButtonText(tr("Cancel"));
    pro.setValue(0);
    for (meterPollIndex = 0; meterPollIndex < 10; meterPollIndex++)
    {
        if (!meterPollFlag)
        {
            meterPollFlag = true;
            break;
        }

        handle_read(meterPollChannel + (meterPollGap * times) + meterTWELVE * meterPollIndex, meterTWELVE, &meterPollReadReady);
        _sleep(1000);
        pro.setValue(meterPollIndex*10);
    }
    pro.setValue(100);

    m_meterPollModelBody->updateData(meterPollList);
    for (int i = 0; i < meterPollList.count(); i++){
        if (meterPollList[i].attribute.isEmpty()){
            ui->meterPollBodyTableView->setRowHidden(i, true);
        }
    }

    ui->meterPollNextPushButton->setEnabled(true);
}

void MainWindow::on_meterPollNextPushButton_clicked()
{
    int Numtimes = ui->meterPollNumLineEdit->text().toInt();

    if (Numtimes == 10)
        Numtimes = 1;
    else
        Numtimes ++;

    ui->meterPollNumLineEdit->setText(QString::number(Numtimes));

    if (!modbusDevice)
        return;

    if (modbusDevice->state() != QModbusDevice::ConnectedState ) {
        statusBar()->showMessage(tr("Read error: Device not Connected.") ,1000);
        return;
    }

    ui->meterPollNextPushButton->setEnabled(false);

    int meterNumber = ui->meterPollNumLineEdit->text().toInt();
    int times = (meterNumber - 1);

    handle_read(meterModelBaseAddress + (meterGap * times), meterEight, &meterPollModelReadReady);
    _sleep(3000);
    if (!meterExist) {
        meterExist = true;
        QMessageBox::information(NULL,  "INFO",  + "Meters " + QString::number(meterNumber) + " settings no exists!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        ui->meterPollNextPushButton->setEnabled(true);
        return;
    }

    handle_read(meterPollSN + (meterPollGap * times), meterEight, &meterPollSNHeadReadReady);
    _sleep(3000);
    handle_read(meterPollAdressMode + (meterPollGap * times), &meterPollAddHeadReadReady);
    _sleep(2000);
    handle_read(meterPollPriAddress + (meterPollGap * times), &meterPollPriHeadReadReady);
    _sleep(2000);
    handle_read(meterPollSecAddress + (meterPollGap * times), meterEight, &meterPollSecHeadReadReady);
    _sleep(2000);
    handle_read(meterPollStatus + (meterPollGap * times), &meterPollStatusHeadReadReady);
    _sleep(2000);
    handle_read(meterPollManu + (meterPollGap * times), meterEight, &meterPollManuHeadReadReady);
    _sleep(2000);
    handle_read(meterPollType + (meterPollGap * times), &meterPollTypeHeadReadReady);
    _sleep(2000);
    handle_read(meterPollVersion + (meterPollGap * times), &meterPollVerHeadReadReady);
    _sleep(2000);
    handle_read(meterPollBaudRate + (meterPollGap * times), meterTwo, &meterPollBaudHeadReadReady);
    _sleep(2000);

    meterPollList.clear();

    // handle_read(meterPollAlarmCode + (meterPollGap * times), &meterPollReadReady);
    // _sleep(2000);
    QProgressDialog pro(this);
    pro.setLabelText(tr("Processing... Please wait..."));
    pro.setRange(0, 100);
    pro.setModal(true);
    pro.setCancelButtonText(tr("Cancel"));
    pro.setValue(0);
    for (meterPollIndex = 0; meterPollIndex < 10; meterPollIndex++)
    {
        handle_read(meterPollChannel + (meterPollGap * times) + meterTWELVE * meterPollIndex, meterTWELVE, &meterPollReadReady);
        _sleep(2000);
        pro.setValue(meterPollIndex * 10);
    }
    pro.setValue(100);

    m_meterPollModelBody->updateData(meterPollList);
    for (int i = 0; i < meterPollList.count(); i++){
        if (meterPollList[i].attribute.isEmpty()){
            ui->meterPollBodyTableView->setRowHidden(i, true);
        }
    }

    ui->meterPollNextPushButton->setEnabled(true);
}

void MainWindow::on_meterPollClearPushButton_clicked()
{    
    meterPollList.clear();
    m_meterPollModelBody->updateData(meterPollList);

    for (int i = 0; i < meterPollList.count(); i++) {
        if (meterPollList[i].attribute.isEmpty()) {
            ui->meterPollBodyTableView->setRowHidden(i, true);
        }
    }
    ui->meterPollNumLineEdit->setText("1");

    for (int i = 0; i < 10; i++)
        m_meterPollModel->setItem(i, 1, new QStandardItem(""));
}
