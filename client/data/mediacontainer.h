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



#include <QString>
#include <QMdiSubWindow>
#include <QAction>


#include "data/cleveruri.h"
#include "preferences/preferencesmanager.h"
#include "vmap/vtoolbar.h"
#include "cleveruri.h"

#include "network/networkmessagewriter.h"

/**
 * @brief The MediaContainer class
 */
class MediaContainer : public QMdiSubWindow
{
    Q_OBJECT
public:
    /**
     * @brief MediaContainer
     */
    MediaContainer(QWidget* parent = NULL);
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
	virtual bool readFileFromUri()=0;
	/**
	 * @brief openFile
	 * @return
	 */
    virtual bool openMedia();

	/**
	 * @brief getTitle
	 * @return
	 */
	virtual QString getTitle() const;
    /**
     * @brief setTitle
     * @param str
     */
    virtual void setTitle(QString str);
    /**
     * @brief error
     * @param err - error message to display
     */
    void error(QString err,QWidget* parent);
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
    virtual void saveMedia() =0;
    /**
     * @brief putDataIntoCleverUri when the uri is empty, save media into cleverURI data
     */
    virtual void putDataIntoCleverUri() = 0;
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
    virtual QString getMediaId();
    /**
     * @brief setMediaId
     */
    virtual void setMediaId(QString);

    void addActionToMenu(QMenu& menu);

    virtual QUndoStack* getUndoStack() const;
    virtual void setUndoStack(QUndoStack* undoStack);


    virtual void fill(NetworkMessageWriter& msg);
    virtual void readMessage(NetworkMessageReader& msg);

    bool isRemote() const;
    void setRemote(bool remote);

signals:
    /**
     * @brief visibleChanged
     */
    void visibleChanged(bool);

public slots:
    /**
     * @brief setVisible
     * @param b
     */
    void setVisible(bool b);

    void detachView(bool b);
protected:
    QString m_localPlayerId;
	CleverURI* m_uri;
	QString m_title;
    QString m_filter;
    PreferencesManager* m_preferences;
    QAction* m_action;
    QCursor* m_currentCursor;
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
    bool m_remote;

};

#endif // MEDIACONTAINER_H
