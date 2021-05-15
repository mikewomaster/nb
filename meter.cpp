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

void MainWindow::on_meterCreatePushButton_clicked()
{
    if (meterProfileList.count() >= 15)
        return;

    meterProfile mp;
    mp.tag = ui->meterAttributeNameLineEdit->text();
    mp.id = ui->meterDataIndexLineEdit->text().toInt();
    mp.magnitude = ui->meterMagnitudeLineEdit->text();

    meterProfileList.append(mp);
    m_meterViewControl->updateData(meterProfileList);
}

void MainWindow::on_meterErasePushButton_clicked()
{
    meterProfileList.clear();
    m_meterViewControl->updateData(meterProfileList);
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

    nb_handle_write(ui->meterModelLineEdit, meterModelBaseAddress + (meterGap * times), meterEight);
    _sleep(2000);
    handle_write(ui->meterAddressModelComboBox, addressModeBaseAddress + (meterGap * times));
    _sleep(2000);
    nb_handle_write(ui->meterAddressLineEdit, meterAddressBaseAddress + (meterGap * times), meterEight);
    _sleep(2000);

    for (int i = 0; i < meterProfileList.count(); i++)
    {
        handle_write(meterProfileList[i].tag, meterTagBaseAddress + (meterGap * times) + i * 4, meterFour);
        _sleep(2000);
        handle_write(meterProfileList[i].id, meterIdBaseAddress + (meterGap * times) + i);
        _sleep(2000);
        handle_write(meterProfileList[i].magnitude, meterMagnitudeBaseAddress + (meterGap * times) + i * 4, meterFour);
        _sleep(2000);
    }
}

void MainWindow::meterModelReadReady()
{
   nb_handle_read_ready(ui->meterModelLineEdit);
}

void MainWindow::meterAddressModelReady()
{
    handle_read_ready(ui->meterAddressModelComboBox);
}

void MainWindow::meterAddressReadReady()
{
    nb_handle_read_ready(ui->meterAddressLineEdit);
}

static int meterProfileListIndex;

void MainWindow::meterProfileTagReadReady()
{
    handle_read_ready(meterProfileList, meterProfileListIndex);
}

void MainWindow::meterProfileIdReadReady()
{
    handle_read_ready(meterProfileList, meterProfileListIndex, 1);
}

void MainWindow::meterProfileMagnitudeReadReady()
{
    handle_read_ready(meterProfileList, meterProfileListIndex);
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

    handle_read(meterModelBaseAddress + (meterGap * times), meterEight, &meterModelReadReady);
    _sleep(2000);
    handle_read(addressModeBaseAddress + (meterGap * times), &meterAddressModelReady);
    _sleep(2000);
    handle_read(meterAddressBaseAddress + (meterGap * times), meterEight, &meterAddressReadReady);
    _sleep(2000);

    for (meterProfileListIndex = 0; meterProfileListIndex < 15; meterProfileListIndex++)
    {
        meterProfileList.clear();

        handle_read(meterTagBaseAddress + (meterGap * times) + meterProfileListIndex * 4, meterFour, &meterProfileTagReadReady);
        _sleep(2000);
        handle_read(meterIdBaseAddress + (meterGap * times) + meterProfileListIndex * 4, &meterProfileIdReadReady);
        _sleep(2000);
        handle_read(meterMagnitudeBaseAddress + (meterGap * times) + meterProfileListIndex * 4, meterFour, &meterProfileMagnitudeReadReady);
        _sleep(2000);
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
