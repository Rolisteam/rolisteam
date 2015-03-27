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

/**
    * @brief This class stores data refering to file, uri and type
    * @brief as a file can be loaded as different type.
    * @brief E.g: an image can be loaded as Picture or as Background for map.
    *
    */

class CleverURI : public RessourcesNode
{

public:
    /**
    * @brief enum of all available type.
    *
    */
    enum ContentType {MAP,TCHAT,PICTURE,TEXT,CHARACTERSHEET,SCENARIO,SONG
              #ifdef WITH_PDF
                      ,PDF
              #endif
                     };
    /**
    * @brief default constructor
    *
    */
    CleverURI();
    /**
    * @brief copy constructor
    *
    */
    CleverURI(const CleverURI & mp);
    /**
    * @brief constructor with parameter
    *
    */
    CleverURI(QString uri,ContentType type);
    /**
    * @brief Destructor
    *
    */
    virtual ~CleverURI();

    /**
    * @brief set the URI parameter
    * @param new uri
    */
    void setUri(QString& uri);
    /**
    * @brief set the type
    * @param the new type
    */
    void setType(CleverURI::ContentType type);
    /**
    * @brief accessor to the URI
    * @return the URI
    */
    const QString getUri() const;
    /**
    * @brief accessor to the type
    * @return the type
    */
    CleverURI::ContentType getType() const;
    bool operator==(const CleverURI& uri1) const;

    /**
    * @brief overriden method from RessourceNode
    * @return always false
    */
    bool hasChildren() const;
    /**
    * @brief accessor to a shorter uri (only the filename)
    * @return the filename
    */
    const QString& getShortName() const;
    /**
    * @brief change the short name
    * @param new short name
    */
    virtual void setShortName(QString& name);
    /**
    * @brief static method which returns the appropriate icon path given the type
    * @param type of the content
    * @return the path to the icon
    */
    static QString& getIcon(ContentType type);

private:
    /**
    * @brief split the uri to get the shortname
    *
    */
    void defineShortName();




    QString m_uri; ///< member to store the uri
    CleverURI::ContentType m_type;///< member to store the content type
    QString m_shortname; ///< member to store the shortname

    static QString m_musicIcon; ///< static member to store the music icon
    static QString m_textIcon;///< static member to store the text icon
    static QString m_mapIcon; ///< static member to store the map icon
    static QString m_pictureIcon; ///< static member to store the picture icon
    static QString m_charactersheetIcon; ///< static member to store the character icon
    static QString m_scenarioIcon; ///< static member to store the scenario icon
    static QString m_chatIcon; ///< static member to store the chat icon
#ifdef WITH_PDF
    static QString m_pdfIcon;///< static member to store the pdf icon
#endif
    static QString m_empty;

    friend QDataStream& operator<<(QDataStream& os,const CleverURI&); ///< operator for serialisation (writing)
    friend QDataStream& operator>>(QDataStream& is,CleverURI&); ///< operator for serialisation (reading)
};
typedef QList<CleverURI> CleverUriList;
Q_DECLARE_METATYPE(CleverURI)
Q_DECLARE_METATYPE(CleverUriList)

#endif // CLEVERURI_H
