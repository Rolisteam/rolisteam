/*************************************************************************
 *    Copyright (C) 2007 by Romain Campioni                              *
 *    Copyright (C) 2009 by Renaud Guezennec                             *
 *    Copyright (C) 2011 by Joseph Boudou                                *
 *                                                                       *
 *      http://www.rolisteam.org/                                        *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/


#include "map/mapframe.h"
#include "userlist/playersList.h"
#include "data/person.h"
#include "data/player.h"
#include "map/newemptymapdialog.h"
#include "map/map.h"
#include "map/mapwizzard.h"

#include <QScrollBar>
#include <QMessageBox>
#include <QFileInfo>
#include <QBuffer>
#include <QUuid>
#include <QImage>

MapFrame::MapFrame(Map* map, QWidget *parent)
    : MediaContainer(parent),m_map(map),m_isHidden(false)
{
    setObjectName("MapFrame");
    m_widgetArea = new QScrollArea(this);
    setWindowIcon(QIcon(":/map.png"));
    setFocusPolicy(Qt::StrongFocus);
    m_widgetArea->setAlignment(Qt::AlignCenter);

    m_mapWizzard = new MapWizzard(false,this);

	initMap();

    setWidget(m_widgetArea);
}


MapFrame::~MapFrame()
{

}
void MapFrame::initMap()
{
	if(nullptr!=m_map)
	{
        //setTitle(m_title);
        updateTitle();
		m_originalSize = m_map->size();
        m_widgetArea->setWidget(m_map);
        //m_widgetArea->setViewportMargins(0,0,0,0);

		resize(m_map->width()+4, m_map->height()+4);

        connect(m_map, SIGNAL(startBipmapMove(QPoint)),
                this, SLOT(startMoving(QPoint)));
        connect(m_map, SIGNAL(moveBipMapWindow(QPoint)),
                this, SLOT(moveMap(QPoint)));
        connect(m_map,SIGNAL(permissionModeChanged()),this,SLOT(updateTitle()));
	}
}
void MapFrame::updateTitle()
{
    QString realTitle(tr("%1 - Permission: %2"));

    if(m_map->getPermissionMode()==Map::GM_ONLY)
    {
        realTitle= realTitle.arg(m_title).arg(tr("GM Only"));
    }
    else if(m_map->getPermissionMode()==Map::PC_ALL)
    {
        realTitle=realTitle.arg(m_title).arg(tr("All"));
    }
    else if(m_map->getPermissionMode()==Map::PC_MOVE)
    {
        realTitle=realTitle.arg(m_title).arg(tr("Pc Move"));
    }
    else
    {
        realTitle=realTitle.arg(m_title).arg(tr("Unknown"));
    }
    setTitle(realTitle);
}

Map* MapFrame::getMap()
{
	return m_map;
}
void MapFrame::setMap(Map* map)
{
	m_map = map;
	initMap();
}

void MapFrame::startMoving(QPoint position)
{
    pointDepart = position;
    horizontalDepart = m_widgetArea->horizontalScrollBar()->value();
    verticalDepart = m_widgetArea->verticalScrollBar()->value();
}

void MapFrame::moveMap(QPoint position)
{
    QPoint diff = pointDepart - position;
    m_widgetArea->horizontalScrollBar()->setValue(horizontalDepart + diff.x());
    m_widgetArea->verticalScrollBar()->setValue(verticalDepart + diff.y());
}
QString MapFrame::getMediaId()
{
	if(nullptr!=m_map)
    {
		return m_map->getMapId();
    }
    else
    {
        return QString();
    }
}
void MapFrame::focusInEvent(QFocusEvent * event)
{
	emit activated(m_map);

    MediaContainer::focusInEvent(event);
}
bool MapFrame::openMedia()
{
	m_mapWizzard->resetData();
	if(m_mapWizzard->exec()==QMessageBox::Accepted)
	{
		QFileInfo info(m_mapWizzard->getFilepath());

		m_preferences->registerValue("MapDirectory",info.absolutePath());
        return true;
	}
    return false;
}
bool MapFrame::openUriAndLoadMap(QString uri)
{
    QFile file(uri);
    if(!file.exists())
    {
        error(tr("File %1 does not exist").arg(m_uri->getUri()),this);
        return false;
    }
    if (!file.open(QIODevice::ReadOnly))
    {
        error(tr("Unsupported file format"),this);
        return false;
    }
    QDataStream in(&file);
    if(!readMapAndNpc(in, m_isHidden))
    {
        error(tr("Wrong Map Format"),this);
        return false;
    }
    file.close();
}

bool MapFrame::readFileFromUri()
{

    if((nullptr!=m_uri)&&(m_uri->hasData())&&(m_uri->exists()))
    {//load from uri data
        QByteArray array = m_uri->getData();
        QDataStream in(&array,QIODevice::ReadOnly);
        m_isHidden = false;
        readMapAndNpc(in,m_isHidden);
        m_title = m_uri->name();
    }
    else if((nullptr!=m_uri)&&(m_uri->exists())&&(m_uri->hasData()))
    {
        if(!openUriAndLoadMap(m_uri->getUri()))
        {
            return false;
        }
        m_title = m_uri->name();
    }
    else //if(nullptr==m_uri)
    {
        Map::PermissionMode Permission = m_mapWizzard->getPermissionMode();
        QString filepath = m_mapWizzard->getFilepath();
        m_title = m_mapWizzard->getTitle();

        m_isHidden = m_mapWizzard->getHidden();
        QFileInfo info(filepath);
        m_uri = new CleverURI(info.baseName(),filepath,CleverURI::MAP);

        if (filepath.endsWith(".pla"))
        {
            if(!openUriAndLoadMap(filepath))
            {
                return false;
            }
        }
        else
        {
            QString str("");
            m_uri->setUri(str);
            m_uri->setName(m_title);
            QImage image(filepath);
            if (image.isNull())
            {
                error(tr("Unsupported file format"),this);
                return false;
            }
            // Creation de l'identifiant
            QString idMap = QUuid::createUuid().toString();
            // Creation de la carte
            m_map = new Map(m_localPlayerId,idMap, &image, m_isHidden);
            m_map->setPermissionMode(Permission);


            //addMap(bipMapWindow, title);

            QByteArray byteArray;
            QBuffer buffer(&byteArray);
            bool ok = image.save(&buffer, "jpeg", 60);
            if (!ok)
            {
                error(tr("Compressing image goes wrong (ouvrirPlan - MainWindow.cpp)"),this);
                return false;
            }

            NetworkMessageWriter msg(NetMsg::MapCategory,NetMsg::LoadMap);
            msg.string16(m_title);
            msg.string8(idMap);
            msg.uint8(12);
            msg.uint8(m_map->getPermissionMode());
            msg.uint8(m_isHidden);
            msg.byteArray32(byteArray);
            msg.sendAll();

        }
    }
	initMap();
	return true;
}
bool MapFrame::readMapAndNpc(QDataStream &in, bool hidden)
{
    in >> m_title;

	QPoint pos;
	in >>pos;

	Map::PermissionMode myPermission;
	quint32 permiInt;
	in >> permiInt;
	myPermission = (Map::PermissionMode) permiInt;

	QSize size;
	in >> size;

	int taillePj;
	in >> taillePj;

	QByteArray baFondOriginal;
	in >> baFondOriginal;

	QByteArray baFond;
	in >> baFond;

	QByteArray baAlpha;
	in>> baAlpha;

	bool ok;


	//////////////////
	// Manage Data to create the map.
	//////////////////

	QImage fondOriginal;
	ok = fondOriginal.loadFromData(baFondOriginal, "jpeg");
	if (!ok)
	{
        error(tr("Extract original background information Failed (readMapAndNpc - bipmapwindow.cpp)"),this);
		return false;
	}

	QImage fond;
	ok = fond.loadFromData(baFond, "jpeg");
	if (!ok)
	{
        error(tr("Extract background information Failed (readMapAndNpc - bipmapwindow.cpp)"),this);
		return false;
	}

	QImage alpha;
	ok = alpha.loadFromData(baAlpha, "jpeg");
	if (!ok)
	{
        error(tr("Extract alpha layer information Failed (readMapAndNpc - bipmapwindow.cpp)"),this);
		return false;
	}

	if (hidden)
	{
		QPainter painterAlpha(&alpha);
        QColor color = PreferencesManager::getInstance()->value("Fog_color",QColor(5,5,5)).value<QColor>();
		painterAlpha.fillRect(0, 0, alpha.width(), alpha.height(), color);
	}

	QString idCarte = QUuid::createUuid().toString();

	m_map = new Map(m_localPlayerId,idCarte, &fondOriginal, &fond, &alpha);
	m_map->setPermissionMode(myPermission);
	//m_map->setPointeur(m_toolBar->getCurrentTool());

    initMap();
	QPoint pos2 = mapFromParent(pos);

	quint16 nbrPersonnages;
	in >>  nbrPersonnages;

	for (int i=0; i<nbrPersonnages; ++i)
	{
		QString nomPerso,ident;
		CharacterToken::typePersonnage type;
		int numeroDuPnj;
		uchar diametre;

		QColor couleur;
		CharacterToken::StateOfHealth sante;

		QPoint centre;
		QPoint orientation;
		int numeroEtat;
		bool visible;
		bool orientationAffichee;

		in >> nomPerso;
		in >> ident ;
		int typeint;
		in >> typeint;
		type=(CharacterToken::typePersonnage) typeint;
		in >> numeroDuPnj ;
		in >> diametre;
		in >> couleur ;
		in >> centre ;
		in >> orientation ;
		in >>sante.stateColor ;
		in >> sante.stateName ;
		in >> numeroEtat ;
		in>> visible;
		in >> orientationAffichee;

		bool showNumber=true;//(type == DessinPerso::pnj)?m_ui->m_showNpcNumberAction->isChecked():false;
		bool showName=true;//(type == DessinPerso::pnj)? m_ui->m_showNpcNameAction->isChecked():m_ui->m_showPcNameAction->isChecked();

		CharacterToken *pnj = new CharacterToken(m_map, ident, nomPerso, couleur, diametre, centre, type,showNumber,showName, numeroDuPnj);

        if (visible || (type == CharacterToken::pnj && PlayersList::instance()->getLocalPlayer()->isGM()))
			pnj->showCharacter();
		// On m.a.j l'orientation
		pnj->newOrientation(orientation);
		// Affichage de l'orientation si besoin
		if (orientationAffichee)
			pnj->showOrHideOrientation();

		pnj->newHealtState(sante, numeroEtat);

		m_map->showHideNPC(pnj);

	}
	m_map->sendMap(windowTitle());
	m_map->sendOffAllCharacters();

	return true;
}

bool MapFrame::createMap()
{
    NewEmptyMapDialog mapDialog;
    if(mapDialog.exec()==QMessageBox::Accepted)
    {
        QString idMap = QUuid::createUuid().toString();

        QImage image(mapDialog.getSize(), QImage::Format_ARGB32_Premultiplied);
        image.fill(mapDialog.getColor().rgb());

        m_map = new Map(m_localPlayerId,idMap, &image);
        m_map->setPermissionMode(mapDialog.getPermission());

        m_title = mapDialog.getTitle();

        m_color = mapDialog.getColor();
        m_width = mapDialog.getSize().width();
        m_height = mapDialog.getSize().height();

        setCleverUriType(CleverURI::MAP);



        initMap();
        return true;
    }
    return false;
}
bool MapFrame::processMapMessage(NetworkMessageReader* msg,bool localIsPlayer)
{
    m_title = msg->string16();
    QString idMap = msg->string8();
    setTitle(m_title);


    if(msg->action() == NetMsg::LoadMap)
    {
        quint8 npSize = msg->uint8();
        quint8 permission = msg->uint8();
        quint8 maskPlan = msg->uint8();
        QByteArray mapData = msg->byteArray32();
        QImage image;
        if (! image.loadFromData(mapData, "jpeg"))
        {
            error(tr("Compression Error (processMapMessage - NetworkLink.cpp)"),this);
            return false;
        }
        m_map= new Map(m_localPlayerId,idMap, &image, maskPlan);
        m_map->setLocalIsPlayer(localIsPlayer);
        m_map->changePjSize(npSize,false);
        m_map->setPermissionMode((Map::PermissionMode)permission);
        emit notifyUser(tr("Receiving map: %1").arg(m_title));
    }
    else if(msg->action() == NetMsg::ImportMap)
    {
        quint8 npSize = msg->uint8();
        quint8 permission = msg->uint8();
        quint8 alphaValue = msg->uint8();
        QByteArray originBackground = msg->byteArray32();
        QByteArray background = msg->byteArray32();
        QByteArray bgAlpha = msg->byteArray32();

        QImage originalBackgroundImage;
        if (!originalBackgroundImage.loadFromData(originBackground, "jpeg"))
        {
            error(tr("Extract original background information Failed (processMapMessage - mainwindow.cpp)"),this);
            return false;
        }
        // Creation de l'image de fond
        QImage backgroundImage;
        if (!backgroundImage.loadFromData(background, "jpeg"))
        {
            error(tr("Extract background information Failed (processMapMessage - mainwindow.cpp)"),this);
            return false;
        }
        // Creation de la couche alpha
        QImage alphaImage;
        if (!alphaImage.loadFromData(bgAlpha, "jpeg"))
        {
            error(tr("Extract alpha layer information Failed (processMapMessage - mainwindow.cpp)"),this);
            return false;
        }

        m_map = new Map(m_localPlayerId,idMap, &originalBackgroundImage, &backgroundImage, &alphaImage);
        m_map->setLocalIsPlayer(localIsPlayer);
        m_map->changePjSize(npSize,false);

        m_map->setPermissionMode((Map::PermissionMode)permission);
        if(localIsPlayer)
        {
            m_map->adaptAlphaLayer(255);
        }
        else
        {
            QColor color = m_preferences->value("Fog_color",QColor(50,50,50)).value<QColor>();
            m_map->adaptAlphaLayer(color.red());
        }

        emit notifyUser(tr("Receiving map: %1").arg(m_title));

    }
    initMap();
    return true;
}
void MapFrame::saveMedia()
{
    if(nullptr!=m_map)
    {
        if(nullptr!=m_uri)
        {
            if(!m_uri->getUri().endsWith(".pla"))
            {
                QString uri = m_uri->getUri()+".pla";
                m_uri->setUri(uri);
            }

            QFile file(m_uri->getUri());
            if (!file.open(QIODevice::WriteOnly))
            {
                notifyUser("could not open file for writting (saveMap - MapFrame.cpp)");
                return;
            }
            QDataStream out(&file);
            m_map->saveMap(out);
            file.close();
        }
    }
}

void MapFrame::putDataIntoCleverUri()
{
    if(nullptr!=m_map)
    {
        QByteArray data;
        QDataStream out(&data,QIODevice::WriteOnly);
        //out << m_isHidden;
        m_map->saveMap(out);
        if(nullptr!=m_uri)
        {
            m_uri->setData(data);
        }
    }
}

void MapFrame::fill(NetworkMessageWriter &msg)
{
    if(nullptr != m_map)
    {
        //NetworkMessageWriter msg(NetMsg::MapCategory,NetMsg::AddEmptyMap);
        msg.string16(m_title);
        msg.string8(m_map->getMapId());
        msg.rgb(m_color);
        msg.uint16(m_width);
        msg.uint16(m_height);
        msg.uint8(12);
        msg.uint8(static_cast<quint8>(m_map->getPermissionMode()));
    }
}

void MapFrame::readMessage(NetworkMessageReader &msg)
{
    //if(msg.action() == NetMsg::AddEmptyMap)
    {
        m_title = msg.string16();
        QString idMap = msg.string8();
        setTitle(m_title);
        m_color = msg.rgb();
        m_width = msg.uint16();
        m_height = msg.uint16();
        quint8 npSize = msg.uint8();
        quint8 permission = msg.uint8();
        QSize mapSize(m_width,m_height);

        QImage image(mapSize, QImage::Format_ARGB32_Premultiplied);
        image.fill(m_color.rgb());

        m_map = new Map(m_localPlayerId,idMap, &image);
        m_map->setLocalIsPlayer(true);
        m_map->setPermissionMode(static_cast<Map::PermissionMode>(permission));
        m_map->changePjSize(npSize,false);

        emit notifyUser(tr("New map: %1").arg(m_title));

        initMap();
    }
}
