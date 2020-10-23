#ifndef SENSOR_EDIT_H
#define SENSOR_EDIT_H

#include <QDialog>

namespace Ui {
class sensor_edit;
}

class sensor_edit : public QDialog
{
    Q_OBJECT

public:
    explicit sensor_edit(QWidget *parent = 0);
    ~sensor_edit();
    void updateDateFromMainWindow();
    int seq;
private slots:
    void on_sensorAddPushButton_clicked();

private:
    Ui::sensor_edit *ui;
};

#endif // SENSOR_EDIT_H
