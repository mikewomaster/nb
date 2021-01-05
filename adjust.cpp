#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>

#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "adjust.h"

// VIN Reference Voltage
void MainWindow::vinRefReadReady()
{
    handle_read_ready(ui->vinLineEdit);
}

void MainWindow::on_vinRead_clicked()
{
    handle_read(vinRef, &vinRefReadReady);
}

void MainWindow::on_vinWrite_clicked()
{
    handle_write(ui->vinLineEdit, vinRef);
}

// VOut Reference Voltage
void MainWindow::voutRefReadReady()
{
    handle_read_ready(ui->voutLineEdit);
}

void MainWindow::on_voutWrite_clicked()
{
    handle_write(ui->voutLineEdit, voutRef);
}

void MainWindow::on_voutRead_clicked()
{
    handle_read(voutRef, &voutRefReadReady);
}

//IInp Reference Voltage
void MainWindow::iinRefReadReady()
{
    handle_read_ready(ui->iinLineEdit);
}

void MainWindow::on_iinWrite_clicked()
{
    handle_write(ui->iinLineEdit, iinRef);
}

void MainWindow::on_iinRead_clicked()
{
    handle_read(iinRef, &iinRefReadReady);
}


// Iout Reference Voltage
void MainWindow::on_ioutWrite_clicked()
{
    handle_write(ui->ioutLineEdit, ioutRef);
}

void MainWindow::ioutReadReady()
{
    handle_read_ready(ui->ioutLineEdit);
}

void MainWindow::on_ioutRead_clicked()
{
    handle_read(ioutRef, &ioutReadReady);
}

// factor 1
void MainWindow::on_factor1Write_clicked()
{
    handle_write(ui->factor1LineEdit, adjust1);
}

void MainWindow::factor1ReadReady()
{
    handle_read_ready(ui->factor1LineEdit);
}

void MainWindow::on_factor1Read_clicked()
{
    handle_read(adjust1, &factor1ReadReady);
}

// factor 2
void MainWindow::on_factor2Write_clicked()
{
    handle_write(ui->factor2LineEdit, adjust2);
}

void MainWindow::factor2ReadReady()
{
    handle_read_ready(ui->factor2LineEdit);
}

void MainWindow::on_factor2Read_clicked()
{
    handle_read(adjust2, &factor2ReadReady);
}

// factor 3
void MainWindow::on_factor3Write_clicked()
{
    handle_write(ui->factor3LineEdit, adjust3);
}

void MainWindow::factor3ReadReady()
{
    handle_read_ready(ui->factor3LineEdit);
}

void MainWindow::on_factor3Read_clicked()
{
    handle_read(adjust3, &factor3ReadReady);
}

// factor 4
void MainWindow::on_factor4Write_clicked()
{
    handle_write(ui->factor4LineEdit, adjust4);
}

void MainWindow::factor4ReadReady()
{
    handle_read_ready(ui->factor4LineEdit);
}

void MainWindow::on_factor4Read_clicked()
{
    handle_read(adjust4, &factor4ReadReady);
}

// factor 5
void MainWindow::on_factor5Write_clicked()
{
    handle_write(ui->factor5LineEdit, adjust5);
}

void MainWindow::factor5ReadReady()
{
    handle_read_ready(ui->factor5LineEdit);
}

void MainWindow::on_factor5Read_clicked()
{
    handle_read(adjust5, &factor5ReadReady);
}

// factor 6
void MainWindow::on_factor6Write_clicked()
{
    handle_write(ui->factor6LineEdit, adjust6);
}

void MainWindow::factor6ReadReady()
{
    handle_read_ready(ui->factor6LineEdit);
}

void MainWindow::on_factor6Read_clicked()
{
    handle_read(adjust6, &factor6ReadReady);
}
