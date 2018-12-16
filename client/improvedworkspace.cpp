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


#include <QtGui>

#include "improvedworkspace.h"
//#include "map/mapframe.h"
//#include "image.h"

#define GRAY_SCALE 191
/********************************************************************/
/* Constructeur                                                     */
/********************************************************************/	
ImprovedWorkspace::ImprovedWorkspace(QWidget *parent)
: QMdiArea(parent),m_variableSizeBackground(size()),m_actionSubWindowMap(new QMap<QAction*,QMdiSubWindow*>())
{
    m_preferences =  PreferencesManager::getInstance();

    m_preferences->registerListener("BackGroundPositioning",this);
    m_preferences->registerListener("PathOfBackgroundImage",this);
    m_preferences->registerListener("BackGroundColor",this);
    m_preferences->registerListener("shortNameInTabMode",this);
    m_preferences->registerListener("MaxLengthTabName",this);

    m_backgroundPicture = new QPixmap(size());

    updateBackGround();

}

ImprovedWorkspace::~ImprovedWorkspace()
{
    if(nullptr!=m_backgroundPicture)
	{
		delete m_backgroundPicture;
        m_backgroundPicture = nullptr;
	}

    if(nullptr!=m_actionSubWindowMap)
	{
        delete m_actionSubWindowMap;
        m_actionSubWindowMap = nullptr;
	}
}
bool ImprovedWorkspace::showCleverUri(CleverURI* uri)
{
    for(auto i : m_actionSubWindowMap->values())
    {
        auto media = dynamic_cast<MediaContainer*>(i);
        if(nullptr != media)
        {
            if(media->getCleverUri() == uri)
            {
                i->setVisible(true);
                return true;
            }
        }
    }
    return false;
}


void ImprovedWorkspace::updateBackGround()
{
    m_color = m_preferences->value("BackGroundColor",QColor(GRAY_SCALE,GRAY_SCALE,GRAY_SCALE)).value<QColor>();
    m_background.setColor(m_color);
    setBackground(m_background);

    QString fileName = m_preferences->value("PathOfBackgroundImage",":/resources/icons/workspacebackground.jpg").toString();
    m_variableSizeBackground = m_variableSizeBackground.scaled(size());
    m_variableSizeBackground.fill(m_color);
    QPainter painter(&m_variableSizeBackground);

    if(m_fileName!=fileName)
    {
        m_fileName = fileName;
        if(nullptr!=m_backgroundPicture)
        {
            delete m_backgroundPicture;
        }
        m_backgroundPicture = new QPixmap(m_fileName);
    }
    int x=0;
    int y=0;
    int w = m_backgroundPicture->width();
    int h = m_backgroundPicture->height();
    bool repeated=false;
    switch(static_cast<Positioning>(m_preferences->value("BackGroundPositioning",0).toInt()))
    {
    case TopLeft:
        break;
    case BottomLeft:
        y = m_variableSizeBackground.size().height()-h;
        break;
    case Center:
        x = m_variableSizeBackground.width()/2-w/2;
        y = m_variableSizeBackground.height()/2-h/2;
        break;
    case TopRight:
        x = m_variableSizeBackground.width()-w;
        break;
    case BottomRight:
        x = m_variableSizeBackground.width()-w;
        y = m_variableSizeBackground.height()-h;
        break;
    case Scaled:
    {
        qreal rd=(qreal)w/(qreal)h;
        if(rd>1)
        {
            w=m_variableSizeBackground.width();
            h=w/rd;
        }
        else
        {
            h=m_variableSizeBackground.height();
            w=h/rd;
        }
        x = m_variableSizeBackground.width()/2-w/2;
        y = m_variableSizeBackground.height()/2-h/2;
    }
        break;
    case Filled:
        w = m_variableSizeBackground.width();
        h = m_variableSizeBackground.height();
        break;
    case Repeated:
        repeated = true;
        break;
    }
    if(!repeated)
    {
        painter.drawPixmap(x,y,w,h,*m_backgroundPicture);
        setBackground(QBrush(m_variableSizeBackground));
    }
    else
    {
        setBackground(QBrush(*m_backgroundPicture));
    }
    update();
}

