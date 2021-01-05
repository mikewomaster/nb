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
