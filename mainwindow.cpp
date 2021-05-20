/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the QtSerialBus module.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "logdialog.h"
#include "system.h"
#include "writeregistermodel.h"
#include "netmodel.h"
#include "logindialog.h"
#include "loginruledialog.h"
#include "ymodem.h"
#include "metermodelviewcontrol.h"

#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QUrl>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QTime>
#include <QSize>
#include <QDebug>
#include <QMessageBox>

enum ModbusConnection {
    Serial,
    Tcp
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , lastRequest(nullptr)
    , modbusDevice(nullptr)
    , m_Model(new QStandardItemModel())
    , m_serial(new QSerialPort)
    , m_login_flag(0)
    , m_login_flag_2(0)
    , m_ymodem(new ymodem())
{
    ui->setupUi(this);

    QString title = UtilityVersion;
    setWindowTitle(title);

    m_settingsDialog = new SettingsDialog(this);
    m_logdialog = new logdialog(this);
    m_system = new systemDialog(this);
    // m_sensor_dialog = new sensor_edit(this);
    m_obis_edit_dialog = new obis_edit(this);

    initActions();

    writeModel = new WriteRegisterModel(this);

    ui->connectType->setCurrentIndex(0);
    on_connectType_currentIndexChanged(0);

    sensorFlag = false;
    sensor_edit_flag = false;
    obis_edit_flag = false;

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        serialInfoVector.push_back(info.portName());
        ui->portComboBox->addItem(info.portName());
    }

    m_Model->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("Address")));
    m_Model->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Mode")));
    m_Model->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("SerialNumber ")));
    m_Model->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("Manu")));
    m_Model->setHorizontalHeaderItem(4, new QStandardItem(QObject::tr("Version")));
    m_Model->setHorizontalHeaderItem(5, new QStandardItem(QObject::tr("Medium")));
    // m_Model->setHorizontalHeaderItem(6, new QStandardItem(QObject::tr("Type")));
    m_Model->setHorizontalHeaderItem(6, new QStandardItem(QObject::tr("Value")));
    m_Model->setHorizontalHeaderItem(7, new QStandardItem(QObject::tr("Unit")));
    // m_Model->setHorizontalHeaderItem(8, new QStandardItem(QObject::tr("Scale")));
    m_Model->setHorizontalHeaderItem(8, new QStandardItem(QObject::tr("Description")));
    m_Model->setHorizontalHeaderItem(9, new QStandardItem(QObject::tr("TimeStamp  ")));

    ui->connectType->hide();
    ui->serverEdit->hide();
    ui->syncWordLineEdit->hide();
    ui->OptimizeLowRateComboBox->hide();

    ui->tabWidget->setTabEnabled(6, false);
    ui->groupBox_6->hide();

#ifdef LORA
    ui->tabWidget->show();
    ui->tabWidget_2->hide();
#else
    ui->tabWidget_2->show();
    ui->tabWidget->hide();
/*
    ui->tabWidget_2->setTabEnabled(0, false);
    ui->tabWidget_2->setTabEnabled(2, false);
    ui->tabWidget_2->setTabEnabled(3, false);
    ui->tabWidget_2->setTabEnabled(4, false);
    ui->tabWidget_2->setTabEnabled(5, false);
    ui->tabWidget_2->setTabEnabled(6, false);
    ui->tabWidget_2->setTabEnabled(7, false);
    ui->tabWidget_2->setTabEnabled(8, false);
*/
    ui->tabWidget_2->setTabEnabled(3, false);
    ui->tabWidget_2->setTabEnabled(4, false);
    ui->tabWidget_2->setTabEnabled(5, false);
    ui->tabWidget_2->setTabEnabled(6, false);
    ui->tabWidget_2->setTabEnabled(7, false);
    ui->tabWidget_2->setTabEnabled(8, false);
    ui->tabWidget_2->setTabEnabled(9, false);
    ui->tabWidget_2->setTabEnabled(10, false);
    ui->tabWidget_2->setTabEnabled(11, false);
    ui->tabWidget_2->setTabEnabled(12, false);
