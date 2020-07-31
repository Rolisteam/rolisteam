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

#include "controller/view_controller/mediacontrollerbase.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

/**
 * @brief The MediaContainer class
 */
class MediaContainer : public QMdiSubWindow
{
    Q_OBJECT
    Q_PROPERTY(QString ownerId READ ownerId WRITE setOwnerId NOTIFY ownerIdChanged)

public:
    enum class ContainerType : int
    {
        ImageContainer,
        VMapContainer,
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
    MediaContainer(MediaControllerBase* ctrl, ContainerType containerType, bool localIsGM, QWidget* parent= nullptr);

    MediaControllerBase* ctrl() const;
    /**
     * @brief ~MediaContainer
     */
    virtual ~MediaContainer() override;

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
     * @brief isUriEndWith
     * @return
     */
    virtual bool isUriEndWith(QString);

    /**
     * @brief readFile
     * @return
     */
    // virtual bool readFileFromUri()= 0;
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
     * @brief saveMedia must be implemented for each media container.
     */
    virtual void saveMedia(const QString& formerPath= QString())= 0;
    /**
     * @brief putDataIntoCleverUri when the uri is empty, save media into cleverURI data
     */
    virtual void putDataIntoCleverUri()= 0;
    /**
     * @brief is called when the selected tool changes.
     * @param the current tool.
     */
    virtual void currentToolChanged(Core::SelectableTool);
    /**
     * @brief is called when the cursor must be changed.
     * @param the new cursor.
     * @todo gathering this function and currentToolChanged should be a better choice.
     */
    virtual void currentCursorChanged(QCursor*);

    void addActionToMenu(QMenu& menu);

    // virtual void fill(NetworkMessageWriter& msg);
    // virtual void readMessage(NetworkMessageReader& msg);

    bool isRemote() const;
    void setRemote(bool remote);

    QString ownerId() const;
    void setOwnerId(const QString& ownerId);

    void setContainerType(const ContainerType& containerType);
    MediaContainer::ContainerType getContainerType() const;

    QString mediaId() const;

protected:
    void closeEvent(QCloseEvent* event) override;
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
    void setVisible(bool b) override;

    void detachView(bool b);

protected slots:
    virtual void updateTitle()= 0;

protected:
    QPointer<MediaControllerBase> m_lifeCycleCtrl;
    QString m_filter;
    PreferencesManager* m_preferences;
    QAction* m_action= nullptr;
    QCursor* m_currentCursor= nullptr;
    /**
     * @brief the current tool, it is an enum item.
     */
    Core::SelectableTool m_currentTool;
    QAction* m_detachedDialog;
    bool m_remote= false;
    ContainerType m_containerType;
};

#endif // MEDIACONTAINER_H
