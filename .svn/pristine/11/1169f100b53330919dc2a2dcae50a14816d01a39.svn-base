#ifndef LOGICALRULEMODEL_H
#define LOGICALRULEMODEL_H

#include <QObject>
#include <QAbstractTableModel>

typedef struct logicalRule{
    int ruleId;
    bool status;
    QString Conditions;
    QString Actions;
} logicalRule;

class logicalRuleModel: public QAbstractTableModel
{
    public:
        logicalRuleModel(QObject *parent = nullptr);
        void updateData(QList<logicalRule> logicalRuleList);
        void clearDate();
        int rowCount(const QModelIndex &parent) const;
        int columnCount(const QModelIndex &parent) const;
        bool setData(const QModelIndex &index, const QVariant &value, int role);
        QVariant data(const QModelIndex &index, int role) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
    private:
        QList<logicalRule> m_logicalRuleList;
};

#endif // LOGICALRULEMODEL_H
