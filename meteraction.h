#ifndef METERACTION_H
#define METERACTION_H

#include <QDialog>

namespace Ui {
class meterAction;
}

class meterAction : public QDialog
{
    Q_OBJECT

public:
    explicit meterAction(QWidget *parent = 0);
    ~meterAction();
    Ui::meterAction *ui;

private slots:
    void on_meterEditPushButton_clicked();
    void on_meterCancelPushButton_clicked();

private:

};

#endif // METERACTION_H
