#ifndef ITEM_H
#define ITEM_H

#include <QVariant>
#include <QJsonObject>
#include <QTextStream>

class QGraphicsScene;
/**
 * @brief The Item class
 */
class Item
{
public:
    enum ColumnId {NAME,X,Y,WIDTH,HEIGHT,BORDER,TEXT_ALIGN,BGCOLOR,TEXTCOLOR};
    enum QMLSection {FieldSec,ConnectionSec};
    Item();
    virtual bool hasChildren();
    virtual int getChildrenCount();
    virtual Item* getChildAt(int);
    virtual QVariant getValue(Item::ColumnId) const;
    virtual void setValue(Item::ColumnId,QVariant data)=0;
    virtual bool mayHaveChildren();
    virtual void appendChild(Item*);
    Item *getParent() const;
    void setParent(Item *parent);

    int row();

    virtual int indexOfChild(Item *itm);
    virtual void save(QJsonObject& json,bool exp=false)=0;
    virtual void load(QJsonObject& json,QGraphicsScene* scene)=0;

    virtual void generateQML(QTextStream& out,Item::QMLSection sec)=0;
private:
    Item* m_parent;
};

#endif // ITEM_H