#endif

    ui->groupBox_13->hide();
    ui->mbusRegister->hide();
    ui->mbusPrimaryEdit->setText("1");
    ui->mbusPrimaryEdit->hide();

    // hide button
    ui->mbusPrimaryRead_12->hide();
    ui->mbusPrimaryWrite_11->hide();
    ui->mbusSecondaryRead->hide();
    ui->mbusSecondaryWrite->hide();
    ui->mbusReadoutRead->hide();
    ui->mbusReadoutWrite->hide();
    ui->mbusTSRead->hide();
    ui->mbusTSWrite->hide();

    ui->apnRead->hide();
    ui->apnWrite->hide();
    ui->userRead->hide();
    ui->userWrite->hide();
    ui->passwordRead->hide();
    ui->passwordWrite->hide();
    ui->nbStatusRead->hide();
    ui->nbStatusWrite->hide();
    ui->ipRead->hide();
    ui->ipWrite->hide();

    ui->srvRead->hide();
    ui->srvWrite->hide();
    ui->portRead->hide();
    ui->portWrite->hide();
    ui->topicRead->hide();
    ui->topicWrite->hide();
    ui->idRead->hide();
    ui->idWrite->hide();

    ui->mqttStatusRead->hide();
    ui->pidButtonWrite_8->hide();

    // ui->intervalLineEdit->hide();
    // ui->intervalRead->hide();
    // ui->idWrite_2->hide();

    ui->mbusSecondaryEdit->hide();
    ui->mbusSecondaryRead->hide();
    ui->mbusSecondaryWrite->hide();

    // SN hide
    ui->SNLineEdit->hide();
    ui->SNPushButton->hide();
    ui->SNPushButtonWrite->hide();

    // menubar hide
    ui->menuDevice->setVisible(false);
    ui->plmnRead->hide();
    ui->plmnWrite->hide();
    ui->mbusReadOutEdit->hide();

    // hide for mbus product
    ui->nbEnableRadioButton->hide();
    ui->nbmodelCombox->hide();
    ui->nbModelLabel->hide();
    ui->groupBox_22->hide();
    ui->mqttPasswordLineEdit_2->hide();
    ui->label_105->hide();
    ui->label_107->hide();
    ui->mbusPrimaryAddressEdit_12->hide();
    ui->mbusDeviceMode_12->hide();
    ui->pushButton_8->hide();
#ifdef TEST_DATA
    QList<QStandardItem *> item;
    item.append(new QStandardItem(QObject::tr("Always")));
    item.append(new QStandardItem(QObject::tr("primary")));
    item.append(new QStandardItem(QObject::tr("201911040001")));
    item.append(new QStandardItem(QObject::tr("WM")));
    item.append(new QStandardItem(QObject::tr("0.0.1")));
    item.append(new QStandardItem(QObject::tr("Heat")));
    item.append(new QStandardItem(QObject::tr("2 BCD")));
    item.append(new QStandardItem(QObject::tr("J/h")));
    item.append(new QStandardItem(QObject::tr("0.001")));
    item.append(new QStandardItem(QObject::tr("99.6")));
    item.append(new QStandardItem(QObject::tr("HMS Heat")));
    m_Model->appendRow(item);
    item.clear();

    item.append(new QStandardItem(QObject::tr("2")));
    item.append(new QStandardItem(QObject::tr("primary")));
    item.append(new QStandardItem(QObject::tr("201911040001")));
    item.append(new QStandardItem(QObject::tr("WM")));
    item.append(new QStandardItem(QObject::tr("0.0.1")));
    item.append(new QStandardItem(QObject::tr("Heat")));
    item.append(new QStandardItem(QObject::tr("2 BCD")));
    item.append(new QStandardItem(QObject::tr("J/h")));
    item.append(new QStandardItem(QObject::tr("0.001")));
    item.append(new QStandardItem(QObject::tr("99.6")));
    item.append(new QStandardItem(QObject::tr("HMS Heat")));
    m_Model->appendRow(item);
    item.clear();

    item.append(new QStandardItem(QObject::tr("3")));
    item.append(new QStandardItem(QObject::tr("primary")));
    item.append(new QStandardItem(QObject::tr("201911040001")));
    item.append(new QStandardItem(QObject::tr("WM")));
    item.append(new QStandardItem(QObject::tr("0.0.1")));
    item.append(new QStandardItem(QObject::tr("Heat")));
    item.append(new QStandardItem(QObject::tr("2 BCD")));
    item.append(new QStandardItem(QObject::tr("J/h")));
    item.append(new QStandardItem(QObject::tr("0.001")));
    item.append(new QStandardItem(QObject::tr("99.6")));
    item.append(new QStandardItem(QObject::tr("HMS Heat Downtown Man Shoot 3!")));
    m_Model->appendRow(item);
    item.clear();
