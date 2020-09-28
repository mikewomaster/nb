#ifndef OBIS_EDIT_H
#define OBIS_EDIT_H

#include <QDialog>

namespace Ui {
class obis_edit;
}

class obis_edit : public QDialog
{
    Q_OBJECT

public:
    explicit obis_edit(QWidget *parent = 0);
    ~obis_edit();
    int id;
private slots:
    void on_obisAdd_clicked();

private:
    Ui::obis_edit *ui;
};

#endif // OBIS_EDIT_H
