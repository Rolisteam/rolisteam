/***************************************************************************
 *	Copyright (C) 2009 by Renaud Guezennec                                 *
 *   https://rolisteam.org/contact                    *
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

#include <QHBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QUuid>
#include <QVBoxLayout>

#include "map/mapwizzard.h"
#include "vmapframe.h"

#include "controller/view_controller/vectorialmapcontroller.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

QString visibilityText(Core::VisibilityMode vis)
{
    QStringList visibilityData;
    visibilityData << QObject::tr("Hidden") << QObject::tr("Fog Of War") << QObject::tr("All visible");

    if(vis < visibilityData.size())
    {
        return visibilityData.at(vis);
    }
    return {};
}

QString permissionModeText(Core::PermissionMode mode)
{
    QStringList permissionData;
    permissionData << QObject::tr("No Right") << QObject::tr("His character") << QObject::tr("All Permissions");
    return permissionData.at(mode);
}

QString layerText(Core::Layer mode)
{
    QString str;
    switch(mode)
    {
    case Core::Layer::GROUND:
        str= QObject::tr("Ground");
        break;
    case Core::Layer::OBJECT:
        str= QObject::tr("Object");
        break;
    case Core::Layer::CHARACTER_LAYER:
        str= QObject::tr("Character");
        break;
    case Core::Layer::FOG:
        str= QObject::tr("Fog Layer");
        break;
    case Core::Layer::GRIDLAYER:
        str= QObject::tr("Grid Layer");
        break;
    case Core::Layer::NONE:
        str= QObject::tr("No Layer");
        break;
    }
    return str;
}

VMapFrame::VMapFrame(VectorialMapController* ctrl, QWidget* parent)
    : MediaContainer(ctrl, MediaContainer::ContainerType::VMapContainer, parent)
    , m_ctrl(ctrl)
    , m_vmap(new VMap(ctrl))
    , m_graphicView(new RGraphicsView(ctrl))
{
    setObjectName("VMapFrame");
    setWindowIcon(QIcon(":/vmap.png"));
    m_graphicView->setScene(m_vmap.get());
    setWidget(m_graphicView.get());

    connect(m_ctrl, &VectorialMapController::nameChanged, this, &VMapFrame::updateTitle);
    connect(m_ctrl, &VectorialMapController::visibilityChanged, this, &VMapFrame::updateTitle);
    connect(m_ctrl, &VectorialMapController::layerChanged, this, &VMapFrame::updateTitle);
    connect(m_ctrl, &VectorialMapController::permissionChanged, this, &VMapFrame::updateTitle);

    // m_vmap= new VMap();

    // connect(m_vmap, SIGNAL(mapStatutChanged()), this, SLOT(updateTitle()));
    // connect(m_vmap, SIGNAL(mapChanged()), this, SLOT(updateTitle()));

    updateTitle();
}

VMapFrame::~VMapFrame() {}

void VMapFrame::updateMap()
{
    /*if(m_vmap->getMapTitle().isEmpty())
    {
        setUriName(tr("Unknown Map"));
    }
    setUriName(m_vmap->getMapTitle());
    m_graphicView->setGeometry(0, 0, m_vmap->mapWidth(), m_vmap->mapHeight());
    setGeometry(0, 0, m_vmap->mapWidth(), m_vmap->mapHeight());

    m_vmap->setVisibilityMode(static_cast<Core::VisibilityMode>(m_vmap->getOption(VisualItem::VisibilityMode).toInt()));

    updateTitle();*/
}

void VMapFrame::updateTitle()
{
    setWindowTitle(tr("%1 - visibility: %2 - permission: %3 - layer: %4")
                       .arg(m_ctrl->name(), visibilityText(m_ctrl->visibility()),
                            permissionModeText(m_ctrl->permission()), layerText(m_ctrl->layer())));
}

void VMapFrame::currentCursorChanged(QCursor* cursor)
{
    m_currentCursor= cursor;
    m_graphicView->setCursor(*cursor);
}

void VMapFrame::currentToolChanged(Core::SelectableTool selectedtool)
{
    /* m_currentTool= selectedtool;
     if(m_vmap != nullptr)
     {
         m_vmap->setCurrentTool(selectedtool);
     }
     if(nullptr != m_graphicView)
     {
         m_graphicView->currentToolChanged(selectedtool);

         switch(m_currentTool)
         {
         case Core::HANDLER:
             m_graphicView->setDragMode(QGraphicsView::RubberBandDrag);
             break;
         default:
             m_graphicView->setDragMode(QGraphicsView::NoDrag);
             break;
         }
     }*/
}

bool VMapFrame::defineMenu(QMenu* /*menu*/)
{
    return false;
}