#endif

    ui->tableView->setModel(m_Model);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // resize
#if 0
    QWidget *widgetAdjust = new QWidget(this);
    setCentralWidget(widgetAdjust);
    QVBoxLayout *windowLayoutAdjust = new QVBoxLayout;
    windowLayoutAdjust->addLayout(ui->gridLayout, 1);
    windowLayoutAdjust->addWidget(ui->tabWidget_2);
    widgetAdjust->setLayout(windowLayoutAdjust);
#endif

    // sensor Check Push Button
    ui->sensorCheckPushButton->setVisible(false);
    ui->sensorTypeCombox->setVisible(false);

    // nb
    ui->nbModelRead->hide();
    ui->nbModeWrite->hide();

    ui->BitMapTextEdit->hide();

    ui->label_195->hide();
    ui->sensorLength->hide();

    // mbus
    ui->pushButton_11->hide();
    ui->label_19->hide();
    ui->mbusCmdType->hide();

    // table view
    m_pModel = new NetModel(nullptr);
    ui->netBitMapTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->netBitMapTableView->setShowGrid(false);
    ui->netBitMapTableView->setFrameShape(QFrame::NoFrame);
    ui->netBitMapTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->netBitMapTableView->setModel(m_pModel);
    ui->netBitMapTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->coapInterval->hide();
    ui->dlmsPwd->setEnabled(false);

    // dlms table view with dlmsModel
    obis_view_model_init();

    // sensor view and sensor model init
    sensor_view_model();

    meterViewModelInit();
    meterPollViewModelInit();
    // logical rule init
    // logicalRuleViewInit();

    // mbus model init
    QList<Device> recordList;
    for (int i = 1; i <= 250; ++i)
    {
        Device record;
        record.bChecked = false;
        record.id = i;
        recordList.append(record);
    }
    m_pModel->updateData(recordList);

    // meter table view
    m_meterPollModel = new QStandardItemModel();
    ui->meterPollHeadTableView->setModel(m_meterPollModel);
    // ui->meterPollHeadTableView->horizontalHeader()->setR

    m_meterPollModel->setColumnCount(2);
    m_meterPollModel->setHeaderData(0, Qt::Horizontal, "Model");
    m_meterPollModel->setHeaderData(1, Qt::Horizontal, "Hydrus");
    ui->meterPollHeadTableView->setColumnWidth(0, 190);
    ui->meterPollHeadTableView->setColumnWidth(1, 200);

    m_meterPollModel->setItem(0, 0, new QStandardItem("Serial Number"));
    m_meterPollModel->setItem(1, 0, new QStandardItem("Addressing Mode"));
    m_meterPollModel->setItem(2, 0, new QStandardItem("Primary Address"));
    m_meterPollModel->setItem(3, 0, new QStandardItem("Secondary Address"));
    m_meterPollModel->setItem(4, 0, new QStandardItem("Status"));
    m_meterPollModel->setItem(5, 0, new QStandardItem("Manufacture"));
    m_meterPollModel->setItem(6, 0, new QStandardItem("Type"));
    m_meterPollModel->setItem(7, 0, new QStandardItem("Version"));
    m_meterPollModel->setItem(8, 0, new QStandardItem("Baudrate"));

    for (int i = 0; i < 9; i++)
    {
        m_meterPollModel->setItem(i, 1, new QStandardItem(" "));
    }

    serialAlarmInit();

    ui->tabWidget->setStyleSheet("QTabBar::tab:disabled {width: 0; color: transparent;}");
    ui->tabWidget_2->setStyleSheet("QTabBar::tab:disabled {width: 0; color: transparent;}");
    update();
}

