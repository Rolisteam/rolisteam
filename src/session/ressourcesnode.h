#ifndef RESSOURCESNODE_H
#define RESSOURCESNODE_H
#include <QString>

class RessourcesNode
{
public:
    virtual const QString& getShortName() const=0;
    virtual void setShortName(QString& name)=0;
    virtual bool hasChildren() const =0;
//    virtual void addChild(RessourcesNode*) =0;
};

#endif // RESSOURCESNODE_H