void ImprovedWorkspace::resizeEvent ( QResizeEvent * event )
{
    Q_UNUSED(event);
    if((m_variableSizeBackground.size()==this->size()))
    {
        return;
    }

    updateBackGround();

    QMdiArea::resizeEvent(event);
}
QWidget*  ImprovedWorkspace::addWindow(QWidget* child,QAction* action)
{
    QMdiSubWindow* sub = addSubWindow(child);
    if(viewMode()==QMdiArea::TabbedView)
    {
        action->setChecked(true);
        sub->setVisible(true);
        child->setVisible(true);
    }
    insertActionAndSubWindow(action,sub);
    connect(action,SIGNAL(triggered()),this,SLOT(ensurePresent()));
    sub->setAttribute(Qt::WA_DeleteOnClose, false);
    child->setAttribute(Qt::WA_DeleteOnClose, false);
    sub->setObjectName(child->objectName());

    sub->installEventFilter(this);
    return sub;
}
void ImprovedWorkspace::addContainerMedia(MediaContainer* mediac)
{
    if(nullptr!=mediac)
    {
        addSubWindow(mediac);
        insertActionAndSubWindow(mediac->getAction(),mediac);
        if(viewMode()==QMdiArea::TabbedView)
        {
            mediac->setVisible(true);
        }
        mediac->setAttribute(Qt::WA_DeleteOnClose, false);
        if(nullptr!=mediac->widget())
        {
            mediac->widget()->setAttribute(Qt::WA_DeleteOnClose, false);
        }
        mediac->installEventFilter(this);
    }
}
void ImprovedWorkspace::removeMediaContainer(MediaContainer* mediac)
{
    removeSubWindow(mediac);
    m_actionSubWindowMap->remove(mediac->getAction());
    mediac->removeEventFilter(this);
}
void ImprovedWorkspace::insertActionAndSubWindow(QAction* act, QMdiSubWindow* sub)
{
    m_actionSubWindowMap->insert(act,sub);
}
void ImprovedWorkspace::setTabbedMode(bool isTabbed)
{
    if(isTabbed)
    {
        setViewMode(QMdiArea::TabbedView);
        //setTabsClosable ( true );
        setTabsMovable ( true );
        setTabPosition(QTabWidget::North);
        /// make all subwindows visible.
        for(QAction* act : m_actionSubWindowMap->keys())
        {
            auto tmp = m_actionSubWindowMap->value(act);
            if(nullptr == tmp)
            {
                tmp->setVisible(true);
                if(nullptr!=act)
                {
                    act->setChecked(true);
                }
                if(nullptr!=tmp->widget())
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
bool ImprovedWorkspace::updateTitleTab()
{
    bool shortName = m_preferences->value("shortNameInTabMode",false).toBool();
    int textLength = m_preferences->value("MaxLengthTabName",10).toInt();
    if((viewMode() == QMdiArea::TabbedView)&&(shortName))
    {
        for(auto subwindow : m_actionSubWindowMap->values())
        {
            auto title = subwindow->windowTitle();
            if(title.size() > textLength)
            {
                m_titleBar.insert(subwindow,title);
                title = title.left(textLength);
                subwindow->setWindowTitle(title);
            }
        }
    }
    else
    {

        for(auto key : m_titleBar.keys())
        {
            auto title = m_titleBar.value(key);
            key->setWindowTitle(title);
        }
        m_titleBar.clear();
    }
    return true;
}
bool ImprovedWorkspace::eventFilter(QObject *object, QEvent *event)
{
    if(event->type()==QEvent::Close)
    {
        QMdiSubWindow* sub = dynamic_cast<QMdiSubWindow*>(object);
        if(nullptr!=sub)
        {
            sub->setVisible(false);
            event->accept();
            return true;
        }
    }
    return QMdiArea::eventFilter(object,event);
}
void ImprovedWorkspace::ensurePresent()
{
    QAction* act = qobject_cast<QAction*>(sender());
    if(nullptr!=act)
    {
        if(!subWindowList().contains(m_actionSubWindowMap->value(act)))
        {
            m_actionSubWindowMap->value(act)->widget()->setVisible(true);
            addSubWindow(m_actionSubWindowMap->value(act));
        }
    }
}

QVector<QMdiSubWindow*> ImprovedWorkspace::getAllSubWindowFromId(const QString& id) const
{
    QVector<QMdiSubWindow*> vector;

    for(QMdiSubWindow* tmp: subWindowList())
    {
        if(nullptr!=tmp->widget())
        {
            MediaContainer* tmpWindow = dynamic_cast<MediaContainer*>(tmp);
            if(nullptr!=tmpWindow)
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

QMdiSubWindow* ImprovedWorkspace::getSubWindowFromId(QString id)
{
    for(QMdiSubWindow* tmp: subWindowList())
    {
        if(nullptr!=tmp->widget())
        {
            MediaContainer* tmpWindow = dynamic_cast<MediaContainer*>(tmp->widget());
            if(nullptr!=tmpWindow)
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

void ImprovedWorkspace::preferencesHasChanged(QString)
{
    updateBackGround();
    update();
    updateTitleTab();
}
void ImprovedWorkspace::addWidgetToMdi(QWidget* wid,QString title)
{
    wid->setParent(this);
    QMdiSubWindow* sub = addSubWindow(wid);
    sub->setWindowTitle(title);
    wid->setWindowTitle(title);
    sub->setVisible(true);
    wid->setVisible(true);
}
