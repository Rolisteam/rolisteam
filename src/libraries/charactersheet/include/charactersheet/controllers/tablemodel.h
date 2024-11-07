#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include <QObject>
#include <QString>
#include <charactersheet/charactersheet_global.h>

#include "fieldcontroller.h"

struct CellData
{
    QString value;
    QString formula;
};

class CHARACTERSHEET_EXPORT TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum customRole
    {
        ValueRole= Qt::UserRole + 1,
        FormulaRole,
    };
    TableModel();

    // MVC Functinos
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    int columnCount(const QModelIndex& parent= QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& data, int role) override;
    QHash<int, QByteArray> roleNames() const override;

    // API to add data
    void addRow();
    void addRows(int rCount);
    void addColumn(FieldController* field);
    void removeLine(int index);
    void removeColumn(int index);

    void clear();
    const CellData* cellData(int line, int col) const;
    const CellData* cellDataFromId(const QString& id) const;

    // serilization
    void save(QJsonObject& json) const;
    void load(const QJsonObject& json, TreeSheetItem* parent);
    void saveDataItem(QJsonArray& json);
    void loadDataItem(const QJsonArray& json, TreeSheetItem* parent);
    void setChildFieldData(const QJsonObject& json);

    const QList<FieldController*>& columns() const;

    void makeSpace(int row, int cols);

    // function
    QList<int> sumColumn() const;
    void setFieldInDictionnary(QHash<QString, QString>& dict, const QString& id, const QString& label) const;

private:
    QList<FieldController*> m_columns;
    QList<QList<CellData>> m_data;
};

#endif // TABLEMODEL_H
