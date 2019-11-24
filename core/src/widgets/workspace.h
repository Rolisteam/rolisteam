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

#include <QImage>
#include <QMdiArea>
#include <QPaintEvent>
#include <QWidget>

#include "data/mediacontainer.h"

class ContentController;
/**
 * @brief The ImprovedWorkspace class
 */
class Workspace : public QMdiArea
{
    Q_OBJECT
    Q_PROPERTY(QString backgroundImagePath READ backgroundImagePath WRITE setBackgroundImagePath NOTIFY
                   backgroundImagePathChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(Positioning backgroundPositioning READ backgroundPositioning WRITE setBackgroundPositioning NOTIFY
                   backgroundPositioningChanged)
public:
    enum Positioning
    {
        TopLeft,
        BottomLeft,
        Center,
        TopRight,
        BottomRight,
        Scaled,
        Filled,
        Repeated
    };
    Workspace(ContentController* ctrl, QWidget* parent= nullptr);
    ~Workspace();

    void setBackgroundImagePath(const QString& path);
    QString backgroundImagePath() const;

    void setBackgroundColor(const QColor& color);
    QColor backgroundColor() const;

    void setBackgroundPositioning(Positioning pos);
    Positioning backgroundPositioning() const;

    QWidget* addWindow(QWidget*, QAction* action);
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
    bool showCleverUri(CleverURI* uri);
    /**
     * @brief removeMediaContainer
     * @param mediac
     */
    void removeMediaContainer(MediaContainer* mediac);
    QVector<QMdiSubWindow*> getAllSubWindowFromId(const QString& id) const;
signals:
    void removedAction(QAction*);
    void backgroundImagePathChanged();
    void backgroundColorChanged();
    void backgroundPositioningChanged();

public slots:
    void setTabbedMode(bool);
    void ensurePresent();
    void addWidgetToMdi(QWidget*, QString title);

protected:
    void resizeEvent(QResizeEvent* event);
    bool eventFilter(QObject* object, QEvent* event);
    bool updateTitleTab();

private:
    void updateBackGround();

private:
    QPointer<ContentController> m_ctrl;
    QColor m_color;

    Positioning m_positioning= TopLeft;

    QPixmap m_backgroundPicture;
    QPixmap m_variableSizeBackground;
    QMap<QAction*, QMdiSubWindow*>* m_actionSubWindowMap;

    QString m_bgFilename;
    QHash<QMdiSubWindow*, QString> m_titleBar;
};

#endif
