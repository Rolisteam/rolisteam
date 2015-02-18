/***************************************************************************
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   Rolisteam is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef SESSION_H
#define SESSION_H


#include <QList>
#include "chapter.h"
#include <QTime>

/**
  * @brief will store any ressources of a compagne
  *
  */
class Session
{


public:
    Session();
    Session(const Session& m);
    ~Session();

    Chapter* addChapter(QString& name);
    CleverURI* addRessource(QString& uri, CleverURI::ContentType& type, Chapter* chapter);

    Chapter* getChapter(int index);

    QList<Chapter*>& chapterList();
    QList<CleverURI*>& getUnclassedList();

    int childrenCount();
    int chapterCount();

    void removeRessourcesNode(RessourcesNode* item);

private:
    QList<Chapter*> m_chapterList;
    QList<CleverURI*> m_ressoucelist;

    QTime m_time;
    int m_gameCount;




    friend QDataStream& operator<<(QDataStream& os,const Session&);
    friend QDataStream& operator>>(QDataStream& is,Session&);
};
typedef QList<Session> SessionList;
Q_DECLARE_METATYPE(Session)
Q_DECLARE_METATYPE(SessionList)
#endif // SESSION_H
