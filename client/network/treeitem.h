#ifndef TREEITEM_H
#define TREEITEM_H

#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QString>
/**
 * @brief The TreeItem class
 */
class TreeItem : public QObject
{
    Q_OBJECT
public:
    TreeItem(QObject* parent= nullptr);

    virtual void addChild();
    virtual bool isLeaf() const;
    virtual int childCount() const;
    virtual int addChild(TreeItem*);
    virtual TreeItem* getChildAt(int row);

    TreeItem* getParentItem() const;
    void setParentItem(TreeItem* parent);

    QString getName() const;
    void setName(const QString& name);

    virtual int indexOf(TreeItem*)= 0;
    int rowInParent();

    QString getId() const;
    void setId(const QString& id);

    virtual bool addChildInto(QString id, TreeItem* child);

    // serialization
    virtual void readFromJson(QJsonObject& json)= 0;
    virtual void writeIntoJson(QJsonObject& json)= 0;

    virtual void clear();

    virtual void kick(QString str);

    virtual TreeItem* getChildById(QString id);
    virtual bool removeChild(TreeItem*);

signals:
    void itemChanged();

protected:
    QString m_id;
    QString m_name;
    TreeItem* m_parentItem= nullptr;
};

#endif // TREEITEM_H
