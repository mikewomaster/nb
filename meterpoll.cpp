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

void MainWindow::meterPollReadReady()
{
    handle_read_ready(meterPollList, meterPollIndex);
}

void MainWindow::on_meterPollPushButton_clicked()
{
    if (!modbusDevice)
        return;

    if (modbusDevice->state() != QModbusDevice::ConnectedState ) {
        statusBar()->showMessage(tr("Read error: Device not Connected.") ,1000);
        return;
    }

    int meterNumber = ui->meterPollNumLineEdit->text().toInt();
    int times = (meterNumber - 1);

    handle_read(meterPollSN + (meterPollGap * times), meterEight, &meterPollSNHeadReadReady);
    _sleep(2000);
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
    for (meterPollIndex = 0; meterPollIndex < 10; meterPollIndex++)
    {
        if (meterPollIndex == 0)
        {
            handle_read(meterPollAlarmCode + (meterPollGap * times), &meterPollReadReady);
            _sleep(2000);
        }
        else
        {
            handle_read(meterPollChannel + (meterPollGap * times) + meterTWELVE * meterPollIndex, meterTWELVE, &meterPollReadReady);
            _sleep(2000);
        }
    }

    m_meterPollModelBody->updateData(meterPollList);
}

void MainWindow::on_meterPollNextPushButton_clicked()
{
    int times = ui->meterPollNumLineEdit->text().toInt();

    if (times == 10)
        times = 1;
    else
        times ++;

    ui->meterPollNumLineEdit->setText(QString::number(times));

    on_meterPollPushButton_clicked();
}
