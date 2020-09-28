#ifndef STUDENT_INFORMATION_H
#define STUDENT_INFORMATION_H
#include <QVector>
#include <QMap>
#include <QAbstractTableModel>

class student_information:public QAbstractTableModel
{
public:
    student_information(const int totalColumn, const int aColumnNumWithChechBox = 0, QObject *parent = 0)
        :totalColumn(totalColumn),colNumberWithCheckBox(aColumnNumWithChechBox),QAbstractTableModel(parent)
        {
            rowCheckStateMap.clear();
        }

public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    void AddStudentInfo(const student_information &student_information);
signals:
    void StudentInfoIsChecked(const StudentInfo &studentInfo);
private:
    typedef QVector<student_information> StudentInfos;
    StudentInfos studentInfos;
    int totalColumn;
    int colNumberWithCheckBox;
    QMap<int, Qt::CheckState> rowCheckStateMap;
};

#endif // STUDENT_INFORMATION_H
