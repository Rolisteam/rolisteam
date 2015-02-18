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
#ifndef CLEVERURI_H
#define CLEVERURI_H

#include <QString>
#include <QMetaType>
#include "ressourcesnode.h"

class CleverURI : public RessourcesNode
{

public:
    enum ContentType {MAP,TCHAT,PICTURE,TEXT,CHARACTERSHEET,SCENARIO,SONG};
    CleverURI();
    CleverURI(const CleverURI & mp);
    CleverURI(QString uri,ContentType type);
    ~CleverURI();

    void setUri(QString& uri);
    void setType(int type);

    const QString getUri() const;
    int getType() const;
    bool operator==(const CleverURI& uri1) const;

    bool hasChildren() const;
    const QString& getShortName() const;
    virtual void setShortName(QString& name);

    static QString& getIcon(ContentType type);

private:
    void defineShortName();




    QString m_uri;
    int m_type;
    QString m_shortname;

    static QString m_musicIcon;
    static QString m_textIcon;
    static QString m_mapIcon;
    static QString m_pictureIcon;
    static QString m_charactersheetIcon;
    static QString m_scenarioIcon;
    static QString m_tchatIcon;

    friend QDataStream& operator<<(QDataStream& os,const CleverURI&);
    friend QDataStream& operator>>(QDataStream& is,CleverURI&);
};
typedef QList<CleverURI> CleverUriList;
Q_DECLARE_METATYPE(CleverURI)
Q_DECLARE_METATYPE(CleverUriList)

#endif // CLEVERURI_H
