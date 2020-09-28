#include "logicalrulemodel.h"
#include <QAbstractTableModel>

#define ruleColumn 0
#define statusColumn 1
#define conditionsColumn 2
#define actionsColumn 3

logicalRuleModel::logicalRuleModel(QObject *parent)
    :QAbstractTableModel(parent)
{

}

void logicalRuleModel::updateData(QList<logicalRule> logicalRuleList){}

void logicalRuleModel::clearDate(){}

int logicalRuleModel::rowCount(const QModelIndex &parent) const{
    Q_UNUSED(parent);
    return 10;
}

int logicalRuleModel::columnCount(const QModelIndex &parent) const{
    Q_UNUSED(parent);
    return 4;
}

bool logicalRuleModel::setData(const QModelIndex &index, const QVariant &value, int role){}

QVariant logicalRuleModel::data(const QModelIndex &index, int role) const{}

QVariant logicalRuleModel::headerData(int section, Qt::Orientation orientation, int role) const{
    switch (role)
    {
        case Qt::TextAlignmentRole:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);

        case Qt::DisplayRole:
        {
            if (orientation == Qt::Horizontal)
            {
                if (section == ruleColumn)
                    return QStringLiteral("Rule");
                else if (section == statusColumn)
                    return QStringLiteral("Status");
                else if (section == conditionsColumn)
                    return QStringLiteral("Conditons");
                else
                    return QStringLiteral("Actions");
            }
        }
        default:
            return QVariant();
    }

    return QVariant();
}

Qt::ItemFlags logicalRuleModel::flags(const QModelIndex &index) const{}
