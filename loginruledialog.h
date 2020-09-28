#ifndef LOGINRULEDIALOG_H
#define LOGINRULEDIALOG_H

#include <QDialog>

namespace Ui {
class loginRuleDialog;
}

class loginRuleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit loginRuleDialog(QWidget *parent = 0);
    ~loginRuleDialog();

private:
    Ui::loginRuleDialog *ui;
};

#endif // LOGINRULEDIALOG_H
