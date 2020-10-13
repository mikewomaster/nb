#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QFile>
#include <QIODevice>
#include <QFileDialog>
#include <QDebug>

#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "ssl.h"
#include "commanhelper.h"

static QStringList fileNames;
static QStringList fileName;
static QByteArray fileByteArray;

void MainWindow::on_sslModeCheck_clicked()
{

}

void MainWindow::on_sslModeSet_clicked()
{

}

void MainWindow::sslFileRead(quint8 type)
{
    if (fileNames.at(0) == "")
        return;

    fileByteArray.clear();

    QFile sslFile(fileNames.at(0));
    if(!sslFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        statusBar()->showMessage("Open File Failed!");
    }

    QByteArray ba;
    ba = sslFile.readAll();

    fileByteArray[0] = 0xaa;
    fileByteArray[1] = 0xbb;
    fileByteArray[2] = 0xcc;
    fileByteArray[3] = type;
    fileByteArray.append(ba);
}

void MainWindow::sslImportFile(quint8 type)
{
    if (modbusDevice)
            modbusDevice->disconnectDevice();

    if (m_serial == nullptr) {
        m_serial = new QSerialPort;
        m_serial->setPortName(ui->portComboBox->currentText());
        m_serial->setBaudRate(QSerialPort::Baud9600);
        m_serial->setDataBits(QSerialPort::Data8);
        m_serial->setParity(QSerialPort::NoParity);
        m_serial->setStopBits(QSerialPort::OneStop);
        m_serial->setFlowControl(QSerialPort::NoFlowControl);

        if (m_serial->open(QIODevice::ReadWrite)) {
            //QMessageBox::information(this, tr("OK"), "open ok");
        } else {
            //QMessageBox::critical(this, tr("Error"), m_serial->errorString());
        }

    } else {
        m_serial->setPortName(ui->portComboBox->currentText());
        m_serial->setBaudRate(QSerialPort::Baud9600);
        m_serial->setDataBits(QSerialPort::Data8);
        m_serial->setParity(QSerialPort::NoParity);
        m_serial->setStopBits(QSerialPort::OneStop);
        m_serial->setFlowControl(QSerialPort::NoFlowControl);

        if (m_serial->open(QIODevice::ReadWrite)) {
            //QMessageBox::information(this, tr("OK"), "open ok");
        } else {
            //QMessageBox::critical(this, tr("Error"), m_serial->errorString());
        }
    }
    statusBar()->clearMessage();

    sslFileRead(type);
    int ret = m_serial->write(fileByteArray);
    /*
        if (ret == 4) {
            // QMessageBox::information(this, tr("OK"), "write cmd ok");
        }else {
            // QMessageBox::information(this, tr("fail"), "write fail");
        }
    */
    _sleep(100);

    m_serial->close();
    modbusDevice->connectDevice();
}

void MainWindow::on_rootCAImport_clicked()
{
    sslImportFile(0x01);
}

void MainWindow::on_cfImport_clicked()
{
    sslImportFile(0x02);
}

void MainWindow::on_keyFileImport_clicked()
{
    sslImportFile(0x03);
}

void MainWindow::sslFile()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setViewMode(QFileDialog::Detail);

    fileNames.clear();
    fileName.clear();

    if (fileDialog->exec()) {
        fileNames = fileDialog->selectedFiles();
        fileName = fileNames.at(0).split("/");
        fileDialog->close();
      }

    delete fileDialog;
}

void MainWindow::on_rootCASelect_clicked()
{
    sslFile();
    ui->rootCALabel->clear();
    ui->rootCALabel->setText(fileName.last());
}


void MainWindow::on_cfSelect_clicked()
{
    sslFile();
    ui->cfLabel->clear();
    ui->cfLabel->setText(fileName.last());
}

void MainWindow::on_keyFileSelect_clicked()
{
    sslFile();
    ui->keyFileLabel->clear();
    ui->keyFileLabel->setText(fileName.last());
}
