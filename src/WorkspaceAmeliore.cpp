/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			   *
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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

#include "WorkspaceAmeliore.h"

#include "CarteFenetre.h"
#include "Image.h"

//#include <QTextStream>

#define GRAY_SCALE 191
/********************************************************************/
/* Constructeur                                                     */
/********************************************************************/	
WorkspaceAmeliore::WorkspaceAmeliore(QWidget *parent)
: QMdiArea(parent),m_variableSizeBackground(size())
{
    m_preferences =  PreferencesManager::getInstance();
    m_map = new QMap<QAction*,QMdiSubWindow*>();

    m_backgroundPicture = new QPixmap(size());

    updateBackGround();

}

WorkspaceAmeliore::~WorkspaceAmeliore()
{
    delete m_backgroundPicture;
}
void WorkspaceAmeliore::updateBackGround()
{
    m_color = m_preferences->value("BackGroundColor",QColor(GRAY_SCALE,GRAY_SCALE,GRAY_SCALE)).value<QColor>();
    m_background.setColor(m_color);
    setBackground(m_background);

    QString fileName = m_preferences->value("PathOfBackgroundImage",":/resources/icones/fond workspace macos.bmp").toString();
    if (!QFile::exists(fileName))
    {
        fileName = ":/resources/icones/fond workspace macos.bmp";
    }

    m_variableSizeBackground = m_variableSizeBackground.scaled(size());

    m_variableSizeBackground.fill(m_color);
    QPainter painter(&m_variableSizeBackground);

    if(m_fileName!=fileName)
    {
        m_fileName = fileName;
        if(NULL!=m_backgroundPicture)
        {
            delete m_backgroundPicture;
        }
        m_backgroundPicture = new QPixmap(m_fileName);
    }



    painter.drawPixmap(0,0,m_backgroundPicture->width(),m_backgroundPicture->height(),*m_backgroundPicture);
    setBackground(QBrush(m_variableSizeBackground));
}

void WorkspaceAmeliore::resizeEvent ( QResizeEvent * event )
{
    Q_UNUSED(event);
    if((m_variableSizeBackground.size()==this->size()))
    {
        return;
    }

    updateBackGround();

    QMdiArea::resizeEvent(event);
}
QWidget*  WorkspaceAmeliore::addWindow(QWidget* child,QAction* action)
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
QWidget* WorkspaceAmeliore::activeWindow()
{
    return currentSubWindow();
}
void WorkspaceAmeliore::insertActionAndSubWindow(QAction* act, QMdiSubWindow* sub)
{
    m_map->insert(act,sub);
}
void WorkspaceAmeliore::setTabbedMode(bool isTabbed)
{
    if(isTabbed)
    {
        setViewMode(QMdiArea::TabbedView);

        //setTabsClosable ( true );
        setTabsMovable ( true );
        setTabPosition(QTabWidget::North);

        /// make all subwindows visible.

        foreach(QMdiSubWindow* tmp, subWindowList())
        {
            tmp->setVisible(true);
            m_map->key(tmp)->setChecked(true);
            if(NULL!=tmp->widget())
            {
                tmp->widget()->setVisible(true);
            }
        }
    }
    else
    {
        setViewMode(QMdiArea::SubWindowView);
    }
}

bool WorkspaceAmeliore::eventFilter(QObject *object, QEvent *event)
{
    if(event->type()==QEvent::Close)
    {
        QMdiSubWindow* sub = dynamic_cast<QMdiSubWindow*>(object);
        if(NULL!=sub)
        {
            removeSubWindow(sub);
            return true;
        }

    }
    return QMdiArea::eventFilter(object,event);
}
void WorkspaceAmeliore::ensurePresent()
{
    QAction* act = qobject_cast<QAction*>(sender());
    if(NULL!=act)
    {
        if(!subWindowList().contains(m_map->value(act)))
        {
            m_map->value(act)->widget()->setVisible(true);
            addSubWindow(m_map->value(act));
        }
    }
}

QMdiSubWindow* WorkspaceAmeliore::getSubWindowFromId(QString id)
{
    foreach(QMdiSubWindow* tmp, subWindowList())
    {
        if(NULL!=tmp->widget())
        {
            CarteFenetre* tmpWindow = dynamic_cast<CarteFenetre*>(tmp->widget());
            if(NULL!=tmpWindow)
            {
                if(tmpWindow->getMapId() == id)
                {
                    return tmp;
                }
            }
            else
            {
                Image* img = dynamic_cast<Image*>(tmp->widget());
                if(NULL!=img)
                {
                    if(img->getImageId() == id)
                    {
                        return tmp;
                    }
                }
            }
        }
    }
    return NULL;
}
