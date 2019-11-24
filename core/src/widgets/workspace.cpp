/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			   *
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   https://rolisteam.org/contact                   *
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

#include <QtGui>

#include "controller/contentcontroller.h"
#include "workspace.h"

#define GRAY_SCALE 191

Workspace::Workspace(ContentController* ctrl, QWidget* parent)
    : QMdiArea(parent)
    , m_ctrl(ctrl)
    , m_variableSizeBackground(size())
{
    connect(m_ctrl, &ContentController::maxLengthTabNameChanged, this, &Workspace::updateTitleTab);
    connect(m_ctrl, &ContentController::shortTitleTabChanged, this, &Workspace::updateTitleTab);

    connect(m_ctrl, &ContentController::workspaceFilenameChanged, this,
            [this]() { setBackgroundImagePath(m_ctrl->workspaceFilename()); });
    connect(m_ctrl, &ContentController::workspaceColorChanged, this,
            [this]() { setBackgroundColor(m_ctrl->workspaceColor()); });
    connect(m_ctrl, &ContentController::workspacePositioningChanged, this,
            [this]() { setBackgroundPositioning(static_cast<Positioning>(m_ctrl->workspacePositioning())); });
    updateBackGround();
}

Workspace::~Workspace()
{
    /*if(nullptr != m_actionSubWindowMap)
    {
        delete m_actionSubWindowMap;
        m_actionSubWindowMap= nullptr;
    }*/
}

void Workspace::setBackgroundImagePath(const QString& path)
{
    if(m_bgFilename == path)
        return;
    m_bgFilename= path;
    emit backgroundImagePathChanged();
    m_backgroundPicture= QPixmap(m_bgFilename);
    updateBackGround();
}

QString Workspace::backgroundImagePath() const
{
    return m_bgFilename;
}

void Workspace::setBackgroundColor(const QColor& color)
{
    if(color == m_color)
        return;
    m_color= color;
    emit backgroundColorChanged();
    updateBackGround();
}
QColor Workspace::backgroundColor() const
{
    return m_color;
}

void Workspace::setBackgroundPositioning(Positioning pos)
{
    if(m_positioning == pos)
        return;
    m_positioning= pos;
    emit backgroundPositioningChanged();
    updateBackGround();
}
Workspace::Positioning Workspace::backgroundPositioning() const
{
    return m_positioning;
}

bool Workspace::showCleverUri(CleverURI* uri)
{
    for(auto& i : m_actionSubWindowMap)
    {
        if(i.isNull())
            continue;
        auto media= qobject_cast<MediaContainer*>(i);
        if(nullptr == media)
            continue;

        if(media->getCleverUri() == uri)
        {
            i->setVisible(true);
            return true;
        }
    }
    return false;
}

void Workspace::updateBackGround()
{
    QBrush background;
    background.setColor(m_color);
    setBackground(background);

    m_variableSizeBackground= m_variableSizeBackground.scaled(size());
    m_variableSizeBackground.fill(m_color);
    QPainter painter(&m_variableSizeBackground);

    int x= 0;
    int y= 0;
    int w= m_backgroundPicture.width();
    int h= m_backgroundPicture.height();
    bool repeated= false;
    switch(m_positioning)
    {
    case TopLeft:
        break;
    case BottomLeft:
        y= m_variableSizeBackground.size().height() - h;
        break;
    case Center:
        x= m_variableSizeBackground.width() / 2 - w / 2;
        y= m_variableSizeBackground.height() / 2 - h / 2;
        break;
    case TopRight:
        x= m_variableSizeBackground.width() - w;
        break;
    case BottomRight:
        x= m_variableSizeBackground.width() - w;
        y= m_variableSizeBackground.height() - h;
        break;
    case Scaled:
    {
        qreal rd= static_cast<qreal>(w) / static_cast<qreal>(h);
        if(rd > 1)
        {
            w= m_variableSizeBackground.width();
            h= static_cast<int>(w / rd);
        }
        else
        {
            h= m_variableSizeBackground.height();
            w= static_cast<int>(h / rd);
        }
        x= m_variableSizeBackground.width() / 2 - w / 2;
        y= m_variableSizeBackground.height() / 2 - h / 2;
    }
    break;
    case Filled:
        w= m_variableSizeBackground.width();
        h= m_variableSizeBackground.height();
        break;
    case Repeated:
        repeated= true;
        break;
    }
    if(!repeated)
    {
        painter.drawPixmap(x, y, w, h, m_backgroundPicture);
        setBackground(QBrush(m_variableSizeBackground));
    }
    else
    {
        setBackground(QBrush(m_backgroundPicture));
    }
    update();
}

