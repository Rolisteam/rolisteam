/***************************************************************************
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   https://rolisteam.org/contact                   *
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

#include <QList>
#include <QMetaType>
#include <QString>

#include "media/mediatype.h"
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
     * @brief default constructor
     *
     */
    CleverURI(Core::ContentType type);
    /**
     * @brief constructor with parameter
     *
     */
    CleverURI(QString name, QString uri, const QString& ownerId, Core::ContentType type);
    /**
     * @brief Destructor
     *
     */
    virtual ~CleverURI() override;

    /**
     * @brief set the URI parameter
     * @param new uri
     */
    void setUri(QString& uri);
    /**
     * @brief set the type
     * @param the new type
     */
    void setContentType(Core::ContentType type);
    /**
     * @brief accessor to the URI
     * @return the URI
     */
    const QString getUri() const;
    /**
     * @brief accessor to the type
     * @return the type
     */
    Core::ContentType contentType() const;
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
    bool hasChildren() const override;
    /**
     * @brief static method which returns the appropriate icon path given the type
     * @param type of the content
     * @return the path to the icon
     */
    virtual QIcon getIcon() const override;
    /**
     * @brief CleverURI::getAbsolueDir
     * @return
     */
    const QString getAbsolueDir() const;

    QString ownerId() const;

    void write(QDataStream& out, bool tag= true, bool saveData= true) const override;
    void read(QDataStream& in) override;

    /**
     * @brief getFilterForType must return the filter for any kind of content but it also return empty string for
     * content with dedicated loading dialog
     * @return
     */
    static QString getFilterForType(Core::ContentType);
    static QString typeToIconPath(Core::ContentType);
    static QString typeToString(Core::ContentType);
    static QString getPreferenceDirectoryKey(Core::ContentType);

    Core::LoadingMode loadingMode() const;
    void setLoadingMode(const Core::LoadingMode& currentMode);

    bool isDisplayed() const;
    void setDisplayed(bool displayed);

    QByteArray getData() const;
    void setData(const QByteArray& data);

    void loadFileFromUri(QByteArray&) const;
    void clearData();

    QVariant getData(ResourcesNode::DataValue i) const override;
    bool seekNode(QList<ResourcesNode*>& path, ResourcesNode* node) override;

    Core::State getState() const;
    void setState(const Core::State& state);

    bool hasData() const;
    bool exists();

protected:
    void loadData();

private:
    void init();
    QString m_uri;            ///< member to store the uri
    Core::ContentType m_type; ///< member to store the content type
    QByteArray m_data;        ///< data from the file
    Core::LoadingMode m_loadingMode;
    Core::State m_state;
    QString m_ownerId;

    static QStringList m_typeNameList;
    static QStringList m_typeToPreferenceDirectory;
};

typedef QList<CleverURI> CleverUriList;
#endif // CLEVERURI_H
