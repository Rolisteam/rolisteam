#ifndef TREEITEM_H
#define TREEITEM_H

#include <QString>
/**
 * @brief The TreeItem class
 */
class TreeItem
{
public:
    TreeItem();

    virtual void addChild();
    virtual bool isLeaf() const;
    virtual int childCount() const;
    virtual int addChild(TreeItem*);
    TreeItem* getChildAt(int row);

    TreeItem* getParent() const;
    void setParent(TreeItem* parent);

    QString getName() const;
    void setName(const QString &name);

    virtual int indexOf(TreeItem*) = 0;
    int rowInParent();

protected:
    QString m_name;
    TreeItem* m_parent;
};

#endif // TREEITEM_H
