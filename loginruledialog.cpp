#include "loginruledialog.h"
#include "ui_loginruledialog.h"

loginRuleDialog::loginRuleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loginRuleDialog)
{
    ui->setupUi(this);
}

loginRuleDialog::~loginRuleDialog()
{
    delete ui;
}
