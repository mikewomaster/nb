#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QDebug>

#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "sl102.h"

///////////////////////////////////////////
/// \brief for the page energy
///  in sl102 project
///////////////////////////////////////////
void MainWindow::energyVReadReady()
{
    handle_read_ready(ui->energyVLineEdit);
}

void MainWindow::on_energyVPushButton_clicked()
{
    handle_read(EnergyVoltageAddress, &energyVReadReady);
}

void MainWindow::energyCReadReady()
{
    handle_read_ready(ui->energyCLineEdit);
}

void MainWindow::on_energyCPushButton_clicked()
{
    handle_read(EnergyAmpAddress, &energyCReadReady);
}

void MainWindow::energyPReadReady()
{
    handle_read_ready(ui->energyPLineEdit);
}

void MainWindow::on_energyPPushButton_clicked()
{
    handle_read(EnergyPowerAddress, &energyPReadReady);
}

void MainWindow::energyVRMSPReadReady()
{
    handle_read_ready(ui->energyVRmsLineEdit);
}

void MainWindow::on_energyVRMSPushButton_clicked()
{
    handle_read(EnergyVRMSAddress, &energyVRMSPReadReady);
}

void MainWindow::energyCRMSReadReady()
{
    handle_read_ready(ui->energyCRmsLineEdit);
}

void MainWindow::on_energyCRMSPushButton_clicked()
{
    handle_read(EnergyIRMSAddress, &energyCRMSReadReady);
}

void MainWindow::energyPFReadReady()
{
    handle_read_ready(ui->energyPFLineEdit);
}

void MainWindow::on_energyPFPushButton_clicked()
{
    handle_read(EnergyPFAddress, &energyPFReadReady);
}

void MainWindow::energyACReadReady()
{
    handle_read_ready(ui->energyACLineEdit);
}

void MainWindow::on_energyACCheckPushButton_clicked()
{
    handle_read(ACFrequencyAddress, &energyACReadReady);
}

void MainWindow::on_energyACApplyPushButton_clicked()
{
    handle_write(ui->energyACLineEdit, ACFrequencyAddress);
}

/////////////////////////////////////////
/// \brief led control test for slc102
///////////////////////////////////////////
void MainWindow::on_ledControlOn_clicked()
{
    prepareSendModbusUnit(controlTestOnAddress, EnergyOneEntry, 1);
}

void MainWindow::on_ledControlOff_clicked()
{
    prepareSendModbusUnit(controlTestOnAddress, EnergyOneEntry, 0);
}

void MainWindow::on_ledControlSlider_sliderReleased()
{
    uint16_t valueToSend = ui->ledControlSlider->value();
    prepareSendModbusUnit(controlTestOnAddress, EnergyOneEntry, valueToSend);
}

////////////////////////////////////////////
/// \brief CMS Check Page
/////////////////////////////////////////////////
void MainWindow::cmsTSReadReady()
{

}

void MainWindow::on_cmsCheckPushButton_clicked()
{
    handle_read(cmsTSAddress, Entry2, &cmsTSReadReady);
    _sleep(2000);




}


/*
void MainWindow::mbusTimeStampReadReady()
{

}

void MainWindow::on_mbusTSRead_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    int multiplyTerm = ui->mbusRegister->currentIndex();
    quint16 ADDR = mbusTimeStampBase + 20 * multiplyTerm;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, mbusTimeStampMapEntries);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::mbusTimeStampReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

*/
