/***************************************************************************
    *	Copyright (C) 2009 by Renaud Guezennec                                 *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                    *
    *                                                                          *
    *   rolisteam is free software; you can redistribute it and/or modify      *
    *   it under the terms of the GNU General Public License as published by   *
    *   the Free Software Foundation; either version 2 of the License, or      *
    *   (at your option) any later version.                                    *
    *                                                                          *
    *   This program is distributed in the hope that it will be useful,        *
    *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
    *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
    *   GNU General Public License for more details.                           *
    *                                                                          *
    *   You should have received a copy of the GNU General Public License      *
    *   along with this program; if not, write to the                          *
    *   Free Software Foundation, Inc.,                                        *
    *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
    ***************************************************************************/


#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QUuid>

#include "vmapframe.h"
#include "map/mapwizzard.h"

VMapFrame::VMapFrame()
    : MediaContainer()
{
    setObjectName("VMapFrame");
    m_vmap = new VMap();
}

VMapFrame::VMapFrame(CleverURI* uri,VMap *map)
    : MediaContainer(),m_vmap(map)
{
    setObjectName("VMapFrame");
    m_uri =uri;
    
    createView();
    updateMap();  
}


VMapFrame::~VMapFrame()
{
    delete m_maskPixmap;
}


void VMapFrame::closeEvent(QCloseEvent *event)
{
    
    hide();
	event->accept();
}
void  VMapFrame::createView()
{
    m_graphicView = new RGraphicsView(m_vmap);
	connect(m_vmap,SIGNAL(mapChanged()),this,SLOT(updateTitle()));
}
void VMapFrame::updateMap()
{
    if(m_vmap->mapTitle().isEmpty())
    {
        m_vmap->setTitle(tr("Unknown Map"));
    }
    setTitle(m_vmap->mapTitle());
    m_graphicView->setGeometry(0,0,m_vmap->mapWidth(),m_vmap->mapHeight());
    setGeometry(m_graphicView->geometry());
	setWidget(m_graphicView);
    setWindowIcon(QIcon(":/map.png"));
    m_maskPixmap = new QPixmap(m_graphicView->size());
	updateTitle();
}
void VMapFrame::updateTitle()
{
    setWindowTitle(tr("%1 - visibility:%2 - permission:%3 - layer:%4").arg(m_vmap->getTitle())
				   .arg(m_vmap->getVisibilityModeText())
				   .arg(m_vmap->getPermissionModeText())
                   .arg(m_vmap->getCurrentLayer()));
}

VMap* VMapFrame::getMap()
{
    return m_vmap;
}
int VMapFrame::editingMode()
{
    return m_currentEditingMode;
}

void VMapFrame::currentCursorChanged(QCursor* cursor)
{
    m_currentCursor = cursor;
    m_graphicView->setCursor(*cursor);
}
void VMapFrame::currentToolChanged(VToolsBar::SelectableTool selectedtool)
{
    m_currentTool = selectedtool;
    if(m_vmap != NULL)
    {
        m_vmap->setCurrentTool(selectedtool);
    }
	if(NULL!=m_graphicView)
	{
		m_graphicView->currentToolChanged(selectedtool);
	}
    switch (m_currentTool)
    {
    case VToolsBar::HANDLER:
        m_graphicView->setDragMode(QGraphicsView::RubberBandDrag);
        break;
    default:
        m_graphicView->setDragMode(QGraphicsView::NoDrag);
        break;
    }
}
void VMapFrame::mousePressEvent(QMouseEvent* event)
{
    MediaContainer::mousePressEvent(event);
}
void VMapFrame::currentPenSizeChanged(int ps)
{
    if(m_vmap != NULL)
    {
        m_vmap->setPenSize(ps);
    }
}
void VMapFrame::setCurrentNpcNameChanged(QString str)
{
    if(m_vmap != NULL)
        m_vmap->setCurrentNpcName(str);
}
void VMapFrame::setCurrentNpcNumberChanged(int str)
{
    if(m_vmap != NULL)
        m_vmap->setCurrentNpcNumber(str);
}
void VMapFrame::currentColorChanged(QColor& penColor)
{
    m_penColor = penColor;
    
    if(m_vmap !=NULL)
    {
        m_vmap->setCurrentChosenColor(m_penColor);
    }
}
void VMapFrame::setEditingMode(int mode)
{
    m_currentEditingMode = mode;
}
bool VMapFrame::defineMenu(QMenu* /*menu*/)
{
    return false;
}

