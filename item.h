#ifndef ITEM_H
#define ITEM_H

#include <QVariant>
class Item
{
public:
    enum ColumnId {NAME,X,Y,WIDTH,HEIGHT,BORDER,TEXT_ALIGN,BGCOLOR,TEXTCOLOR};
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
private:
    Item* m_parent;
};

#endif // ITEM_H
