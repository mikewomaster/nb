#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QDebug>

#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "ledcontrol.h"

void MainWindow::ledControlTypeReadReady()
{
    handle_read_ready(ui->ledControlComboBox, ui->ledControlPercentLineEdit);
}

void MainWindow::on_ledControlTypeCheck_clicked()
{
    handle_read(LEDControlTypeAddress, LEDControlEntry, &ledControlTypeReadReady);
}

void MainWindow::on_ledControlTypeApply_clicked()
{
    quint16 valueSet = ui->ledControlComboBox->currentIndex();
    valueSet = (valueSet << 8) + ui->ledControlPercentLineEdit->text().toShort();
    prepareSendModbusUnit(LEDControlTypeAddress, LEDControlEntry, valueSet);
}

void MainWindow::ledControlAutoReadReady()
{
    handle_read_ready(ui->ledControlAutoCombox);
}

void MainWindow::ledControlOnReadReady()
{
    handle_read_ready(ui->ledControlOnHour, ui->ledControlOffHour);
}

void MainWindow::ledControlOffReadReady()
{
    handle_read_ready(ui->ledControlOffHour, ui->ledControlOffMin);
}

void MainWindow::on_ledControlAutoCheck_clicked()
{
    handle_read(LEDControlAutoAddress, LEDControlEntry, &ledControlAutoReadReady);
    _sleep(2000);

    handle_read(LEDcontrolOnTimeAddress, LEDControlEntry, &ledControlOnReadReady);
    _sleep(2000);

    handle_read(LEDControlOffTimeAddress, LEDControlEntry, &ledControlOffReadReady);
    _sleep(2000);
}

void MainWindow::on_ledControlAutoApply_clicked()
{
    quint16 valueToSend;

    valueToSend = ui->ledControlAutoCombox->currentIndex();
    prepareSendModbusUnit(LEDControlAutoAddress, LEDControlEntry, valueToSend);

    valueToSend = ui->ledControlOnHour->currentIndex();
    valueToSend = (valueToSend << 8) + ui->ledControlOnMin->currentIndex();
    prepareSendModbusUnit(LEDcontrolOnTimeAddress, LEDControlEntry, valueToSend);

    valueToSend = ui->ledControlOffHour->currentIndex();
    valueToSend = (valueToSend << 8) + ui->ledControlOffMin->currentIndex();
    prepareSendModbusUnit(LEDcontrolOnTimeAddress, LEDControlEntry, valueToSend);
}
