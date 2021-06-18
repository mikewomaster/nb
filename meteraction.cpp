#include "meteraction.h"
#include "ui_meteraction.h"

meterAction::meterAction(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::meterAction)
{
    ui->setupUi(this);
    setWindowTitle("Meter Edit");
}

meterAction::~meterAction()
{
    delete ui;
}

void meterAction::on_meterEditPushButton_clicked()
{

}

void meterAction::on_meterCancelPushButton_clicked()
{
    close();
    delete ui;
}