MainWindow::~MainWindow()
{
    if (modbusDevice)
        modbusDevice->disconnectDevice();
    delete modbusDevice;

    delete ui;
}

void MainWindow::initActions()
{
#ifdef ACTION
    ui->actionConnect->setEnabled(true); 
    ui->actionDisconnect->setEnabled(false);
    ui->actionExit->setEnabled(true);
    ui->actionOptions->setEnabled(true);

    connect(ui->actionConnect, &QAction::triggered,this, &MainWindow::on_connectButton_clicked);
    connect(ui->actionDisconnect, &QAction::triggered,this, &MainWindow::on_connectButton_clicked);
    connect(ui->actionExit, &QAction::triggered, this, &QMainWindow::close);
    connect(ui->actionOptions, &QAction::triggered, m_settingsDialog, &QDialog::show);
    //connect(ui->actionLog, &QAction::triggered, m_logdialog, &QDialog::show);
#endif
    ui->action_Settings->setEnabled(true);
    connect(ui->action_Settings, &QAction::triggered, m_system, &QDialog::show);

    ui->action_Default->setEnabled(true);
    connect(ui->action_Default, &QAction::triggered, this, &MainWindow::defaultTheme);
    ui->actionDa_rk->setEnabled(true);
    connect(ui->actionDa_rk, &QAction::triggered, this, &MainWindow::darkTheme);

    connect(m_ymodem, SIGNAL(finRcv(QString)), this, SLOT(parseEventLog(QString)));
}

void MainWindow::logicalRuleViewInit()
{
    m_logicalRuleModel = new logicalRuleModel(this);
    ui->logicalRuleTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->logicalRuleTableView->setShowGrid(true);
    ui->logicalRuleTableView->setFrameShape(QFrame::Box);
    ui->logicalRuleTableView->setSelectionMode(QAbstractItemView::SingleSelection);

    QList<QStandardItem *> item;
    item.append(new QStandardItem(QObject::tr("#1")));
    item.append(new QStandardItem(QObject::tr("Enable")));
    item.append(new QStandardItem(QObject::tr("(DI-0 = ON)")));
    item.append(new QStandardItem(QObject::tr("(DO-3 = ON)")));
    m_Model->appendRow(item);
    item.clear();

    item.append(new QStandardItem(QObject::tr("#2")));
    item.append(new QStandardItem(QObject::tr("Enable")));
    item.append(new QStandardItem(QObject::tr("(AI-0 > 10) AND (A2 > 5)")));
    item.append(new QStandardItem(QObject::tr("(AO-3 = 10)")));
    m_Model->appendRow(item);
    item.clear();

    item.append(new QStandardItem(QObject::tr("#3")));
    item.append(new QStandardItem(QObject::tr("Enable")));
    item.append(new QStandardItem(QObject::tr("(AI-0 > 10) OR (DI = ON) OR (D2 = OFF)")));
    item.append(new QStandardItem(QObject::tr("(Reset)")));
    m_Model->appendRow(item);
    item.clear();

    item.append(new QStandardItem(QObject::tr("#4")));
    item.append(new QStandardItem(QObject::tr("Disable")));
    item.append(new QStandardItem(QObject::tr("")));
    item.append(new QStandardItem(QObject::tr("")));
    m_Model->appendRow(item);
    item.clear();

    item.append(new QStandardItem(QObject::tr("#5")));
    item.append(new QStandardItem(QObject::tr("Disable")));
    item.append(new QStandardItem(QObject::tr("")));
    item.append(new QStandardItem(QObject::tr("")));
    m_Model->appendRow(item);
    item.clear();

    item.append(new QStandardItem(QObject::tr("#6")));
    item.append(new QStandardItem(QObject::tr("Disable")));
    item.append(new QStandardItem(QObject::tr("")));
    item.append(new QStandardItem(QObject::tr("")));
    m_Model->appendRow(item);
    item.clear();

    item.append(new QStandardItem(QObject::tr("#7")));
    item.append(new QStandardItem(QObject::tr("Disable")));
    item.append(new QStandardItem(QObject::tr("")));
    item.append(new QStandardItem(QObject::tr("")));
    m_Model->appendRow(item);
    item.clear();

    item.append(new QStandardItem(QObject::tr("#8")));
    item.append(new QStandardItem(QObject::tr("Disable")));
    item.append(new QStandardItem(QObject::tr("")));
    item.append(new QStandardItem(QObject::tr("")));
    m_Model->appendRow(item);
    item.clear();

    item.append(new QStandardItem(QObject::tr("#9")));
    item.append(new QStandardItem(QObject::tr("Disable")));
    item.append(new QStandardItem(QObject::tr("")));
    item.append(new QStandardItem(QObject::tr("")));
    m_Model->appendRow(item);
    item.clear();

    item.append(new QStandardItem(QObject::tr("#10")));
    item.append(new QStandardItem(QObject::tr("Disable")));
    item.append(new QStandardItem(QObject::tr("")));
    item.append(new QStandardItem(QObject::tr("")));
    m_Model->appendRow(item);
    item.clear();

    ui->logicalRuleTableView->setModel(m_Model);
    ui->logicalRuleTableView->setColumnWidth(0, 50);
    ui->logicalRuleTableView->setColumnWidth(1, 70);
    ui->logicalRuleTableView->setColumnWidth(2, 450);
    ui->logicalRuleTableView->setColumnWidth(3, 450);
    ui->logicalRuleTableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->logicalRuleTableView->setRowHeight(0, 35);
    ui->logicalRuleTableView->setRowHeight(1, 35);
    ui->logicalRuleTableView->setRowHeight(2, 35);
    ui->logicalRuleTableView->setRowHeight(3, 35);
    ui->logicalRuleTableView->setRowHeight(4, 35);
    ui->logicalRuleTableView->setRowHeight(5, 35);
    ui->logicalRuleTableView->setRowHeight(6, 35);
    ui->logicalRuleTableView->setRowHeight(7, 35);
    ui->logicalRuleTableView->setRowHeight(8, 35);
    ui->logicalRuleTableView->setRowHeight(9, 35);
    ui->logicalRuleTableView->verticalHeader()->hide();
    // ui->logicalRuleTableView->setContextMenuPolicy(Qt::CustomContextMenu);
}