bool VMapFrame::openFile(const QString& filepath)
{
    if(filepath.isEmpty())
        return false;

    QFile input(filepath);
    if(!input.open(QIODevice::ReadOnly))
        return false;
    QDataStream in(&input);
    in.setVersion(QDataStream::Qt_5_7);
    /* createView();
     m_vmap->openFile(in);
     m_vmap->setVisibilityMode(Core::HIDDEN);*/
    updateMap();
    return true;
}

void VMapFrame::keyPressEvent(QKeyEvent* event)
{
    switch(event->key())
    {
    case Qt::Key_Delete:
        /// @todo remove selected item
        break;
    case Qt::Key_Escape:
        // m_toolsbar->setCurrentTool(VToolsBar::HANDLER);
        emit defineCurrentTool(Core::HANDLER);
        break;
    default:
        MediaContainer::keyPressEvent(event);
    }
}

void VMapFrame::saveMedia(const QString&)
{
    // if(nullptr != m_vmap)
    {
        /*   if(nullptr != m_uri)
           {
               if(!m_uri->getUri().endsWith(".vmap"))
               {
                   QString str= m_uri->getUri() + ".vmap";
                   m_uri->setUri(str);
               }

               QFile file(m_uri->getUri());
               if(!file.open(QIODevice::WriteOnly))
               {
                   return;
               }
               QDataStream out(&file);
               out.setVersion(QDataStream::Qt_5_7);
               m_vmap->saveFile(out);
               file.close();
           }*/
    }
}

void VMapFrame::putDataIntoCleverUri()
{
    /*  if(nullptr != m_vmap)
      {
          QByteArray data;
          QDataStream out(&data, QIODevice::WriteOnly);
          out.setVersion(QDataStream::Qt_5_7);
          m_vmap->saveFile(out);*/
    /*  if(nullptr != m_uri)
      {
          m_uri->setData(data);
      }*/
    // }
}

NetWorkReceiver::SendType VMapFrame::processMessage(NetworkMessageReader* msg)
{
    NetWorkReceiver::SendType type= NetWorkReceiver::NONE;
    /* if(nullptr == m_vmap)
         return type;

     switch(msg->action())
     {
     case NetMsg::DelPoint:
         break;
     case NetMsg::AddItem:
     {
         m_vmap->processAddItemMessage(msg);
         type= NetWorkReceiver::AllExceptSender;
     }
     break;
     case NetMsg::DelItem:
     {
         m_vmap->processDelItemMessage(msg);
         type= NetWorkReceiver::AllExceptSender;
     }
     break;
     case NetMsg::MoveItem:
     {
         m_vmap->processMoveItemMessage(msg);
         type= NetWorkReceiver::AllExceptSender;
     }
     break;
     case NetMsg::GeometryItemChanged:
     {
         m_vmap->processGeometryChangeItem(msg);
         type= NetWorkReceiver::AllExceptSender;
     }
     break;
     case NetMsg::OpacityItemChanged:
     {
         m_vmap->processOpacityMessage(msg);
         type= NetWorkReceiver::AllExceptSender;
     }
     break;
     case NetMsg::LayerItemChanged:
     {
         m_vmap->processLayerMessage(msg);
         type= NetWorkReceiver::AllExceptSender;
     }
     break;
     case NetMsg::vmapChanges:
     {
         // m_vmap->processMapPropertyChange(msg);
         m_vmap->readMessage(*msg, false);
         type= NetWorkReceiver::AllExceptSender;
     }
     break;
     case NetMsg::GeometryViewChanged:
     {
         // m_vmap->processGeometryViewChange(msg);
         m_graphicView->readMessage(msg);
         type= NetWorkReceiver::AllExceptSender;
     }
     break;
     case NetMsg::CharacterStateChanged:
         m_vmap->processCharacterStateHasChanged(*msg);
         type= NetWorkReceiver::AllExceptSender;
         break;
     case NetMsg::CharacterChanged:
         m_vmap->processCharacterHasChanged(*msg);
         type= NetWorkReceiver::AllExceptSender;
         break;
     case NetMsg::SetParentItem:
     {
         m_vmap->processSetParentItem(msg);
         type= NetWorkReceiver::AllExceptSender;
     }
     break;
     case NetMsg::ZValueItem:
     {
         m_vmap->processZValueMsg(msg);
         type= NetWorkReceiver::AllExceptSender;
     }
     break;
     case NetMsg::RectGeometryItem:
     {
         m_vmap->processRectGeometryMsg(msg);
         type= NetWorkReceiver::AllExceptSender;
     }
     break;
     case NetMsg::RotationItem:
     {
         m_vmap->processRotationMsg(msg);
         type= NetWorkReceiver::AllExceptSender;
     }
     break;
     case NetMsg::MovePoint:
     {
         m_vmap->processMovePointMsg(msg);
         type= NetWorkReceiver::AllExceptSender;
     }
     break;
     case NetMsg::VisionChanged:
     {
         m_vmap->processVisionMsg(msg);
         type= NetWorkReceiver::AllExceptSender;
     }
     break;
     case NetMsg::ColorChanged:
     {
         m_vmap->processColorMsg(msg);
         type= NetWorkReceiver::AllExceptSender;
     }
     break;
     default:
         break;
     }*/

    return type;
}