void VMapFrame::openFile(const QString& filepath)
{
    if(!filepath.isEmpty())
    {
        QFile input(filepath);
        if (!input.open(QIODevice::ReadOnly))
            return;
        QDataStream in(&input);
        m_vmap->openFile(in);
        createView();

        updateMap();
    }
}
void VMapFrame::keyPressEvent ( QKeyEvent * event )
{
    switch (event->key())
    {
    case Qt::Key_Delete:
        /// @todo remove selected item
        break;
    case Qt::Key_Escape:
        //m_toolsbar->setCurrentTool(VToolsBar::HANDLER);
        emit defineCurrentTool(VToolsBar::HANDLER);
        break;
    default:
        MediaContainer::keyPressEvent(event);
    }
}
void VMapFrame::setCleverURI(CleverURI* uri)
{
    m_uri=uri;
}
bool VMapFrame::openMedia()
{
    MapWizzard wiz(true);
    wiz.resetData();
    if(wiz.exec()==QMessageBox::Accepted)
    {
        QString path = wiz.getFilepath();
        QFileInfo info(path);
        m_uri->setUri( path);
        m_preferences->registerValue("MapDirectory",info.absolutePath());
        return true;
    }
    return false;
}
void VMapFrame::saveMedia()
{
    if(NULL!=m_vmap)
    {
        if(!m_uri->getUri().endsWith(".vmap"))
        {
            QString str = m_uri->getUri()+".vmap";
            m_uri->setUri(str);
        }

        QFile file(m_uri->getUri());
        if (!file.open(QIODevice::WriteOnly))
        {
            return;
        }
        QDataStream out(&file);
        m_vmap->saveFile(out);
        file.close();
    }
}

bool VMapFrame::hasDockWidget() const
{
    return false;
}
QDockWidget* VMapFrame::getDockWidget()
{
    return NULL;
    //return m_toolsbar;
}
bool VMapFrame::readFileFromUri()
{
    openFile(m_uri->getUri());
    return true;
}
void VMapFrame::processAddItemMessage(NetworkMessageReader* msg)
{
    if(NULL!=m_vmap)
    {
        m_vmap->processAddItemMessage(msg);
    }
}
void VMapFrame::processMoveItemMessage(NetworkMessageReader* msg)
{
    if(NULL!=m_vmap)
    {
        m_vmap->processMoveItemMessage(msg);
    }
}
void VMapFrame::processGeometryChangeItem(NetworkMessageReader* msg)
{
    if(NULL!=m_vmap)
    {
        m_vmap->processGeometryChangeItem(msg);
    }
}
void VMapFrame::processMapPropertyChange(NetworkMessageReader* msg)
{
    if(NULL!=m_vmap)
    {
        m_vmap->readMessage(*msg,false);
    }
}
void VMapFrame::processGeometryViewChange(NetworkMessageReader* msg)
{
    if(NULL!=m_vmap)
    {
        m_graphicView->readMessage(msg);
    }
}
void VMapFrame::processDelItemMessage(NetworkMessageReader* msg)
{
    if(NULL!=m_vmap)
    {
        m_vmap->processDelItemMessage(msg);
    }
}

QString VMapFrame::getMapId()
{
    if(NULL!=m_vmap)
    {
        return m_vmap->getId();
    }
    return QString();
}
