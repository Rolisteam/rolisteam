#ifndef CHAPTER_H
#define CHAPTER_H

#include <QList>
#include "cleveruri.h"
#include "ressourcesnode.h"

class Chapter : public RessourcesNode
{
public:
    Chapter();
    virtual const QString& getShortName() const;
    virtual bool hasChildren() const;
    void setName(QString& p );
    void addResource(CleverURI* );
private:
    QList<CleverURI*> m_ressoucelist;
    QString m_name;
};

#endif // CHAPTER_H