void VMapFrame::fill(NetworkMessageWriter& msg)
{
    /* if(nullptr != m_graphicView)
     {
         if(msg.action() == NetMsg::addMedia)
         {
             QRectF rect= m_graphicView->sceneRect();
             msg.real(rect.x());
             msg.real(rect.y());
             msg.real(rect.width());
             msg.real(rect.height());

             m_vmap->fill(msg);
             m_vmap->sendAllItems(msg);
         }
     }*/
}

void VMapFrame::readMessage(NetworkMessageReader& msg)
{
    /*if(msg.action() == NetMsg::addMedia)
    {
        qreal x= msg.real();
        qreal y= msg.real();
        qreal w= msg.real();
        qreal h= msg.real();
        if(nullptr == m_vmap)
        {
            m_vmap= new VMap();
        }
        m_vmap->readMessage(msg);
        m_vmap->setOption(VisualItem::LocalIsGM, false);
        if(nullptr == m_graphicView)
        {
            createView();
            updateMap();
        }
        m_graphicView->setSceneRect(x, y, w, h);
    }*/
}

bool VMapFrame::readFileFromUri()
{
    return false;
    /* if(nullptr == m_uri || nullptr == m_vmap)

     bool read= false;
     if(!m_uri->exists()) // have not been saved outside story
     {
         QByteArray data= m_uri->getData();
         QDataStream in(&data, QIODevice::ReadOnly);
         in.setVersion(QDataStream::Qt_5_7);
         createView();
         m_vmap->openFile(in);
         m_vmap->setVisibilityMode(VMap::HIDDEN);
         updateMap();
         read= true;
     }
     else if(openFile(m_uri->getUri()))
     {
         read= true;
     }

     if(read)
     {
         NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::addVmap);
         fill(msg);
         m_vmap->sendAllItems(msg);
         msg.sendToServer();
     }*/
    // return read;
}

/*void VMapFrame::processAddItemMessage(NetworkMessageReader* msg)
{
    if(nullptr != m_vmap)
    {
        m_vmap->processAddItemMessage(msg);
    }
}
void VMapFrame::processMoveItemMessage(NetworkMessageReader* msg)
{
    if(nullptr != m_vmap)
    {
        m_vmap->processMoveItemMessage(msg);
    }
}
void VMapFrame::processOpacityMessage(NetworkMessageReader* msg)
{
    if(nullptr != m_vmap)
    {
        m_vmap->processOpacityMessage(msg);
    }
}
void VMapFrame::processLayerMessage(NetworkMessageReader* msg)
{
    if(nullptr != m_vmap)
    {
        m_vmap->processLayerMessage(msg);
    }
}
void VMapFrame::processGeometryChangeItem(NetworkMessageReader* msg)
{
    if(nullptr != m_vmap)
    {
        m_vmap->processGeometryChangeItem(msg);
    }
}
void VMapFrame::processMapPropertyChange(NetworkMessageReader* msg)
{
    if(nullptr != m_vmap)
    {
        m_vmap->readMessage(*msg, false);
    }
}
void VMapFrame::processGeometryViewChange(NetworkMessageReader* msg)
{
    if(nullptr != m_vmap)
    {
        m_graphicView->readMessage(msg);
    }
}
void VMapFrame::processDelItemMessage(NetworkMessageReader* msg)
{
    if(nullptr != m_vmap)
    {
        m_vmap->processDelItemMessage(msg);
    }
}
void VMapFrame::processSetParentItem(NetworkMessageReader* msg)
{
    if(nullptr != m_vmap)
    {
        m_vmap->processSetParentItem(msg);
    }
}


void VMapFrame::processsZValueMsg(NetworkMessageReader* msg)
{
    if(nullptr != m_vmap)
    {
        m_vmap->processZValueMsg(msg);
    }
}
void VMapFrame::processsRotationMsg(NetworkMessageReader* msg)
{
    if(nullptr != m_vmap)
    {
        m_vmap->processRotationMsg(msg);
    }
}
void VMapFrame::processsRectGeometryMsg(NetworkMessageReader* msg)
{
    if(nullptr != m_vmap)
    {
        m_vmap->processRectGeometryMsg(msg);
    }
}*/
