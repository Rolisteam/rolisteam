#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include <QObject>
#include <QString>
#include <charactersheet/charactersheet_global.h>

#include "fieldcontroller.h"

class CellData : public TreeSheetItem
{
    Q_OBJECT
    Q_PROPERTY(QString value READ value WRITE setValue NOTIFY valueChanged FINAL)
    Q_PROPERTY(QString formula READ formula WRITE setFormula NOTIFY formulaChanged FINAL)
public:
    CellData(QObject* parent= nullptr);

    QString value() const;
    void setValue(const QString& newValue);

    QString formula() const;
    void setFormula(const QString& newFormula) override;

    void setFieldInDictionnary(QHash<QString, QString>& dict) const override;
    void setOrig(TreeSheetItem* origine) override;
    QVariant valueFrom(TreeSheetItem::ColumnId col, int role) const override;
    void setValueFrom(TreeSheetItem::ColumnId col, const QVariant& data) override;

signals:
    void valueChanged();
    void formulaChanged();

private:
    QString m_value;
    QString m_formula;
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
    TableModel(TreeSheetItem* parent);

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
    CellData* cellData(int line, int col) const;
    CellData* cellDataFromId(const QString& id) const;
    FieldController* colField(int col) const;
    FieldController* colFieldFromId(const QString& id) const;
    QModelIndex indexFromCell(TreeSheetItem* data) const;

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
    CellData* addCellData();

private:
    QList<FieldController*> m_columns;
    QList<QList<CellData*>> m_data;
    QPointer<TreeSheetItem> m_parent;
    int m_count{0};
};

#endif // TABLEMODEL_H