void Workspace::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    if((m_variableSizeBackground.size() == this->size()))
    {
        return;
    }

    updateBackGround();

    QMdiArea::resizeEvent(event);
}
QWidget* Workspace::addWindow(QWidget* child, QAction* action)
{
    QMdiSubWindow* sub= addSubWindow(child);
    if(viewMode() == QMdiArea::TabbedView)
    {
        action->setChecked(true);
        sub->setVisible(true);
        child->setVisible(true);
    }
    insertActionAndSubWindow(action, sub);
    connect(action, &QAction::triggered, this, &Workspace::ensurePresent);
    sub->setAttribute(Qt::WA_DeleteOnClose, false);
    child->setAttribute(Qt::WA_DeleteOnClose, false);
    sub->setObjectName(child->objectName());

    sub->installEventFilter(this);
    return sub;
}
void Workspace::addContainerMedia(MediaContainer* mediac)
{
    if(nullptr == mediac)
        return;

    addSubWindow(mediac);
    insertActionAndSubWindow(mediac->getAction(), mediac);
    if(viewMode() == QMdiArea::TabbedView)
    {
        mediac->setVisible(true);
    }
    mediac->setAttribute(Qt::WA_DeleteOnClose, false);
    if(nullptr != mediac->widget())
    {
        mediac->widget()->setAttribute(Qt::WA_DeleteOnClose, false);
    }
    mediac->installEventFilter(this);
}
void Workspace::removeMediaContainer(MediaContainer* mediac)
{
    removeSubWindow(mediac);
    m_actionSubWindowMap.remove(mediac->getAction());
    mediac->removeEventFilter(this);
}
void Workspace::insertActionAndSubWindow(QAction* act, QMdiSubWindow* sub)
{
    m_actionSubWindowMap.insert(act, sub);
}
void Workspace::setTabbedMode(bool isTabbed)
{
    if(isTabbed)
    {
        setViewMode(QMdiArea::TabbedView);
        // setTabsClosable ( true );
        setTabsMovable(true);
        setTabPosition(QTabWidget::North);
        /// make all subwindows visible.
        auto keys= m_actionSubWindowMap.keys();
        for(QAction* act : keys)
        {
            auto tmp= m_actionSubWindowMap.value(act);
            if(nullptr == tmp)
            {
                tmp->setVisible(true);
                if(nullptr != act)
                {
                    act->setChecked(true);
                }
                if(nullptr != tmp->widget())
                {
                    tmp->widget()->setVisible(true);
                }
            }
        }
    }
    else
    {
        setViewMode(QMdiArea::SubWindowView);
    }
    updateTitleTab();
}
bool Workspace::updateTitleTab()
{
    bool shortName= m_ctrl->shortTitleTab(); // m_preferences->value("shortNameInTabMode", false).toBool();
    int textLength= m_ctrl->maxLengthTabName();
    // int textLength= m_preferences->value("MaxLengthTabName", 10).toInt();
    if((viewMode() == QMdiArea::TabbedView) && (shortName))
    {
        auto values= m_actionSubWindowMap.values();
        for(auto& subwindow : values)
        {
            auto title= subwindow->windowTitle();
            if(title.size() > textLength)
            {
                m_titleBar.insert(subwindow, title);
                title= title.left(textLength);
                subwindow->setWindowTitle(title);
            }
        }
    }
    else
    {
        auto keys= m_titleBar.keys();
        for(auto& key : keys)
        {
            auto title= m_titleBar.value(key);
            key->setWindowTitle(title);
        }
        m_titleBar.clear();
    }
    return true;
}
bool Workspace::eventFilter(QObject* object, QEvent* event)
{
    if(event->type() == QEvent::Close)
    {
        QMdiSubWindow* sub= dynamic_cast<QMdiSubWindow*>(object);
        if(nullptr != sub)
        {
            sub->setVisible(false);
            event->accept();
            return true;
        }
    }
    return QMdiArea::eventFilter(object, event);
}
void Workspace::ensurePresent()
{
    QAction* act= qobject_cast<QAction*>(sender());
    if(nullptr != act)
    {
        if(!subWindowList().contains(m_actionSubWindowMap.value(act)))
        {
            m_actionSubWindowMap.value(act)->widget()->setVisible(true);
            addSubWindow(m_actionSubWindowMap.value(act));
        }
    }
}

QVector<QMdiSubWindow*> Workspace::getAllSubWindowFromId(const QString& id) const
{
    QVector<QMdiSubWindow*> vector;

    for(auto& tmp : subWindowList())
    {
        if(nullptr != tmp->widget())
        {
            MediaContainer* tmpWindow= dynamic_cast<MediaContainer*>(tmp);
            if(nullptr != tmpWindow)
            {
                if(tmpWindow->getMediaId() == id)
                {
                    vector.append(tmp);
                }
            }
        }
    }
    return vector;
}

QMdiSubWindow* Workspace::getSubWindowFromId(const QString& id)
{
    for(auto& tmp : subWindowList())
    {
        if(nullptr != tmp->widget())
        {
            MediaContainer* tmpWindow= dynamic_cast<MediaContainer*>(tmp->widget());
            if(nullptr != tmpWindow)
            {
                if(tmpWindow->getMediaId() == id)
                {
                    return tmp;
                }
            }
        }
    }
    return nullptr;
}

void ImprovedWorkspace::preferencesHasChanged(const QString&)
{
    updateBackGround();
    update();
    updateTitleTab();
}
void Workspace::addWidgetToMdi(QWidget* wid, const QString& title)
{
    wid->setParent(this);
    QMdiSubWindow* sub= addSubWindow(wid);
    sub->setWindowTitle(title);
    wid->setWindowTitle(title);
    sub->setVisible(true);
    wid->setVisible(true);
}
