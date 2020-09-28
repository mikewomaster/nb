#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QModbusClient>

#define Entries 8
#define USERNAME (181 - 1)
#define PASSWORD (189 - 1)
#define LoginADDR (200 - 1)
#define AddrEntries 16
namespace Ui {
class logindialog;
}

class logindialog : public QDialog
{
    Q_OBJECT

public:
    explicit logindialog(QWidget *parent = nullptr);
    void closeEvent(QCloseEvent *event);
    ~logindialog();

private slots:
    void on_loginPushButton_clicked();
    void on_quitPushButton_clicked();
    void ReadReady();

private:
    Ui::logindialog *ui;
    QModbusClient *modbusDeviceLogin;
    QString m_username;
    QString m_password;
};

#endif // LOGINDIALOG_H
