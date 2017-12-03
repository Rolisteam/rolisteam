/***************************************************************************
    *	Copyright (C) 2009 by Renaud Guezennec                             *
    *   http://www.rolisteam.org/contact                   *
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
#include <QList>
#include <QMetaType>
#include "resourcesnode.h"

class CleverURIListener;
/**
* @brief This class stores data refering to file, uri and type
* @brief as a file can be loaded as different type.
* @brief E.g: an image can be loaded as Picture or as Background for map.
*
*/
class CleverURI : public ResourcesNode
{

public:
    /**
    * @brief enum of all available type.
    *
    */
    enum ContentType {NONE,MAP,VMAP,CHAT,PICTURE,ONLINEPICTURE,TEXT,CHARACTERSHEET,SCENARIO,SONG,SONGLIST,SHAREDNOTE
              #ifdef WITH_PDF
                      ,PDF
              #endif
                     };

    enum LoadingMode {Internal,Linked};
    enum State {Remain,Opened,Displayed};
    enum DataValue {NAME,MODE,DISPLAYED,URI};
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
    CleverURI(QString name, QString uri,ContentType type);
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
    /**
     * @brief operator ==
     * @param uri1
     * @return
     */
    bool operator==(const CleverURI& uri1) const;

    /**
    * @brief overriden method from RessourceNode
    * @return always false
    */
    bool hasChildren() const;
    /**
    * @brief static method which returns the appropriate icon path given the type
    * @param type of the content
    * @return the path to the icon
    */
    virtual QIcon getIcon();
    /**
     * @brief CleverURI::getAbsolueDir
     * @return
     */
    const QString getAbsolueDir() const;

    virtual void setName(const QString &name);

    void write(QDataStream& out,bool tag = true) const;
    void read(QDataStream &in);

    /**
     * @brief getFilterForType must return the filter for any kind of content but it also return empty string for content with dedicated loading dialog
     * @return
     */
    static QString getFilterForType(CleverURI::ContentType);
    static QString typeToString(CleverURI::ContentType);
    static QString getPreferenceDirectoryKey(CleverURI::ContentType);

    LoadingMode getCurrentMode() const;
    void setCurrentMode(const LoadingMode &currentMode);

    bool isDisplayed() const;
    void setDisplayed(bool displayed);

    QByteArray getData() const;
    void setData(const QByteArray& data);

    void loadFileFromUri(QByteArray&) const;
    void clearData();

    QVariant getData(ResourcesNode::DataValue i);
    bool seekNode(QList<ResourcesNode*>& path,ResourcesNode* node);

    //static CleverURIListener *getListener();
    void setListener(CleverURIListener *value);

    CleverURI::State getState() const;
    void setState(const State &state);

    bool hasData()const;

    void updateListener(CleverURI::DataValue value);

    ResourcesNode::TypeResource getResourcesType() const;
private:
    /**
    * @brief split the uri to get the shortname
    */
    void setUpListener();
    void init();
    QString m_uri; ///< member to store the uri
    CleverURI::ContentType m_type;///< member to store the content type
    QByteArray m_data;///< data from the file
    LoadingMode m_currentMode;
    State m_state;


    static QHash<CleverURI::ContentType,QString> m_iconPathHash;
    static QStringList m_typeNameList;
    static QStringList m_typeToPreferenceDirectory;
    QList<CleverURIListener*> m_listenerList;
};
/**
 * @brief The CleverURIListener class is an abstract track to be notify when CleverURI has changed.
 */
class CleverURIListener
{
public:
    virtual void cleverURIHasChanged(CleverURI*,CleverURI::DataValue)=0;
};

typedef QList<CleverURI> CleverUriList;
Q_DECLARE_METATYPE(CleverURI)
Q_DECLARE_METATYPE(CleverUriList)
QDataStream &operator<<(QDataStream &out,const CleverURI&); ///< operator for serialisation (writing)
QDataStream &operator>>(QDataStream &in,CleverURI&); ///< operator for serialisation (reading)
QDataStream &operator<<(QDataStream &out,const CleverUriList &myObj); ///< operator for serialisation (writing)
QDataStream &operator>>(QDataStream &in,CleverUriList &myObj); ///< operator for serialisation (reading)
#endif // CLEVERURI_H


