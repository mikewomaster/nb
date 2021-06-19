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

#include <QApplication>
#include <QLoggingCategory>
#include <QFile>
#include <QDebug>
#include <QDialog>
#include <QMessageBox>
#include "logindialog.h"

int main(int argc, char *argv[])
{
    // TODO uncomment this line before release
    // right now we always need it
    QLoggingCategory::setFilterRules(QStringLiteral("qt.modbus* = true"));
    QApplication a(argc, argv);

#ifdef SKIN
    QFile qss(":stylesheet.qss");
    if( qss.open(QFile::ReadOnly)) {
           QString style = QLatin1String(qss.readAll());
           a.setStyleSheet(style);
           qss.close();
    }
#endif

    MainWindow *w = new MainWindow();
    w->setFixedSize(1027, 675);

    QFile qss(":stylesheet.qss");
    if(qss.open(QFile::ReadOnly)) {
           QString style = QLatin1String(qss.readAll());
           w->setStyleSheet(style);
           qss.close();
    }

#if 0
    QDateTime time = QDateTime::currentDateTime();
    int timeT = time.toTime_t();
    if (timeT <= 1624198378)
    {
        QString str = "You are using testing Version " + QString::number(1621593042 - timeT) + " seconds remained";
        // QMessageBox::information(NULL, "INFO", str, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
    else
    {
        QMessageBox::information(NULL, "INFO",  "You are using testing Version. License Expired!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return -1;
    }
#endif

#ifdef LOGIN
    logindialog log(w);
    log.show();
    if (log.exec() == QDialog::Accepted){
        w->show();
        w->on_connectButton_clicked();
        return a.exec();
    }else {
        return 0;
    }
#else
    w->show();
    return a.exec();
#endif

}
