#ifndef CHAPTER_H
#define CHAPTER_H

#include <QList>
#include "cleveruri.h"

class Chapter
{
public:
    Chapter();

private:
    QList<CleverURI> m_ressoucelist;
};

#endif // CHAPTER_H
