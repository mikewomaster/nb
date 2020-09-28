#include "logdialog.h"
#include "ui_logdialog.h"

logdialog::logdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::logdialog)
{
    ui->setupUi(this);
    Qt::WindowFlags flags=Qt::Dialog;
    flags |=Qt::WindowCloseButtonHint;
    setWindowFlags(flags);
}

logdialog::~logdialog()
{
    delete ui;
}
