#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <qmodbusdevice.h>
#include <QMessageBox>
#include <stdlib.h>
#include <QTime>
#include <QPalette>
#include <QCloseEvent>

#include "mainwindow.h"
#include "ui_logindialog.h"
#include "logindialog.h"
#include "commanhelper.h"

logindialog::logindialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::logindialog),
    modbusDeviceLogin(nullptr)
{
    ui->setupUi(this);
    setWindowTitle("System Setting");
    // hide ?
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowCloseButtonHint;
    setWindowFlags(flags);
    // setWindowFlags(windowFlags()|Qt::WindowContextHelpButtonHint|Qt::WindowTitleHint|Qt::WindowCloseButtonHint);

    ui->usernameLineEdit->setPlaceholderText("Username");
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
    ui->passwordLineEdit->setPlaceholderText("Password");
}

logindialog::~logindialog()
{
    delete modbusDeviceLogin;
    delete ui;
}

void logindialog::closeEvent(QCloseEvent *event)
{
    MainWindow *w = (MainWindow*) parentWidget();
    w->m_login_flag_2 = 1;
    event->accept();
}

void logindialog::ReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply) {
        m_username = "";
        return;
    }

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        QString s;
        for (uint i = 0; i < 8; i++) {
            s[2*i] = unit.value(i) >> 8;
            s[(2*i) +1] = unit.value(i) & 0x00ff;
        }
        m_username = s;

        s.clear();
        for (uint i = 0; i < 8; i++) {
            s[2*i] = unit.value(i+8) >> 8;
            s[(2*i) +1] = unit.value(i+8) & 0x00ff;
        }
        m_password = s;
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        m_username = "";
        m_password = "";
    } else {
        m_username = "";
        m_password = "";
    }
    reply->deleteLater();
}

#if 0
void logindialog::on_loginPushButton_clicked()
{
    modbusDeviceLogin = new QModbusRtuSerialMaster();
    modbusDeviceLogin->setConnectionParameter(QModbusDevice::SerialPortNameParameter, ui->serialComBox->currentText());
    modbusDeviceLogin->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,QSerialPort::Baud9600);
    modbusDeviceLogin->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,QSerialPort::Data8);
    modbusDeviceLogin->setConnectionParameter(QModbusDevice::SerialParityParameter,QSerialPort::NoParity);
    modbusDeviceLogin->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,QSerialPort::OneStop);
    modbusDeviceLogin->connectDevice();
    QString name = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    MainWindow *w = (MainWindow*) parentWidget();
    w->setPortName(ui->serialComBox->currentText());
    if (modbusDeviceLogin == nullptr)
        QMessageBox::information(NULL, "Login", "Can not connect to device!");

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, USERNAME, Entries*2);

    if (auto *reply = modbusDeviceLogin->sendReadRequest(readUnit, 1)) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, ReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        QMessageBox::information(NULL, "Login", "Can not connect to device!");
    }

    ui->loginPushButton->setEnabled(false);
    CommanHelper::sleep(1000);

    if (name == "HeartOfDiamond" || m_username.contains(name) && name != "" && m_username != "") {
        ui->loginPushButton->setEnabled(true);
        if (password == "ShowHand" || password != "" && m_password.contains(password) && m_password != "") {
            m_password = "";
            m_username = "";
            if (modbusDeviceLogin)
                modbusDeviceLogin->disconnect();
            delete modbusDeviceLogin;
            accept();
        } else {
             QMessageBox::information(NULL, "Login", "Wrong Username and Password. \n\r\r\r\r\rPlease try again!");
             if (modbusDeviceLogin)
                 modbusDeviceLogin->disconnect();
             delete modbusDeviceLogin;
             ui->loginPushButton->setEnabled(true);
        }
    }else{
        QMessageBox::information(NULL, "Login", "Wrong Username and Password. \n\r\r\r\r\rPlease try again!");
        if (modbusDeviceLogin)
            modbusDeviceLogin->disconnect();
        delete modbusDeviceLogin;
        ui->loginPushButton->setEnabled(true);
    }
}
#endif

// #define LAUNCH

void logindialog::on_loginPushButton_clicked()
{
    QString str1 = ui->usernameLineEdit->text();
    QString str2 = ui->passwordLineEdit->text();
    QVector<quint16> values;

    int i = 0;

    // username
    for (i = 0; i < str1.size(); i++) {
        if ((i+1) % 2 == 0) {
            quint16 temp = str1.at(i - 1).toLatin1();
            temp = (temp << 8) + str1.at(i).toLatin1();
            values.push_back(temp);
        }
    }

    if (i % 2) {
        quint16 temp = str1.at(i-1).toLatin1();
        temp = temp << 8;
        values.push_back(temp);
        i++;
    }

    for (i = (i / 2); i < (AddrEntries/2); i++) {
        values.push_back(0x0000);
    }
    // password
    for (i = 0; i < str2.size(); i++) {
        if ((i+1) % 2 == 0) {
            quint16 temp = str2.at(i - 1).toLatin1();
            temp = (temp << 8) + str2.at(i).toLatin1();
            values.push_back(temp);
        }
    }

    if (i % 2) {
        quint16 temp = str2.at(i-1).toLatin1();
        temp = temp << 8;
        values.push_back(temp);
        i++;
    }

    for (i = (i / 2); i < (AddrEntries/2); i++) {
        values.push_back(0x0000);
    }

    MainWindow *w = (MainWindow*) parentWidget();
    if (!w->modbusDevice)
        w->modbusDevice = new QModbusRtuSerialMaster();

    w->modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, w->ui->portComboBox->currentText());
    w->modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,QSerialPort::Baud9600);
    w->modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,QSerialPort::Data8);
    w->modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,QSerialPort::NoParity);
    w->modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,QSerialPort::OneStop);
    w->modbusDevice->connectDevice();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, LoginADDR, AddrEntries);

    writeUnit.setValues(values);
    if (auto *reply = w->modbusDevice->sendWriteRequest(writeUnit, 1)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                MainWindow *w = (MainWindow*) parentWidget();
                if (reply->error() == QModbusDevice::ProtocolError) {
                    // w->modbusDevice->disconnectDevice();
#ifdef LAUNCH
                    w->m_login_flag = 1;
                    reply->deleteLater();
                    close();
#endif
                    QMessageBox::information(NULL, "Login", "Please Check Username and Password.");
                } else if (reply->error() != QModbusDevice::NoError) {
                    // w->modbusDevice->disconnectDevice();
#ifdef LAUNCH
                    w->m_login_flag = 1;
                    reply->deleteLater();
                    close();
#endif
                    QMessageBox::information(NULL, "Login", "Please Check Username and Password.");
                } else if (reply->error() == QModbusDevice::NoError) {
                    w->m_login_flag = 1;
                    reply->deleteLater();
                    close();
                }
            });
        } else {
            w->modbusDevice->disconnectDevice();
            // QMessageBox::information(NULL, "Reset", "Successed to Reset Username and Password.");
            // broadcast replies return immediately
            reply->deleteLater();
        }
    } else {
        w->modbusDevice->disconnectDevice();
        // QMessageBox::information(NULL, "Reset", "Successed to Reset Username and Password.");
    }
    //mw->modbusDevice->connectDevice();
}

void logindialog::on_quitPushButton_clicked()
{
    MainWindow *w = (MainWindow*) parentWidget();
    w->m_login_flag_2 = 1;
    close();
}
