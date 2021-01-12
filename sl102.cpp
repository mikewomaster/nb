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
    handle_read_ready_divide_ten(ui->energyVLineEdit);
}

void MainWindow::on_energyVPushButton_clicked()
{
    handle_read(EnergyVoltageAddress, &energyVReadReady);
}

void MainWindow::energyCReadReady()
{
    handle_read_ready_divide_ten(ui->energyCLineEdit);
}

void MainWindow::on_energyCPushButton_clicked()
{
    handle_read(EnergyAmpAddress, &energyCReadReady);
}

void MainWindow::energyPReadReady()
{
    handle_read_ready_divide_ten(ui->energyPLineEdit);
}

void MainWindow::on_energyPPushButton_clicked()
{
    handle_read(EnergyPowerAddress, &energyPReadReady);
}

void MainWindow::energyVRMSPReadReady()
{
    handle_read_ready_divide_ten(ui->energyVRmsLineEdit);
}

void MainWindow::on_energyVRMSPushButton_clicked()
{
    handle_read(EnergyVRMSAddress, &energyVRMSPReadReady);
}

void MainWindow::energyCRMSReadReady()
{
    handle_read_ready_divide_ten(ui->energyCRmsLineEdit);
}

void MainWindow::on_energyCRMSPushButton_clicked()
{
    handle_read(EnergyIRMSAddress, &energyCRMSReadReady);
}

void MainWindow::energyPFReadReady()
{
    handle_read_ready_divide_ten(ui->energyPFLineEdit);
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

////////////////////////////////////////////
/// \brief CMS Check Page
/////////////////////////////////////////////////
void MainWindow::cmsTSReadReady()
{
    handle_read_ready(ui->cmsTSLineEdit, 1);
}

void MainWindow::cmsACKReadReady()
{
    nb_handle_read_ready(ui->cmsACKLineEdit);
}

void MainWindow::cmsRVReadReady()
{
    nb_handle_read_ready(ui->cmsReturnLineEdit);
}

void MainWindow::cmsDimmingReadReady()
{
    handle_read_ready(ui->cmsDimmingLineEdit);
}

void MainWindow::cmsDelayReadReady()
{
    handle_read_ready(ui->cmsDelayLineEdit);
}

void MainWindow::cmsPeriodReadReady()
{
    handle_read_ready(ui->cmsPeriodLineEdit);
}

void MainWindow::on_cmsCheckPushButton_clicked()
{
    ui->cmsCheckPushButton->setEnabled(false);

    handle_read(cmsTSAddress, Entry2, &cmsTSReadReady);
    _sleep(2000);

    handle_read(cmsACKAddress, Entry16, &cmsACKReadReady);
    _sleep(2000);

    handle_read(cmsRVAddress, Entry16, &cmsRVReadReady);
    _sleep(2000);

    handle_read(cmsDimmingAddress, &cmsDimmingReadReady);
    _sleep(2000);

    handle_read(cmsDelayAddress, &cmsDelayReadReady);
    _sleep(2000);

    handle_read(cmsPeriodAddress, &cmsPeriodReadReady);
    _sleep(2000);

    ui->cmsCheckPushButton->setEnabled(true);
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
