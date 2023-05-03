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
#include <core_global.h>
/**
 * @brief This class stores data refering to file, uri and type
 * @brief as a file can be loaded as different type.
 * @brief E.g: an image can be loaded as Picture or as Background for map.
 *
 */
class CORE_EXPORT CleverURI : public ResourcesNode
{
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(Core::ContentType contentType READ contentType WRITE setContentType NOTIFY contentTypeChanged)
    Q_PROPERTY(QByteArray data READ data WRITE setData NOTIFY dataChanged)
    Q_PROPERTY(Core::State state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(Core::LoadingMode loadingMode READ loadingMode WRITE setLoadingMode NOTIFY loadingModeChanged)

public:
    /**
     * @brief default constructor
     */
    CleverURI(Core::ContentType type);
    /**
     * @brief constructor with parameter
     *
     */
    CleverURI(const QString& name, const QString& uri, Core::ContentType type);
    /**
     * @brief Destructor
     *
     */
    virtual ~CleverURI() override;

    void setPath(const QString& uri);
    QString path() const;

    void setContentType(Core::ContentType type);
    Core::ContentType contentType() const;

    Core::LoadingMode loadingMode() const;
    void setLoadingMode(const Core::LoadingMode& currentMode);

    bool isDisplayed() const;

    Core::State state() const;
    void setState(const Core::State& state);

    QByteArray data() const;
    bool hasData() const;
    void setData(const QByteArray& data);

    void loadFileFromUri(QByteArray&) const;

    // From  Resources
    bool hasChildren() const override;
    virtual QIcon icon() const override;
    void write(QDataStream& out, bool tag= true, bool saveData= true) const;
    void read(QDataStream& in);

    QVariant getData(ResourcesNode::DataValue i) const override;

    // utility function
    bool exists();
    const QString getAbsolueDir() const;

    bool operator==(const CleverURI& uri1) const;

signals:
    void pathChanged();
    void contentTypeChanged();
    void dataChanged();
    void stateChanged();
    void loadingModeChanged();

protected:
    void loadData();
    void init();

private:
    QString m_path;
    Core::ContentType m_type;
    QByteArray m_data;
    Core::LoadingMode m_loadingMode;
    Core::State m_state;

    static QStringList m_typeNameList;
    static QStringList m_typeToPreferenceDirectory;
};

typedef QList<CleverURI> CleverUriList;
#endif // CLEVERURI_H