void MainWindow::findComPort()
{
    ui->portComboBox->clear();

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
       ui->portComboBox->addItem(info.portName());
}

void MainWindow::on_connectType_currentIndexChanged(int index)
{
    if (modbusDevice) {
        modbusDevice->disconnectDevice();
        delete modbusDevice;
        modbusDevice = nullptr;
    }

    auto type = static_cast<ModbusConnection> (index);
    if (type == Serial) {
        //ui->portEdit->clear();
        modbusDevice = new QModbusRtuSerialMaster(this);
    } else if (type == Tcp) {
        modbusDevice = new QModbusTcpClient(this);
        //if (ui->portEdit->text().isEmpty())
            //ui->portEdit->setText(QLatin1Literal("127.0.0.1:502"));
    }

    connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
        statusBar()->showMessage(modbusDevice->errorString(), 5000);
    });

    if (!modbusDevice) {
        ui->connectButton->setDisabled(true);
        if (type == Serial)
            statusBar()->showMessage(tr("Could not create Modbus master."), 5000);
        else
            statusBar()->showMessage(tr("Could not create Modbus client."), 5000);
    } else {
        connect(modbusDevice, &QModbusClient::stateChanged,
                this, &MainWindow::onStateChanged);
    }
}

void MainWindow::modelNameReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

        if (reply->error() == QModbusDevice::NoError) {
            const QModbusDataUnit unit = reply->result();
            QString s;
            for (uint i = 0; i < unit.valueCount(); i++) {
                s[2*i] = unit.value(i) >> 8;
                s[(2*i) +1] = unit.value(i) & 0x00ff;
            }
            ui->portEdit_3->setText(s);
            statusBar()->showMessage(tr("OK!"));
        } else if (reply->error() == QModbusDevice::ProtocolError) {
            statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                        arg(reply->errorString()).
                                        arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
        } else {
           // statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
           //                             arg(reply->errorString()).
           //                             arg(reply->error(), -1, 16), 5000);
            statusBar()->showMessage(tr("Unknown Device: Please input the correct type of IOT Devices."));
            ui->portEdit_3->setText("Unknown Device");
        }
        reply->deleteLater();
}

