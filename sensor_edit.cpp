#include "sensor_edit.h"
#include "ui_sensor_edit.h"
#include "mainwindow.h"
#include "sensor.h"
#include <QMessageBox>

sensor_edit::sensor_edit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sensor_edit)
{
    ui->setupUi(this);
}

sensor_edit::~sensor_edit()
{
    delete ui;
}

void sensor_edit::on_sensorAddPushButton_clicked()
{
     MainWindow *w = (MainWindow*) parentWidget();
     if (ui->sensorSlaveId->text().toShort() < 0 || ui->sensorSlaveId->text().toShort() >255) {
         QMessageBox::information(NULL, "Error", "Please set ID number between 0 and 255.");
         return;
     }

     if (ui->sensorPLCAddress->text().toInt() < 1 || ui->sensorPLCAddress->text().toInt() > 10001) {
         QMessageBox::information(NULL, "Error", "Please set Register Address between 1 and 10001.");
         return;
     }

     if (ui->sensorTypeLE->text().length() > 8) {
         QMessageBox::information(NULL, "Error", "Please set RTU number with maximum 8 word.");
         return;
     }

     if (ui->sensorTypeLE->text().length() == 0) {
         QMessageBox::information(NULL, "Error", "Please set RTU number.");
         return;
     }

     if (ui->sensorLLE->text().toInt() < 1 || ui->sensorLLE->text().toInt() > 10) {
         QMessageBox::information(NULL, "Error", "Please set Length number between 1 and 10.");
         return;
     }

     w->sensorRecordList[seq].type = ui->sensorTypeLE->text();
     w->sensorRecordList[seq].id = ui->sensorSlaveId->text().toShort();
     w->sensorRecordList[seq].reg_addr = ui->sensorPLCAddress->text().toInt();
     w->sensorRecordList[seq].len = ui->sensorLLE->text().toInt();
     w->sensorRecordList[seq].value = "null";
     w->sensor_edit_flag = true;
     close();
}
