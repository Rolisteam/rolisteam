/***************************************************************************
 *	Copyright (C) 2015 by Renaud Guezennec                                 *
 *   http://www.renaudguezennec.eu/accueil,3.html                          *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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
#ifndef MEDIACONTAINER_H
#define MEDIACONTAINER_H

#include <QAction>
#include <QMdiSubWindow>
#include <QString>

#include "cleveruri.h"
#include "data/cleveruri.h"
#include "preferences/preferencesmanager.h"
#include "vmap/vtoolbar.h"

#include "network/networkmessagewriter.h"

/**
 * @brief The MediaContainer class
 */
class MediaContainer : public QMdiSubWindow, public CleverURIListener
{
    Q_OBJECT
    Q_PROPERTY(QString ownerId READ ownerId WRITE setOwnerId NOTIFY ownerIdChanged)
    Q_PROPERTY(QString uriName READ getUriName WRITE setUriName NOTIFY uriNameChanged)

public:
    enum class ContainerType : int
    {
        ImageContainer,
        VMapContainer,
        MapContainer,
        NoteContainer,
        SharedNoteContainer,
        ChatContainer,
        CharacterSheetContainer,
        PDFContainer,
        WebViewContainer
    };
    Q_ENUM(ContainerType)
    /**
     * @brief MediaContainer
     */
    MediaContainer(ContainerType containerType, bool localIsGM, QWidget* parent= nullptr);
    /**
     * @brief ~MediaContainer
     */
    virtual ~MediaContainer();

    /**
     * @brief setLocalPlayerId
     * @param id
     */
    void setLocalPlayerId(QString id);
    /**
     * @brief getLocalPlayerId
     * @return
     */
    QString getLocalPlayerId();

    /**
     * @brief setCleverUri
     * @param uri
     */
    virtual void setCleverUri(CleverURI* uri);
    /**
     * @brief getCleverUri
     * @return
     */
    virtual CleverURI* getCleverUri() const;
    /**
     * @brief isUriEndWith
     * @return
     */
    virtual bool isUriEndWith(QString);

    /**
     * @brief readFile
     * @return
     */
    virtual bool readFileFromUri()= 0;
    /**
     * @brief openFile
     * @return
     */
    virtual bool openMedia();
    /**
     * @brief error
     * @param err - error message to display
     */
    void error(QString err, QWidget* parent);
    /**
     * @brief setAction
     */
    void setAction(QAction*);
    /**
     * @brief getAction
     * @return
     */
    QAction* getAction();
    /**
     * @brief setCleverUriType
     */
    void setCleverUriType(CleverURI::ContentType);

    /**
     * @brief getContentType
     * @return
     */
    virtual CleverURI::ContentType getContentType();
    /**
     * @brief saveMedia must be implemented for each media container.
     */
    virtual void saveMedia()= 0;
    /**
     * @brief putDataIntoCleverUri when the uri is empty, save media into cleverURI data
     */
    virtual void putDataIntoCleverUri()= 0;
    /**
     * @brief is called when the selected tool changes.
     * @param the current tool.
     */
    virtual void currentToolChanged(VToolsBar::SelectableTool);
    /**
     * @brief is called when the cursor must be changed.
     * @param the new cursor.
     * @todo gathering this function and currentToolChanged should be a better choice.
     */
    virtual void currentCursorChanged(QCursor*);
    /**
     * @brief is called when the user has changed the selected color.
     * @param the new color.
     */
    virtual void currentColorChanged(QColor&);

    /**
     * @brief getMediaId
     * @return the media id
     */
    virtual QString getMediaId() const;
    virtual QString getUriName() const;
    /**
     * @brief setMediaId
     */
    virtual void setMediaId(QString);

    virtual void cleverURIHasChanged(CleverURI*, CleverURI::DataValue field);

    void addActionToMenu(QMenu& menu);

    virtual QUndoStack* getUndoStack() const;
    virtual void setUndoStack(QUndoStack* undoStack);

    virtual void fill(NetworkMessageWriter& msg);
    virtual void readMessage(NetworkMessageReader& msg);

    bool isRemote() const;
    void setRemote(bool remote);

    bool getLocalIsGM() const;
    void setLocalIsGM(bool localIsGM);

    QString ownerId() const;
    void setOwnerId(const QString& ownerId);

    void setContainerType(const ContainerType& containerType);
    MediaContainer::ContainerType getContainerType() const;
signals:
    /**
     * @brief visibleChanged
     */
    void visibleChanged(bool);
    void ownerIdChanged();
    void uriNameChanged();

public slots:
    /**
     * @brief setVisible
     * @param b
     */
    void setVisible(bool b);

    void detachView(bool b);
    virtual void setUriName(const QString &name);

protected slots:
    virtual void updateTitle()= 0;

protected:
    QString m_localPlayerId;
    CleverURI* m_uri= nullptr;
    QString m_filter;
    PreferencesManager* m_preferences;
    QAction* m_action=nullptr;
    QString m_name;
    QCursor* m_currentCursor=nullptr;
    /**
     * @brief the current tool, it is an enum item.
     */
    VToolsBar::SelectableTool m_currentTool;
    /**
     * @brief the current color of the pen.
     */
    QColor m_penColor;
    /**
     * @brief m_mediaId stores the unique id to identify this media. Helpful with network message.
     * see getMediaId and setMediaId
     */
    QString m_mediaId;
    QAction* m_detachedDialog;
    bool m_remote= false;
    bool m_localIsGM= false;
    QString m_ownerId;
    ContainerType m_containerType;
};

#endif // MEDIACONTAINER_H
