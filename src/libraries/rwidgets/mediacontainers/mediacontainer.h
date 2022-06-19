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

#include "controller/view_controller/mediacontrollerbase.h"
#include "preferences/preferencesmanager.h"
#include "rwidgets/rwidgets_global.h"
#include "rwidgets/toolbars/vtoolbar.h"

/**
 * @brief The MediaContainer class
 */
class RWIDGET_EXPORT MediaContainer : public QMdiSubWindow
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
        WebViewContainer,
        MindMapContainer
    };
    Q_ENUM(ContainerType)
    /**
     * @brief MediaContainer
     */
    MediaContainer(MediaControllerBase* ctrl, ContainerType containerType, QWidget* parent= nullptr);

    MediaControllerBase* ctrl() const;
    /**
     * @brief ~MediaContainer
     */
    virtual ~MediaContainer() override;
    /**
     * @brief error
     * @param err - error message to display
     */
    void error(QString err, QWidget* parent);
    void addActionToMenu(QMenu& menu);

    QString ownerId() const;
    void setOwnerId(const QString& ownerId);

    void setContainerType(const ContainerType& containerType);
    MediaContainer::ContainerType getContainerType() const;

    QString mediaId() const;

protected:
    void closeEvent(QCloseEvent* event) override;
    void showEvent(QShowEvent* showEvent) override;
    void hideEvent(QHideEvent* hideEvent) override;

signals:
    void visibleChanged(bool);
    void ownerIdChanged();
    void uriNameChanged();

public slots:
    void setVisible(bool b) override;
    void detachView(bool b);

protected:
    QPointer<MediaControllerBase> m_lifeCycleCtrl;
    QString m_filter;
    PreferencesManager* m_preferences;
    QCursor* m_currentCursor= nullptr;
    QAction* m_detachedDialog;
    ContainerType m_containerType;
};

#endif // MEDIACONTAINER_H
