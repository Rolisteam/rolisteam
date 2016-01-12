#ifndef FIELDMODEL_H
#define FIELDMODEL_H

#include <QObject>
#include <QAbstractItemModel>

#include "field.h"
#include "item.h"
#include <QJsonObject>

/**
 * @brief The Section class
 */
class Section : public Item
{
public:
    Section();

    virtual bool hasChildren();
    virtual int getChildrenCount();
    virtual Item* getChildAt(int);
    virtual QVariant getValue(Item::ColumnId) const;
    virtual void setValue(Item::ColumnId,QVariant);
    virtual bool mayHaveChildren();
    virtual void appendChild(Item*);
    virtual int indexOfChild(Item *itm);
    QString getName() const;
    void setName(const QString &name);
    virtual void save(QJsonObject& json);

private:
    QList<Item*> m_children;
    QString m_name;
};
/**
s * @brief The Column class
 */
class Column
{

public:
    Column(QString,Item::ColumnId);
    QString getName() const;
    void setName(const QString &name);


    Item::ColumnId getPos() const;
    void setPos(const Item::ColumnId &pos);

private:
    QString m_name;
    Item::ColumnId m_pos;
};

/**
 * @brief The FieldModel class
 */
class FieldModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit FieldModel(QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    void appendField(Field* f);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    void save(QJsonObject& json);
signals:

public slots:
    void updateItem(Field *);
private:
    QList<Column*> m_colunm;
    Section* m_rootSection;

};

#endif // FIELDMODEL_H
