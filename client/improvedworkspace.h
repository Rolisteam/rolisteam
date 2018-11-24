/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			   *
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://www.rolisteam.org/contact                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify  *
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
#ifndef WORKSPACE_AMELIORE_H
#define WORKSPACE_AMELIORE_H

#include <QWidget>
#include <QImage>
#include <QPaintEvent>
#include <QMdiArea>


#include "preferences/preferencesmanager.h"
#include "preferences/preferenceslistener.h"
#include "data/mediacontainer.h"

/**
 * @brief The ImprovedWorkspace class
 */
class ImprovedWorkspace : public QMdiArea,public PreferencesListener
{
    Q_OBJECT
public :
    enum Positioning {TopLeft,BottomLeft,Center,TopRight,BottomRight,Scaled,Filled,Repeated};
	ImprovedWorkspace(QWidget *parent = 0);
	~ImprovedWorkspace();

    QWidget* addWindow(QWidget*,QAction* action);
   // QWidget* activeWindow();

    void insertActionAndSubWindow(QAction*, QMdiSubWindow*);
    /**
     * @brief getSubWindowFromId must be improved with polymorphism
     * @param id
     */
    QMdiSubWindow* getSubWindowFromId(QString id);
    /**
     * @brief addComteneurMedia
     * @param mediac
     */
    void addContainerMedia(MediaContainer* mediac);
    /**
     * @brief preferencesHasChanged
     */
    void preferencesHasChanged(QString);
    /**
     * @brief showCleverUri
     * @param uri
     */
    bool showCleverUri(CleverURI *uri);
    /**
     * @brief removeMediaContainer
     * @param mediac
     */
    void removeMediaContainer(MediaContainer* mediac);
    QVector<QMdiSubWindow *> getAllSubWindowFromId(const QString& id) const;
signals:
    void removedAction(QAction*);

public slots:
    void setTabbedMode(bool );
    void ensurePresent();
    void addWidgetToMdi(QWidget*,QString title);

protected :
    void resizeEvent ( QResizeEvent * event );
    bool eventFilter(QObject *object, QEvent *event);
    bool updateTitleTab();
private :
    void updateBackGround();


private:
    QBrush m_background;
    QColor m_color;

    QPixmap* m_backgroundPicture;
    QPixmap m_variableSizeBackground;

    PreferencesManager* m_preferences;
    QMap<QAction*,QMdiSubWindow*>* m_actionSubWindowMap;
    QImage *imageFond;		// Image de fond du workspace

    QString m_fileName;
    QHash<QMdiSubWindow*, QString> m_titleBar;
};

#endif