void MainWindow::setModelName() const
{
    ui->portEdit_3->text().clear();
    quint16 ADDR = ModelNameAddr;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ADDR, ModelNameEntires);
    //modbusDevice->setNumberOfRetries(1);
    if (auto *reply = modbusDevice->sendReadRequest(readUnit, ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::modelNameReadReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::setIOChannel()
{
    do {
        _sleep(10);
    }while(ui->portEdit_3->text() == "");
    QString modelName =  ui->portEdit_3->text();

    ui->currentInputComboBox->clear();
    ui->currentOutputComboBox->clear();
    ui->voltageInputComboBox->clear();
    ui->voltageOutputcomboBox->clear();
    ui->PWMInputComboBox->clear();
    ui->PWMOutputComboBox->clear();
    ui->thermocoupleComboBox->clear();

    if (modelName.contains("LC144")) {
        ui->currentInputComboBox->addItem("1");
        ui->currentInputComboBox->addItem("3");

        ui->currentOutputComboBox->addItem("4");

        ui->voltageInputComboBox->addItem("0");
        ui->voltageInputComboBox->addItem("2");

        ui->voltageOutputcomboBox->addItem("5");

        ui->PWMOutputComboBox->addItem("6");
        ui->PWMOutputComboBox->addItem("7");
    }
    else if (modelName.contains("LR")){
        ui->currentInputComboBox->addItem("1");
        ui->currentInputComboBox->addItem("3");

        ui->voltageInputComboBox->addItem("0");
        ui->voltageInputComboBox->addItem("2");
    }
    else if (modelName.contains("Un")){
        // no channel IO
    }
}

void MainWindow::setWidgetLoRa()
{
    QString s = ui->portEdit_3->text();

    if (s.contains("LM")) {
        ui->tabWidget->setTabEnabled(3, false);
        ui->tabWidget->setTabEnabled(4, false);
        ui->tabWidget->setTabEnabled(7, false);
        ui->netSIDRead->setEnabled(false);
        ui->netSIDWrite->setEnabled(false);
    }else if (s.contains("LC")) {
        ui->tabWidget->setTabEnabled(3, false);
        ui->tabWidget->setTabEnabled(7, false);
        ui->netSIDRead->setEnabled(true);
        ui->netSIDWrite->setEnabled(true);
    }else if (s.contains("LR")) {
        ui->tabWidget->setTabEnabled(1, false);
        ui->tabWidget->setTabEnabled(2, false);
        // ui->tabWidget->setTabEnabled(4, false);

        ui->label_8->setVisible(false);
        ui->portEdit_2->setVisible(false);
        ui->label_71->setVisible(false);
    }

    ui->tabWidget->setStyleSheet("QTabBar::tab:disabled {width: 0; color: transparent;}");
    update();
}

void MainWindow::setWidget()
{
    QString s = ui->portEdit_3->text();

    ui->tabWidget->setEnabled(false);
    ui->tabWidget_2->setEnabled(true);

    if (s.contains("485")) {
        // ui->tabWidget_2->setTabEnabled(2, false);
        ui->tabWidget_2->setTabEnabled(4, false);
        ui->tabWidget_2->setTabEnabled(5, false);
        ui->tabWidget_2->setTabEnabled(6, false);

        ui->nbEnableRadioButton->setVisible(false);
        /*
            // ui->nbModelLabel->setVisible(false);
            // ui->nbmodelCombox->setVisible(false);
            // ui->label_124->setVisible(false);
            // ui->ipLineEdit->setVisible(false);
            // ui->label_125->setVisible(false);
            // ui->nbStatusLineEdit->setVisible(false);
        */

        ui->plmnLineEdit->hide();
        ui->plmnlabel->hide();
        ui->tabWidget_2->setTabText(0, "RS485");
    } else if (s.contains("MB-NB")) {
        ui->tabWidget_2->setTabEnabled(7, false);
        ui->tabWidget_2->setTabEnabled(8, false);
        ui->tabWidget_2->setTabEnabled(9, false);
        ui->tabWidget_2->setTabEnabled(10, false);
        ui->tabWidget_2->setTabEnabled(11, false);
        ui->tabWidget_2->setTabText(0, "MBus");
    }

    ui->tabWidget_2->setStyleSheet("QTabBar::tab:disabled {width: 0; color: transparent;}");
    update();
}

void MainWindow::on_connectButton_clicked()
{
    if (!modbusDevice)
        return;

    statusBar()->clearMessage();

    if (modbusDevice->state() == QModbusDevice::ConnectedState) {
            modbusDevice->disconnectDevice();
            ui->actionConnect->setEnabled(true);
            ui->actionDisconnect->setEnabled(false);         
            ui->label_8->setVisible(true);
            ui->portEdit_2->setVisible(true);
            ui->label_71->setVisible(true);

            QString s = ui->portEdit_3->text();

            if (s.contains("485")) {
                // ui->tabWidget_2->setTabEnabled(2, true);
                ui->tabWidget_2->setTabEnabled(4, true);
                ui->tabWidget_2->setTabEnabled(5, true);
                ui->tabWidget_2->setTabEnabled(6, true);

                ui->nbEnableRadioButton->setVisible(true);

                ui->nbModelLabel->setVisible(true);
                ui->nbmodelCombox->setVisible(true);

                // ui->label_124->setVisible(true);
                ui->ipLineEdit->setVisible(true);

                // ui->label_125->setVisible(true);
                ui->nbStatusLineEdit->setVisible(true);
            } else if (s.contains("MB-NB")) {
                ui->tabWidget_2->setTabEnabled(7, true);
                ui->tabWidget_2->setTabEnabled(8, true);
                ui->tabWidget_2->setTabEnabled(9, true);
                ui->tabWidget_2->setTabEnabled(10, true);
                ui->tabWidget_2->setTabEnabled(11, true);
            } else if (s.contains("DL")) {
                // ui->tabWidget_2->setTabEnabled(4, true);
                // ui->tabWidget_2->setTabEnabled(5, true);
                // ui->tabWidget_2->setTabEnabled(6, true);
            }

            ui->tabWidget->setStyleSheet("QTabBar::tab:disabled {width: 0; color: transparent;}");
            ui->tabWidget_2->setStyleSheet("QTabBar::tab:disabled {width: 0; color: transparent;}");
            ui->portEdit_3->clear();
            update(); // repaint

            m_login_flag = 0;
            m_login_flag_2 = 0;
            sensorFlag = false;
     } else {
#if 0
        logindialog log(this);
        log.show();


        loginRuleDialog ruleDialog(this);
        ruleDialog.show();
        // init
        m_login_flag_2 = 0;

        while(!m_login_flag) {
            // quit button
            if (m_login_flag_2)
                return;
            _sleep(1500);
        }
#endif
        if (!modbusDevice)
            modbusDevice = new QModbusRtuSerialMaster();

        modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, ui->portComboBox->currentText());
        modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,QSerialPort::Baud9600);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,QSerialPort::Data8);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,QSerialPort::NoParity);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,QSerialPort::OneStop);
        modbusDevice->setNumberOfRetries(0);
        modbusDevice->connectDevice();

        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);

        setModelName();
        setIOChannel();

        #ifdef LORA
            setWidgetLoRa();
        #else
            setWidget();
        #endif
    }
}

