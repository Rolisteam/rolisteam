#ifndef TREEITEM_H
#define TREEITEM_H

#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QString>

#include "network_global.h"
/**
 * @brief The TreeItem class
 */
class NETWORK_EXPORT TreeItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged)
public:
    TreeItem(QObject* parent= nullptr);

    virtual void appendChild();
    virtual bool isLeaf() const;
    virtual int childCount() const;
    virtual int addChild(TreeItem*);
    virtual TreeItem* getChildAt(int row);

    TreeItem* getParentItem() const;
    void setParentItem(TreeItem* parent);

    QString name() const;
    void setName(const QString& name);

    virtual int indexOf(TreeItem*)= 0;
    int rowInParent();

    QString uuid() const;
    void setUuid(const QString& id);

    virtual bool addChildInto(QString id, TreeItem* child);

    virtual void clear();

    virtual void kick(const QString& str, bool isAdmin, const QString& senderId);

    virtual TreeItem* getChildById(QString id);
    virtual bool removeChild(TreeItem*);

signals:
    void itemChanged();
    void nameChanged();
    void uuidChanged();

protected:
    QString m_id;
    QString m_name;
    TreeItem* m_parentItem= nullptr;
};

#endif // TREEITEM_H
