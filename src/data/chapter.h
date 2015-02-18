#ifndef CHAPTER_H
#define CHAPTER_H

#include <QList>
#include "cleveruri.h"
#include "ressourcesnode.h"

class Chapter : public RessourcesNode
{
public:
    Chapter();
    Chapter(const Chapter& m);
    ~Chapter();
    virtual const QString& getShortName() const;
    virtual void setShortName(QString& name);
    virtual bool hasChildren() const;
    void addResource(CleverURI* );
    Chapter* addChapter(QString& name);

    QList<CleverURI>& getResourceList();
    QList<Chapter>& getChapterList();
private:
    QList<CleverURI> m_ressoucelist;
    QList<Chapter> m_chapterlist;
    QString m_name;


    friend QDataStream& operator<<(QDataStream& os,const Chapter&);
    friend QDataStream& operator>>(QDataStream& is,Chapter&);
};
typedef QList<Chapter> ChapterList;
Q_DECLARE_METATYPE(Chapter)
Q_DECLARE_METATYPE(ChapterList)
#endif // CHAPTER_H