void MainWindow::onStateChanged(int state)
{
    bool connected = (state != QModbusDevice::UnconnectedState);
    connected = connected;
    ui->actionConnect->setEnabled(!connected);
    ui->actionDisconnect->setEnabled(connected);

    if (state == QModbusDevice::UnconnectedState)
        ui->connectButton->setText(tr("Connect"));
    else if (state == QModbusDevice::ConnectedState)
        ui->connectButton->setText(tr("Disconnect"));
}

/*
    IO Set Code Configuration supposed to be Product Utility Tool
    Remain Since already done
*/
void MainWindow::writeSingleHoldingRegister(QModbusDataUnit &writeUnit_)
{
    if (auto *reply = modbusDevice->sendWriteRequest(writeUnit_, ui->serverEdit->value())) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                if (reply->error() == QModbusDevice::ProtocolError) {
                    statusBar()->showMessage(tr("Write response error: %1 (Mobus exception: 0x%2)")
                        .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16),
                        5000);
                } else if (reply->error() != QModbusDevice::NoError) {
                    statusBar()->showMessage(tr("Write response error: %1 (code: 0x%2)").
                        arg(reply->errorString()).arg(reply->error(), -1, 16), 5000);
                }
                statusBar()->showMessage(tr("OK!"));
                reply->deleteLater();
            });
        } else {
            // broadcast replies return immediatel
            reply->deleteLater();
        }
    } else {
        statusBar()->showMessage(tr("Write error: ") + modbusDevice->errorString(), 5000);
    }
}

void MainWindow::_sleep(unsigned int msec)
{
    QTime reachTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < reachTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void MainWindow::defaultTheme()
{
    QFile qss(":stylesheet.qss");
    if( qss.open(QFile::ReadOnly)) {
           QString style = QLatin1String(qss.readAll());
           this->setStyleSheet(style);
           qss.close();
    }
    ui->tabWidget->setTabEnabled(6, false);
    ui->tabWidget->setStyleSheet("QTabBar::tab:disabled {width: 0; color: transparent;}");
}

void MainWindow::darkTheme()
{
    QFile qss(":stylesheetblack.qss");
    if( qss.open(QFile::ReadOnly)) {
           QString style = QLatin1String(qss.readAll());
           this->setStyleSheet(style);
           qss.close();
    }
    ui->tabWidget->setTabEnabled(6, true);
    ui->tabWidget->setStyleSheet("QTabBar::tab:disabled {width: 0; color: transparent;}");
}

void MainWindow::on_netBitMapClear_clicked()
{
    m_pModel->clearDate();
}

void MainWindow::serialAlarmInit()
{
    serialAlarm = new QTimer();
    serialAlarm->stop();
    serialAlarm->setInterval(1000);
    connect(serialAlarm, SIGNAL(timeout()), this, SLOT(serialAlarmTask()));
    serialAlarm->start();
}

void MainWindow::serialAlarmTask()
{
    QVector<QString> tmp;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        tmp.push_back(info.portName());
    }
    if (tmp != serialInfoVector) {
        serialInfoVector.clear();
        ui->portComboBox->clear();
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
            serialInfoVector.push_back(info.portName());
            ui->portComboBox->addItem(info.portName());
        }
    }
}

void MainWindow::ymodemCancelButtonCliked()
{
    m_ymodem->receiveStatus = waitFirstCRC;

    if(modbusDevice->state() == QModbusDevice::UnconnectedState) {
        modbusDevice->connectDevice();
        m_ymodem->port->close();
        m_ymodem->dwnFlagRdy = false;
    }

    if (m_ymodem->port->isOpen()) {
        m_ymodem->port->close();
        m_ymodem->dwnFlagRdy = false;
    }

    modbusDevice->connectDevice();

    ui->eventLogEnableRadioButton->setChecked(false);
    on_eventLogEnableRadioButton_clicked();
}

void MainWindow::meterViewModelInit()
{
    m_meterViewControl = new meterModelViewControl(this);

    ui->meterTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->meterTableView->setShowGrid(true);
    ui->meterTableView->setFrameShape(QFrame::Box);
    ui->meterTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->meterTableView->setModel(m_meterViewControl);
    ui->meterTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->meterTableView->setColumnWidth(0, 160);
    ui->meterTableView->setColumnWidth(1, 130);
    ui->meterTableView->setColumnWidth(2, 130);
}

void MainWindow::meterPollViewModelInit()
{
    m_meterPollModelBody = new meterPollTableModel(this);

    ui->meterPollBodyTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->meterPollBodyTableView->setShowGrid(true);
    ui->meterPollBodyTableView->setFrameShape(QFrame::Box);
    ui->meterPollBodyTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->meterPollBodyTableView->setModel(m_meterPollModelBody);
    ui->meterPollBodyTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->meterPollBodyTableView->setColumnWidth(0, 160);
    ui->meterPollBodyTableView->setColumnWidth(1, 160);
    ui->meterPollBodyTableView->setColumnWidth(2, 130);
}
