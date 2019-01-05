/*************************************************************************
 *   Copyright (C) 2007 by Romain Campioni                               *
 *   Copyright (C) 2009 by Renaud Guezennec                              *
 *   Copyright (C) 2011 by Joseph Boudou                                 *
 *                                                                       *
 *     http://www.rolisteam.org/                                         *
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

// clazy:skip
#include "map/map.h"

#include <QMessageBox>
#include <QUuid>
#include <QBuffer>

#include "network/networkmessagewriter.h"
#include "map/charactertoken.h"
#include "network/networklink.h"
#include "data/person.h"
#include "data/player.h"
#include "data/character.h"
#include "userlist/playersList.h"

//#include "variablesGlobales.h"
#include <QDebug>
#include "preferences/preferencesmanager.h"



Map::Map(QString localPlayerId,QString identCarte, QImage *image, bool masquer, QWidget *parent)
    : QWidget(parent), m_mapId(identCarte),m_hasPermissionMode(true)
{
    m_localPlayerId = localPlayerId;
    if(nullptr!=PlayersList::instance()->getLocalPlayer())
    {
        m_localIsPlayer = !PlayersList::instance()->getLocalPlayer()->isGM();
    }
    else
    {
        m_localIsPlayer = true;
    }
    m_currentMode = Map::GM_ONLY;
    m_currentTool = ToolsBar::Handler;

    m_originalBackground = new QImage(image->size(), QImage::Format_ARGB32);
    *m_originalBackground = image->convertToFormat(QImage::Format_ARGB32);
    

    m_backgroundImage = new QImage(image->size(), QImage::Format_ARGB32_Premultiplied);
    *m_backgroundImage = image->convertToFormat(QImage::Format_ARGB32_Premultiplied);

    m_alphaLayer = new QImage(image->size(), QImage::Format_ARGB32_Premultiplied);

    QPainter painterAlpha(m_alphaLayer);
    painterAlpha.fillRect(0, 0, image->width(), image->height(),masquer?getFogColor():Qt::white);

    p_init();
}
Map::Map(QString localPlayerId,QString identCarte, QImage *original, QImage *avecAnnotations, QImage *coucheAlpha, QWidget *parent)
    : QWidget(parent), m_mapId(identCarte),m_hasPermissionMode(true)
{
    m_localPlayerId = localPlayerId;


    m_currentMode = Map::GM_ONLY;

    m_originalBackground = new QImage(original->size(), QImage::Format_ARGB32);
    *m_originalBackground = original->convertToFormat(QImage::Format_ARGB32);

    m_backgroundImage = new QImage(avecAnnotations->size(), QImage::Format_ARGB32_Premultiplied);
    *m_backgroundImage = avecAnnotations->convertToFormat(QImage::Format_ARGB32_Premultiplied);

    m_alphaLayer = new QImage(coucheAlpha->size(), QImage::Format_ARGB32_Premultiplied);
    *m_alphaLayer = coucheAlpha->convertToFormat(QImage::Format_ARGB32_Premultiplied);

    p_init();
}

void Map::p_init()
{
    initCursor();
    m_showPcName = true;
    m_eraseAlpha = new QImage(m_originalBackground->size(), QImage::Format_ARGB32_Premultiplied);
    QPainter painterEfface(m_eraseAlpha);
    painterEfface.fillRect(0, 0, m_originalBackground->width(), m_originalBackground->height(), Qt::black);
	addAlphaLayer(m_originalBackground, m_eraseAlpha, m_originalBackground);

    m_localPlayer = PlayersList::instance()->getLocalPlayer();

    m_alphaBg = new QImage(m_originalBackground->size(), QImage::Format_ARGB32);

    addAlphaLayer(m_backgroundImage, m_alphaLayer, m_alphaBg);

    QColor color(m_alphaBg->pixel(10,10));
    QColor color2(m_alphaLayer->pixel(10,10));


    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, PreferencesManager::getInstance()->value("Mask_color",QColor(Qt::darkMagenta)).value<QColor>());
    setPalette(pal);
    
    m_npcSize = 12;
    m_leftButtonStatus = false;
    m_rightButtonStatus = false;
    m_mousePoint = QPoint(0,0);
    m_originPoint = QPoint(0,0);
    m_distanceBetweenMouseAndNPC = QPoint(0,0);
    m_selectedNpc = 0;
    m_lastSelectedNpc = 0;

    resize(m_backgroundImage->size());
    m_penPointList.clear();
    m_characterMoveList.clear();
    m_penGlobalZone.setRect(0,0,0,0);

    m_motionList.clear();

    // Get every characters
    PlayersList* playersList = PlayersList::instance();
    int maxPlayersIndex = playersList->getPlayerCount();
    for (int i = 0 ; i < maxPlayersIndex ; i++)
    {
        Player * player = playersList->getPlayer(i);
        int maxCharactersIndex = player->getChildrenCount();
        for (int j = 0 ; j < maxCharactersIndex ; j++)
        {
            addCharacter(player->getCharacterByIndex(j));
        }
    }

    // connect to g_playesList to stay tuned
    connect(playersList, SIGNAL(characterAdded(Character *)),
            this, SLOT(addCharacter(Character *)));
    connect(playersList, SIGNAL(characterDeleted(Character *)),
            this, SLOT(delCharacter(Character *)));
    connect(playersList, SIGNAL(characterChanged(Character *)),
            this, SLOT(changeCharacter(Character *)));
}



Map::~Map()
{
    delete m_orientCursor;
    delete m_pipetteCursor;
    delete m_addCursor;
    delete m_delCursor;
    delete m_movCursor;
    delete m_textCursor;
    delete m_pencilCursor;
    delete m_stateCursor;
    delete m_originalBackground;
    delete m_backgroundImage;
    delete m_alphaLayer;
    delete m_eraseAlpha;
    delete m_alphaBg;
}

void Map::initCursor()
{
    m_orientCursor  = new QCursor(QPixmap(":/resources/icons/orientationcursor.png"), 10, 12);
    m_pipetteCursor   = new QCursor(QPixmap(":/resources/icons/pipettecursor.png"), 1, 19);
    m_addCursor= new QCursor(QPixmap(":/resources/icons/addcursor.png"), 6, 0);
    m_delCursor = new QCursor(QPixmap(":/resources/icons/deletecursor.png"), 6, 0);
    m_movCursor= new QCursor(QPixmap(":/resources/icons/movecursor.png"), 0, 0);
    m_textCursor= new QCursor(QPixmap(":/resources/icons/textcursor.png"), 4, 13);
    m_pencilCursor = new QCursor(QPixmap(":/resources/icons/paintcursor.png"), 8, 8);
    m_stateCursor = new QCursor(QPixmap(":/resources/icons/statecursor.png"), 0, 0);
}

void Map::paintEvent(QPaintEvent *event)
{

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);  

    painter.drawImage(event->rect(), *m_alphaBg, event->rect());

    //painter.drawImage(event->rect(), *fondAlpha, event->rect());
    painter.drawImage(rect(), *m_alphaBg, m_alphaBg->rect());

    if (m_leftButtonStatus == false)
        return;

    if (m_currentTool == ToolsBar::AddNpc || m_currentTool == ToolsBar::DelNpc ||
        m_currentTool == ToolsBar::MoveCharacterToken || m_currentTool == ToolsBar::ChangeCharacterState)
        return;
        
    paintMap(painter);


   // painter.drawImage(rect(), *m_alphaLayer, m_alphaLayer->rect());

    /*painter.setBrush(Qt::black);
    painter.fillRect(m_refreshZone,Qt::black);*/
}
QPoint Map::mapToScale(const QPoint & p)
{
    QPoint tmp = p;
    m_scaleX=(qreal)m_alphaBg->rect().width()/rect().width();
    m_scaleY=(qreal)m_alphaBg->rect().height()/rect().height();
    tmp.setX(tmp.x()*m_scaleX);
    tmp.setY(tmp.y()*m_scaleY);




    return tmp;
}
QPoint Map::mapToNormal(const QPoint & p)
{
    QPoint tmp;
   /* m_origineScalePoint = m_scalePoint;
    m_scalePoint = p;*/
    tmp.setX(p.x()/m_scaleX);
    tmp.setY(p.y()/m_scaleY);
    return tmp;
}
void Map::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = mapToScale(event->pos());


    if ((event->button() == Qt::LeftButton) && !m_leftButtonStatus && !m_rightButtonStatus)
    {

            m_leftButtonStatus = true;



            if (m_currentTool == ToolsBar::AddNpc || m_currentTool == ToolsBar::DelNpc ||
                m_currentTool == ToolsBar::MoveCharacterToken || m_currentTool == ToolsBar::ChangeCharacterState)
            {
				if((!m_localIsPlayer)||(Map::PC_ALL==m_currentMode)||(Map::PC_MOVE == m_currentMode))
                {
					processNpcAction(pos);
                }
            }
            else if (m_currentTool == ToolsBar::Handler)
            {
                // On initialise le deplacement de la Carte
				//if((!m_localIsPlayer)||
               //   (((m_currentMode == Carte::PC_ALL))))
                {
                    emit startBipmapMove(mapToGlobal(pos));
                }

        }
        else
        {
				if(((!m_localIsPlayer))||
                  (((m_currentMode == Map::PC_ALL))))
                {
                        m_originPoint = m_mousePoint = pos;
                        m_origZone = m_newZone = zoneToRefresh();
                        m_penPointList.clear();
                        m_penGlobalZone = m_newZone;
                        update();
                }
        }
    }
    else if ((event->button() == Qt::RightButton) && !m_leftButtonStatus && !m_rightButtonStatus)
    {

        QPoint positionSouris = pos;
        
        if (m_currentTool == ToolsBar::AddNpc || m_currentTool == ToolsBar::DelNpc)
        {
            m_rightButtonStatus = true;

            setCursor(Qt::WhatsThisCursor);
			CharacterToken *pnj = paintCharacter(positionSouris);
            if (pnj)
            {
                int diametre;
                QColor couleur;
                QString nomPnj;

                pnj->defineToken(&diametre, &couleur, &nomPnj);
                emit updateNPC(diametre, nomPnj);
                emit changeCurrentColor(couleur);
            }
        }
        else if (m_currentTool == ToolsBar::MoveCharacterToken || m_currentTool == ToolsBar::ChangeCharacterState)
        {
            m_rightButtonStatus = true;
            setCursor(*m_orientCursor);
            CharacterToken *npc = paintCharacter(positionSouris);
            if (npc)
            {
                npc->showOrHideOrientation();
                m_lastSelectedNpc = npc;

                NetworkMessageWriter msg(NetMsg::CharacterCategory,NetMsg::showCharecterOrientation);
                msg.string8(m_mapId);
                msg.string8(npc->getCharacterId());
                msg.uint8(npc->orientationStatus());
                msg.sendToServer();
            }
            else if (m_lastSelectedNpc)
            {
                m_rightButtonStatus = true;
                m_lastSelectedNpc->drawCharacter(positionSouris);
            }
        }
        else if(event->modifiers()==Qt::ControlModifier)
        {
            m_rightButtonStatus = true;
            setCursor(*m_pipetteCursor);
            QColor couleur = QColor(m_backgroundImage->pixel(positionSouris.x(), positionSouris.y()));
            emit changeCurrentColor(couleur);
        }
    }
}


void Map::mouseReleaseEvent(QMouseEvent *event)
{
      QPoint pos = mapToScale(event->pos());

    if (event->button() == Qt::LeftButton && m_leftButtonStatus)
    {
        m_leftButtonStatus = false;

        if (m_currentTool == ToolsBar::AddNpc || m_currentTool == ToolsBar::DelNpc ||
            m_currentTool == ToolsBar::MoveCharacterToken || m_currentTool == ToolsBar::ChangeCharacterState)
        {
			processNpcActionReleased(pos);
        }

        else if (m_currentTool == ToolsBar::Handler)
        {
            //nothing
        }
        else
        {
            /// @warning deadcode
            if (!m_localIsPlayer)//PreferencesManager::getInstance()->value("isClient",true).toBool()
            {
                QPainter painter;
        
                if (ColorSelector::getSelectedColor().type == ColorType)
                    painter.begin(m_backgroundImage);
                else if (ColorSelector::getSelectedColor().type == Veil || ColorSelector::getSelectedColor().type == Unveil)
                    painter.begin(m_alphaLayer);
                else if (ColorSelector::getSelectedColor().type == Erase)
                    painter.begin(m_eraseAlpha);
                else
                {
                    painter.begin(m_backgroundImage);
                }
                painter.setRenderHint(QPainter::Antialiasing);
                
				paintMap(painter);
            }

			if(((!m_localIsPlayer))||
              (((m_currentMode == Map::PC_ALL))))
            {
                 m_mousePoint = pos;
                m_newZone = zoneToRefresh();
            }
            /// NOTE @warning deadcode
           if (!m_localIsPlayer)//!PreferencesManager::getInstance()->value("isClient",true).toBool()
            {
                if (ColorSelector::getSelectedColor().type == Erase)
                {
                    addAlphaLayer(m_originalBackground, m_eraseAlpha, m_originalBackground, m_newZone);
                    QPainter painterFond(m_backgroundImage);
                    painterFond.drawImage(m_newZone, *m_originalBackground, m_newZone);
                    QPainter painterEfface(m_eraseAlpha);
                    painterEfface.fillRect(m_newZone, Qt::black);
                }

                addAlphaLayer(m_backgroundImage, m_alphaLayer, m_alphaBg, m_newZone);
                update();//zoneOrigine.unite(zoneNouvelle)
                showHideNPC();
            }
			if((!m_localIsPlayer)||
              (((m_currentMode == Map::PC_ALL))))
            {
                sendTrace();
            }

        }        //
    }            //
    else if (event->button() == Qt::RightButton && m_rightButtonStatus)
    {
        m_rightButtonStatus = false;
        setCursor(m_mouseCursor);

        if ((m_currentTool == ToolsBar::MoveCharacterToken || m_currentTool == ToolsBar::ChangeCharacterState) && m_lastSelectedNpc)
        {///@todo test Npc orientation
            QPoint orientation =  m_lastSelectedNpc->getCharacterOrientation();
            NetworkMessageWriter msg(NetMsg::CharacterCategory,NetMsg::changeCharacterOrientation);
            msg.string8(m_mapId);
            msg.string8(m_lastSelectedNpc->getCharacterId());
            msg.int16(orientation.x());
            msg.int16(orientation.y());
            msg.sendToServer();
        }
    }
}


void Map::mouseMoveEvent(QMouseEvent *event)
{
      QPoint pos = mapToScale(event->pos());

    if (m_leftButtonStatus && !m_rightButtonStatus)
    {
        if (m_currentTool == ToolsBar::AddNpc || m_currentTool == ToolsBar::DelNpc ||
            m_currentTool == ToolsBar::MoveCharacterToken || m_currentTool == ToolsBar::ChangeCharacterState)
        {
			processNpcMove(pos);
        }
        else if (m_currentTool == ToolsBar::Handler)
        {
            emit moveBipMapWindow(mapToGlobal(pos));
        }
        else
        {
			if((!m_localIsPlayer)||
              (((m_currentMode == Map::PC_ALL))))
            {
                m_mousePoint = pos;

                m_newZone = zoneToRefresh();
                update();
                m_origZone = m_newZone;
            }
        }
    }
    else if (m_rightButtonStatus && !m_leftButtonStatus && (m_backgroundImage->rect()).contains(pos))
    {
        QPoint positionSouris = pos;
        
        if (m_currentTool == ToolsBar::AddNpc || m_currentTool == ToolsBar::DelNpc)
        {
        }
        else if (m_currentTool == ToolsBar::MoveCharacterToken || m_currentTool == ToolsBar::ChangeCharacterState)
        {
			if (m_lastSelectedNpc && !paintCharacter(positionSouris))
                m_lastSelectedNpc->drawCharacter(positionSouris);
        }
        else
        {
            QColor couleur = QColor(m_backgroundImage->pixel(positionSouris.x(), positionSouris.y()));
            emit changeCurrentColor(couleur);
        }
    }
}


void Map::paintMap(QPainter &painter)
{
    QColor couleurPinceau;
    

    if (ColorSelector::getSelectedColor().type == ColorType)
        couleurPinceau = ColorSelector::getSelectedColor().color;
    else if (ColorSelector::getSelectedColor().type == Veil)
    {
        couleurPinceau = getFogColor();
    }
    else if(ColorSelector::getSelectedColor().type == Unveil)
        couleurPinceau = Qt::white;
    else if(ColorSelector::getSelectedColor().type == Erase)
        couleurPinceau = Qt::white;
    else
		qWarning() << tr("color type not allowed (paintMap - map.cpp)");


    QPen pen;
	pen.setWidth(m_penSize);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setColor(couleurPinceau);
    painter.setPen(pen);


    if (m_currentTool == ToolsBar::Pen)
    {

        QPainter painterCrayon;
        
        if (ColorSelector::getSelectedColor().type == ColorType)
            painterCrayon.begin(m_backgroundImage);
        else if (ColorSelector::getSelectedColor().type == Veil || ColorSelector::getSelectedColor().type == Unveil)
            painterCrayon.begin(m_alphaLayer);
        else if (ColorSelector::getSelectedColor().type == Erase)
            painterCrayon.begin(m_eraseAlpha);
        else
        {
            painter.begin(m_backgroundImage);
			qWarning() << tr("color type not allowed  (paintMap - map.cpp)");
        }
        painterCrayon.setRenderHint(QPainter::Antialiasing);
        
        QPen pen;
        pen.setColor(couleurPinceau);
		pen.setWidth(m_penSize);
        pen.setCapStyle(Qt::RoundCap);
        painterCrayon.setPen(pen);
        painter.setPen(pen);
        painterCrayon.drawLine(m_originPoint, m_mousePoint);
        painter.drawLine(m_originPoint, m_mousePoint);

        painter.drawPoint(m_mousePoint);
        
        if (ColorSelector::getSelectedColor().type == Erase)
        {
			addAlphaLayer(m_originalBackground, m_eraseAlpha, m_originalBackground, m_newZone);
            QPainter painterFond(m_backgroundImage);
            painterFond.drawImage(m_newZone, *m_originalBackground, m_newZone);
            painterCrayon.fillRect(m_newZone, Qt::black);
        }
        
		addAlphaLayer(m_backgroundImage, m_alphaLayer, m_alphaBg, m_newZone);
        m_penPointList.append(m_mousePoint);
        m_penGlobalZone = m_penGlobalZone.united(m_newZone);
        m_originPoint = m_mousePoint;
    }
    
    else if (m_currentTool == ToolsBar::Line)
    {
        painter.drawLine(m_originPoint, m_mousePoint);
    }
    
    else if (m_currentTool == ToolsBar::EmptyRect)
    {
        QPoint supGauche, infDroit;
        supGauche.setX(m_originPoint.x()<m_mousePoint.x()?m_originPoint.x():m_mousePoint.x());
        supGauche.setY(m_originPoint.y()<m_mousePoint.y()?m_originPoint.y():m_mousePoint.y());
        infDroit.setX(m_originPoint.x()>m_mousePoint.x()?m_originPoint.x():m_mousePoint.x());
        infDroit.setY(m_originPoint.y()>m_mousePoint.y()?m_originPoint.y():m_mousePoint.y());

		if ((infDroit.x()-supGauche.x() < m_penSize) && (infDroit.y()-supGauche.y() < m_penSize))
        {
			QPoint delta(m_penSize/2, m_penSize/2);
            painter.fillRect(QRect(supGauche - delta, infDroit + delta), couleurPinceau);
        }
        else
            painter.drawRect(QRect(supGauche, infDroit));
    }
    
    else if (m_currentTool == ToolsBar::FilledRect)
    {
        painter.fillRect(QRect(m_originPoint, m_mousePoint), couleurPinceau);
    }
    else if (m_currentTool == ToolsBar::EmptyEllipse)
    {
        QPoint diff = m_mousePoint - m_originPoint;
        QPoint nouveauPointOrigine = m_originPoint - diff;

		if (abs(m_mousePoint.x()-nouveauPointOrigine.x()) < m_penSize && abs(m_mousePoint.y()-nouveauPointOrigine.y()) < m_penSize)
        {
            QPoint supGauche, infDroit;
            supGauche.setX(nouveauPointOrigine.x()<m_mousePoint.x()?nouveauPointOrigine.x():m_mousePoint.x());
            supGauche.setY(nouveauPointOrigine.y()<m_mousePoint.y()?nouveauPointOrigine.y():m_mousePoint.y());
            infDroit.setX(nouveauPointOrigine.x()>m_mousePoint.x()?nouveauPointOrigine.x():m_mousePoint.x());
            infDroit.setY(nouveauPointOrigine.y()>m_mousePoint.y()?nouveauPointOrigine.y():m_mousePoint.y());
            
            QPen pen;
            pen.setColor(couleurPinceau);
            pen.setWidth(0);
            painter.setPen(pen);
            painter.setBrush(couleurPinceau);
			QPoint delta(m_penSize/2, m_penSize/2);
            painter.drawEllipse(QRect(supGauche - delta, infDroit + delta));
        }
        else
            painter.drawEllipse(QRect(nouveauPointOrigine, m_mousePoint));
    }
    
    else if (m_currentTool == ToolsBar::FilledEllipse)
    {
        QPoint diff = m_mousePoint - m_originPoint;
        QPoint nouveauPointOrigine = m_originPoint - diff;
        QPen pen;
        pen.setColor(couleurPinceau);
        pen.setWidth(0);
        painter.setPen(pen);
        painter.setBrush(couleurPinceau);
        painter.drawEllipse(QRect(nouveauPointOrigine, m_mousePoint));
    }
    
    else if (m_currentTool == ToolsBar::Text)
    {
        QFont font;
        font.setPixelSize(16);
        painter.setFont(font);
		painter.drawText(m_mousePoint, m_currentText);  // + QPoint(2,7)
    }
    
    else if (m_currentTool == ToolsBar::Handler)
    {
    }
            
    else
        qWarning() << tr("undefined drawing tools (paintMap - map.cpp)");
}

QRect Map::zoneToRefresh()
{
    QRect resultat;

    int gauche = m_originPoint.x()<m_mousePoint.x()?m_originPoint.x():m_mousePoint.x();
    int haut = m_originPoint.y()<m_mousePoint.y()?m_originPoint.y():m_mousePoint.y();
    int droit = m_originPoint.x()>m_mousePoint.x()?m_originPoint.x():m_mousePoint.x();
    int bas = m_originPoint.y()>m_mousePoint.y()?m_originPoint.y():m_mousePoint.y();
    int largeur = droit - gauche + 1;
    int hauteur = bas - haut + 1;

    if (m_currentTool == ToolsBar::Pen)
    {
		resultat = QRect(gauche-m_penSize/2-2, haut-m_penSize/2-2, largeur+m_penSize+4, hauteur+m_penSize+4);
    }
    
    else if (m_currentTool == ToolsBar::Line)
    {
		resultat = QRect(gauche-m_penSize, haut-m_penSize, largeur+m_penSize*2, hauteur+m_penSize*2);
    }
    
    else if (m_currentTool == ToolsBar::EmptyRect)
    {
		resultat = QRect(gauche-m_penSize/2-2, haut-m_penSize/2-2, largeur+m_penSize+4, hauteur+m_penSize+4);
    }
    
    else if (m_currentTool == ToolsBar::FilledRect)
    {
        resultat = QRect(gauche-2, haut-2, largeur+4, hauteur+4);
    }
    
    else if (m_currentTool == ToolsBar::EmptyEllipse)
    {
        QPoint diff = m_mousePoint - m_originPoint;
        QPoint nouveauPointOrigine = m_originPoint - diff;
        
        int gauche = nouveauPointOrigine.x()<m_mousePoint.x()?nouveauPointOrigine.x():m_mousePoint.x();
        int haut = nouveauPointOrigine.y()<m_mousePoint.y()?nouveauPointOrigine.y():m_mousePoint.y();
        int droit = nouveauPointOrigine.x()>m_mousePoint.x()?nouveauPointOrigine.x():m_mousePoint.x();
        int bas = nouveauPointOrigine.y()>m_mousePoint.y()?nouveauPointOrigine.y():m_mousePoint.y();
        int largeur = droit - gauche + 1;
        int hauteur = bas - haut + 1;

		resultat = QRect(gauche-m_penSize/2-2, haut-m_penSize/2-2, largeur+m_penSize+4, hauteur+m_penSize+4);
    }
    
    else if (m_currentTool == ToolsBar::FilledEllipse)
    {
        QPoint diff = m_mousePoint - m_originPoint;
        QPoint nouveauPointOrigine = m_originPoint - diff;
        
        int gauche = nouveauPointOrigine.x()<m_mousePoint.x()?nouveauPointOrigine.x():m_mousePoint.x();
        int haut = nouveauPointOrigine.y()<m_mousePoint.y()?nouveauPointOrigine.y():m_mousePoint.y();
        int droit = nouveauPointOrigine.x()>m_mousePoint.x()?nouveauPointOrigine.x():m_mousePoint.x();
        int bas = nouveauPointOrigine.y()>m_mousePoint.y()?nouveauPointOrigine.y():m_mousePoint.y();
        int largeur = droit - gauche + 1;
        int hauteur = bas - haut + 1;

        resultat = QRect(gauche-2, haut-2, largeur+4, hauteur+4);
    }
    
    else if (m_currentTool == ToolsBar::Text)
    {
        resultat = QRect(QPoint(m_mousePoint.x()-2, m_mousePoint.y()-15), QPoint(m_backgroundImage->width(), m_mousePoint.y()+4));
    }
    
    else if (m_currentTool == ToolsBar::Handler)
    {

    } 
    else
		qWarning() <<  (tr("Undefined tool  (paintMap - map.cpp)"));

    /*m_originePoint=m_origineScalePoint;
    m_mousePoint=m_scalePoint;*/
    /*m_originePoint=mapToScale(m_originePoint);
    m_mousePoint=mapToScale(m_mousePoint);*/

    m_refreshZone = resultat.intersected(m_backgroundImage->rect());
    return m_refreshZone;//rect();
}


bool Map::addAlphaLayer(QImage *source, QImage *alpha, QImage *destination, const QRect &rect)
{
    if (source->size() != destination->size() || source->size() != alpha->size())
    {
		qWarning() << (tr("Source, destination and alpha layer have not the same size  (addAlphaLayer - map.cpp)"));
        return false;
    }

    QRect zone = rect;
    if (rect.isNull())
    {
        zone = QRect(0, 0, source->width(), source->height());
    }
    if((zone.height() > source->height())||(zone.width() > source->width()))
    {
        zone.setWidth(source->width());
        zone.setHeight(source->height());
    }

    int finX = zone.left() + zone.width();
    int finY = (zone.top() + zone.height()) * source->width();

    QRgb *pixelSource = (QRgb *)source->bits();
    QRgb *pixelDestination = (QRgb *)destination->bits();
    QRgb *pixelAlpha = (QRgb *)alpha->bits();

    int x, y;
    for (y=zone.top()*source->width(); y<finY; y+=source->width())
    {
        for (x=zone.left(); x<finX; x++)
        {
            pixelDestination[x+y] = (pixelSource[x+y] & 0x00FFFFFF) | ((pixelAlpha[x+y] & 0x00FF0000) << 8);
        }
    }
    return true;
}


void Map::processNpcAction(QPoint positionSouris)
{
    if (m_currentTool == ToolsBar::AddNpc)
    {
		if((!m_localIsPlayer)||(Map::PC_ALL==m_currentMode))
        {
            if (ColorSelector::getSelectedColor().type == ColorType)
            {
                QString idPnj = QUuid::createUuid().toString();
                CharacterToken *pnj = new CharacterToken(this, idPnj, m_currentNpcName, ColorSelector::getSelectedColor().color, m_npcSize, positionSouris, CharacterToken::pnj, m_showNpcNumber, m_showNpcName, m_currentNpcNumber);
                pnj->showCharacter();
                m_selectedNpc = pnj;
                m_distanceBetweenMouseAndNPC = m_selectedNpc->mapFromParent(positionSouris);
                emit increaseNpcNumber();
            }
        }
    }
    else
    {
		CharacterToken *pnj = paintCharacter(positionSouris);
        if (pnj)
        {
			if((!m_localIsPlayer)||
               (Map::PC_ALL==m_currentMode)||
                    ((Map::PC_MOVE == m_currentMode)&&(m_localPlayer->getIndexOf(pnj->getCharacterId())>-1)) )//if not found -1
            {
                if (m_currentTool == ToolsBar::DelNpc)
                {
                        if (!pnj->isPc())
                        {
                            if (m_lastSelectedNpc == pnj)
                                m_lastSelectedNpc = 0;

                            NetworkMessageWriter msg(NetMsg::NPCCategory,NetMsg::delNpc);
                            msg.string8(m_mapId);
                            msg.string8(pnj->getCharacterId());
                            msg.sendToServer();

                            pnj->~CharacterToken();
                        }

                }

                else if (m_currentTool == ToolsBar::MoveCharacterToken)
                {
                            m_selectedNpc = pnj;
                            m_distanceBetweenMouseAndNPC = pnj->mapFromParent(positionSouris);
                            m_characterMoveList.clear();
                            m_characterMoveList.append(pnj->getCharacterCenter());
                }
                else if (m_currentTool == ToolsBar::ChangeCharacterState)
                {
                        pnj->changeState();
                        m_lastSelectedNpc = pnj;

                        NetworkMessageWriter msg(NetMsg::CharacterCategory,NetMsg::changeCharacterState);
                        msg.string8(m_mapId);
                        msg.string8(pnj->getCharacterId());
                        msg.uint16(pnj->getHealtState());
                        msg.sendToServer();
                }
                else
                    qWarning() << (tr("undefined tool for processing action on NPC or PC (processNpcAction - map.cpp)"));
            }
    }
}

}


void Map::processNpcActionReleased(QPoint positionSouris)
{
             Q_UNUSED(positionSouris)
    if (m_currentTool == ToolsBar::AddNpc)
    {
        if (ColorSelector::getSelectedColor().type == ColorType)
        {
            if (m_selectedNpc!=nullptr)
            {
                showHideNPC(m_selectedNpc);
                m_lastSelectedNpc = m_selectedNpc;
                m_selectedNpc = 0;
                m_lastSelectedNpc->emettrePnj(m_mapId);
            }
        }
        else
        {
            QMessageBox msgBox(this);
            msgBox.addButton(tr("OK"), QMessageBox::AcceptRole);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setWindowTitle(tr("Inappropriate Color"));
            Qt::WindowFlags flags = msgBox.windowFlags();
            msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);
            msgBox.setText(tr("You can not select this color because\n It's a special color."));
            msgBox.exec();
        }
    }
    
    else if (m_currentTool == ToolsBar::DelNpc)
    {
    }
    
    else if (m_currentTool == ToolsBar::MoveCharacterToken)
    {
        if (m_selectedNpc)
        {
			if((!m_localIsPlayer)||
               (Map::PC_ALL==m_currentMode)||
               ((Map::PC_MOVE == m_currentMode)&&(m_localPlayer->getIndexOf(m_selectedNpc->getCharacterId())>-1)) )
            {
                showHideNPC(m_selectedNpc);
                m_lastSelectedNpc = m_selectedNpc;
                sendCharacterPath();
                m_selectedNpc = nullptr;
            }

        }
    }
    
    else if (m_currentTool == ToolsBar::ChangeCharacterState)
    {
    }

    else
        qWarning() << (tr("undefined tool for processing action on NPC or PC (processNpcActionReleased - map.cpp)"));

}


void Map::processNpcMove(QPoint positionSouris)
{
    if (m_currentTool == ToolsBar::AddNpc)
    {
        if (m_selectedNpc)
        {
            if ( QRect(0, 0, m_backgroundImage->width(), m_backgroundImage->height()).contains(positionSouris, true) )
                m_selectedNpc->moveCharacter(positionSouris - m_distanceBetweenMouseAndNPC);
        }
    }
    
    else if (m_currentTool == ToolsBar::DelNpc)
    {
    }
    
    else if (m_currentTool == ToolsBar::MoveCharacterToken)
    {
        if (m_selectedNpc)
        {
			if((!m_localIsPlayer)||
               (Map::PC_ALL==m_currentMode)||
               ((Map::PC_MOVE == m_currentMode)&&(m_localPlayer->getIndexOf(m_selectedNpc->getCharacterId())>-1)) )
            {
                if ( QRect(0, 0, m_backgroundImage->width(), m_backgroundImage->height()).contains(positionSouris, true) )
                {
                    m_selectedNpc->moveCharacter(positionSouris - m_distanceBetweenMouseAndNPC);
                    m_characterMoveList.append(m_selectedNpc->getCharacterCenter());
                }
            }
        }
    }
    
    else if (m_currentTool != ToolsBar::ChangeCharacterState)
    {
        qWarning() << (tr("undefined tool for processing action on NPC or PC (processNpcMove - map.cpp)"));
    }



}


CharacterToken* Map::paintCharacter(QPoint positionSouris)
{
    CharacterToken *resultat;
    CharacterToken *dessinPnj;
    
    QWidget *widget = childAt(positionSouris);
    
    if (widget)
    {

        if (widget->objectName() == QString("disquePerso"))
        {
            dessinPnj = (CharacterToken *) (widget->parentWidget());
            if (dessinPnj->onTransparentPart(positionSouris))
            {
                dessinPnj->hide();
				resultat = paintCharacter(positionSouris);
                dessinPnj->show();
            }
            else
            {
                 resultat = dessinPnj;
            }
        }
        else if (widget->objectName() == QString("intitulePerso"))
        {
            dessinPnj = (CharacterToken *) (widget->parentWidget());
            dessinPnj->hide();
			resultat = paintCharacter(positionSouris);
            dessinPnj->show();
        }
        else if (widget->objectName() == QString("DessinPerso"))
        {
            dessinPnj = (CharacterToken *) widget;
            dessinPnj->hide();
			resultat = paintCharacter(positionSouris);
            dessinPnj->show();
        }
        else
        {
			qWarning() <<  (tr("unknown widget under cursor (paintCharacter - map.cpp)"));
            resultat = 0;
        }
    }
    else
        resultat = 0;

    return resultat;
}


void Map::showHideNPC(CharacterToken *pnjSeul)
{
    QObjectList enfants;
    int i, j, masque, affiche;
    QPoint contour[8];
    QRect limites = m_backgroundImage->rect();

    if (pnjSeul)
        enfants.append(pnjSeul);
    else
        enfants = children();
    
    int tailleListe = enfants.size();
    
    for (i=0; i<tailleListe; i++)
    {
		CharacterToken* pnj = (CharacterToken *)(enfants[i]);

        if (!pnj->isPc())
        {
            pnj->diskBorder(contour);

            if (pnj->isVisible())
            {
                masque = 0;
                for (j=0; j<8; j++)
                {
                    if (limites.contains(contour[j]))
                    {
                        if ( qAlpha(m_alphaBg->pixel(contour[j].x(), contour[j].y())) == 0)
                            masque++;
                    }
                    else
                        masque++;
                }

                if (masque == 8)
                    pnj->hideCharater();
            }
            else
            {
                affiche = 0;
                for (j=0; j<8 && !affiche; j++)
                {
                    if (limites.contains(contour[j]))
                    {
                        if ( qAlpha(m_alphaBg->pixel(contour[j].x(), contour[j].y())) == 255)
                            affiche++;
                    }
                    else
                        affiche++;
                }
                if (affiche)
                    pnj->showCharacter();
            }
        }    //
    }        //
}


void Map::changePcSize(int nouvelleTaille, bool updatePj)
{
	m_npcSize = nouvelleTaille;
    if((updatePj)&&(m_lastSelectedNpc!=nullptr))
    {
		m_lastSelectedNpc->setPcSize(nouvelleTaille);
    }
		//emit changerm_npcSize(nouvelleTaille);
}

int Map::getPcSize()
{
	return m_npcSize;
}


CharacterToken* Map::findCharacter(QString idPerso)
{
    CharacterToken *perso=nullptr;
    bool ok = false;
    for (int i=0; i<children().size() && !ok; i++)
    {
        perso = dynamic_cast<CharacterToken *>(children().at(i));
        if(nullptr!=perso)
        {
            if (perso->getCharacterId() == idPerso)
            {
                ok = true;
            }
        }

    }

    if (!ok)
        return nullptr;

    return perso;
}

void Map::toggleCharacterView(Character * character)
{
    QString uuid = character->getUuid();
	bool newState = !isVisiblePc(uuid);
	if((!m_localIsPlayer)||(Map::PC_ALL==m_currentMode)||(Map::PC_MOVE == m_currentMode))
    {
		showPc(uuid, newState);

        NetworkMessageWriter message(NetMsg::CharacterPlayerCategory, NetMsg::ToggleViewPlayerCharacterAction);
        message.string8(m_mapId);
        message.string8(uuid);
        message.uint8(newState ? 1 : 0);
        message.sendToServer();
    }

}

void Map::showPc(QString idPerso, bool show)
{
    CharacterToken *pj = findCharacter(idPerso);
    if (nullptr==pj)
    {
        qWarning() << (tr("PC ID %1 not found (showPc - map.cpp)").arg(idPerso));
        return;
    }
    if (show)
    {
        pj->showCharacter();
    }
    else
    {
        pj->hideCharater();
        update(QRect(pj->pos(), pj->size()));
    }
}


bool Map::isVisiblePc(QString idPerso)
{
    CharacterToken *pj = findCharacter(idPerso);
    if (nullptr==pj)
    {
		qWarning() << tr("PC ID: %1 not found (isVisiblePc - map.cpp)") << qPrintable(idPerso);
        return false;
    }

    return pj->isVisible();
}

void Map::addCharacter(Character * person)
{
    new CharacterToken(this, person->getUuid(), person->name(), person->getColor(), m_npcSize, QPoint(m_backgroundImage->width()/2, m_backgroundImage->height()/2), CharacterToken::pj, false, m_showPcName);
}


void Map::eraseCharacter(QString idCharacter)
{
    CharacterToken *perso = findCharacter(idCharacter);
     if (nullptr==perso)
    {
		qWarning() << tr("No character with this id: %1").arg(idCharacter)<<"(eraseCharacter - map.cpp)";
        return;
    }
    perso->~CharacterToken();
}

void Map::delCharacter(Character * person)
{
	if(nullptr==person)
		return;

    CharacterToken * pj = findCharacter(person->getUuid());
    if (pj == nullptr)
    {
        qWarning() << tr("Person %s %s unknown in Carte::changePerson").arg(qPrintable(person->getUuid())).arg(qPrintable(person->name()));
        return;
    }

    delete pj;
}

void Map::changeCharacter(Character * person)
{
	if(nullptr==person)
		return;
    CharacterToken * pj = findCharacter(person->getUuid());
    if (pj == nullptr)
    {
        qWarning() << tr("Person %s %s unknown in Carte::changePerson").arg(person->getUuid()).arg(person->name()) ;
        return;
    }

    pj->renameCharacter(person->name());
    pj->changeCharacterColor(person->getColor());
}


void Map::sendMap(QString titre)
{
    sendOffGlobalMap(titre);
}


void Map::sendMap(QString titre, QString key)
{
    sendOffGlobalMap(titre, key, true);
}


void Map::sendOffGlobalMap(QString titre, QString key, bool versNetworkLinkUniquement)
{
    QByteArray baFondOriginal;
    QBuffer bufFondOriginal(&baFondOriginal);
    if (!m_originalBackground->save(&bufFondOriginal, "jpeg", 70))
    {
        qWarning() << (tr("Codec Error (emettreCarte - map.cpp)"));
    }


    QByteArray baFond;
    QBuffer bufFond(&baFond);

    if (!m_backgroundImage->save(&bufFond, "jpeg", 70))
    {
        qWarning() << (tr("Codec Error (emettreCarte - map.cpp)"));
    }
    QByteArray baAlpha;
    QBuffer bufAlpha(&baAlpha);
    if (!m_alphaLayer->save(&bufAlpha, "jpeg", 100))
    {
        qWarning() << (tr("Codec Error (emettreCarte - map.cpp)"));
    }


    NetworkMessageWriter message(NetMsg::MapCategory, NetMsg::ImportMap);
    message.string16(titre);
    message.string8(m_mapId);
    message.uint8(m_npcSize);
    message.uint8(getPermissionMode());
    message.uint8(getFogColor().red());
    message.byteArray32(baFondOriginal);
    message.byteArray32(baFond);
    message.byteArray32(baAlpha);

    if (versNetworkLinkUniquement)
    {
        QStringList idList;
        idList << key;
        message.setRecipientList(idList,NetworkMessage::OneOrMany);
        message.sendToServer();
    }
    else
    {
        message.sendToServer();
    }
}


void Map::sendOffAllCharacters()
{
    sendOffAllGlobalCharacters();
}


void Map::sendOffAllCharacters(QString idPlayer)
{
    sendOffAllGlobalCharacters(idPlayer, true);
}


void Map::sendOffAllGlobalCharacters(QString idPlayer, bool versNetworkLinkUniquement)
{

    NetworkMessageWriter msg(NetMsg::CharacterCategory,NetMsg::addCharacterList);
    msg.string8(m_mapId);
    msg.uint16(children().size());

    QObjectList enfants = children();
    // Taille de la liste
    int tailleListe = enfants.size();
    for (int i=0; i<tailleListe; i++)
    {
		CharacterToken* character = (CharacterToken *)(enfants[i]);
        character->prepareToSendOff(&msg);
    }

    if (versNetworkLinkUniquement)
    {
        QStringList idList;
        idList << idPlayer;
        msg.setRecipientList(idList,NetworkMessage::OneOrMany);
        msg.sendToServer();
    }
    else
    {
        msg.sendToServer();
    }

}


void Map::sendTrace()
{
    NetworkMessageWriter* msg = nullptr;

    if(ToolsBar::Pen == m_currentTool )
    {
        msg = new NetworkMessageWriter(NetMsg::DrawCategory,NetMsg::penPainting);
        msg->string8(m_localPlayerId);
        msg->string8(m_mapId);
        msg->uint32(m_penPointList.size());
        for (int i=0; i<m_penPointList.size(); i++)
        {
            msg->uint16(m_penPointList[i].x());
            msg->uint16(m_penPointList[i].y());
        }
        msg->uint16(m_penGlobalZone.x());
        msg->uint16(m_penGlobalZone.y());
        msg->uint16(m_penGlobalZone.width());
        msg->uint16(m_penGlobalZone.height());

		msg->uint8(m_penSize);
        msg->uint8(ColorSelector::getSelectedColor().type);
        msg->rgb(ColorSelector::getSelectedColor().color.rgb());
    }
    else if(m_currentTool == ToolsBar::Text)
    {
        msg = new NetworkMessageWriter(NetMsg::DrawCategory,NetMsg::textPainting);
        msg->string8(m_mapId);
		msg->string16(m_currentText);
        msg->uint16(m_mousePoint.x());
        msg->uint16(m_mousePoint.y());

        msg->uint16(m_newZone.x());
        msg->uint16(m_newZone.y());
        msg->uint16(m_newZone.width());
        msg->uint16(m_newZone.height());

        msg->uint8(ColorSelector::getSelectedColor().type);
        msg->rgb(ColorSelector::getSelectedColor().color.rgb());

    }
    else if (m_currentTool == ToolsBar::Handler)
    {
        return;
    }
    else
    {
        NetMsg::Action action;
        bool ok = true;
        switch(m_currentTool)
        {
        case ToolsBar::Line:
            action = NetMsg::linePainting;
            break;
        case ToolsBar::EmptyRect:
            action = NetMsg::emptyRectanglePainting;
            break;
        case ToolsBar::FilledRect:
            action = NetMsg::filledRectanglePainting;
            break;
        case ToolsBar::EmptyEllipse:
            action = NetMsg::emptyEllipsePainting;
            break;
        case ToolsBar::FilledEllipse:
            action = NetMsg::filledEllipsePainting;
            break;
        default:
            ok = false;
            break;
        }
        if(ok)
        {
            msg = new NetworkMessageWriter(NetMsg::DrawCategory,action);
            msg->string8(m_mapId);
            msg->uint16(m_originPoint.x());
            msg->uint16(m_originPoint.y());

            msg->uint16(m_mousePoint.x());
            msg->uint16(m_mousePoint.y());

            msg->uint16(m_newZone.x());
            msg->uint16(m_newZone.y());
            msg->uint16(m_newZone.width());
            msg->uint16(m_newZone.height());

            msg->uint8(m_penSize);

            msg->uint8(ColorSelector::getSelectedColor().type);
            msg->rgb(ColorSelector::getSelectedColor().color.rgb());
        }
    }

    if(nullptr!=msg)
    {
        msg->sendToServer();
    }
}


void Map::sendCharacterPath()
{
    QString idPerso = m_selectedNpc->getCharacterId();
    quint32 sizeList = m_characterMoveList.size();

    NetworkMessageWriter msg(NetMsg::CharacterCategory,NetMsg::moveCharacter);
    msg.string8(m_mapId);
    msg.string8(idPerso);
    msg.uint32(sizeList);
    for (unsigned int i=0; i<sizeList; i++)
    {
        msg.uint16(m_characterMoveList[i].x());
        msg.uint16(m_characterMoveList[i].y());
    }
    msg.sendToServer();
}


void Map::paintPenLine(QList<QPoint> *listePoints, QRect zoneARafraichir, quint8 diametre, SelectedColor couleur, bool joueurLocal)
{
    QPainter painter;
    QColor couleurPinceau;
    if (couleur.type == ColorType)
    {
        painter.begin(m_backgroundImage);
        couleurPinceau = couleur.color;
    }            
    else if (couleur.type == Veil)
    {
        painter.begin(m_alphaLayer);
        couleurPinceau = getFogColor();
    }
    else if (couleur.type == Unveil)
    {
        painter.begin(m_alphaLayer);
        couleurPinceau = Qt::white;
    }
    else if (couleur.type == Erase)
    {
        painter.begin(m_eraseAlpha);
        couleurPinceau = Qt::white;
    }
    else
    {
		qWarning() << tr("Color type is not correct") <<  "(paintPenLine - map.cpp)";
        return;
    }
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen;
    pen.setColor(couleurPinceau);
    if (joueurLocal)
        pen.setWidthF(((qreal)diametre)-0.5);
    else
        pen.setWidth(diametre);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);

    int tailleListe = listePoints->size();
    
    if (tailleListe == 1)
        painter.drawPoint(listePoints->at(0));

    else if (tailleListe == 2)
    {
        if (listePoints->at(0) == listePoints->at(1))
            painter.drawPoint(listePoints->at(0));
        else
            painter.drawLine(listePoints->at(0), listePoints->at(1));
    }
    
    else
        for (int i=1; i<tailleListe; i++)
            painter.drawLine(listePoints->at(i-1), listePoints->at(i));

    if (couleur.type == Erase)
    {
		addAlphaLayer(m_originalBackground, m_eraseAlpha, m_originalBackground, zoneARafraichir);
        QPainter painterFond(m_backgroundImage);
        painterFond.drawImage(zoneARafraichir, *m_originalBackground, zoneARafraichir);
        QPainter painterEfface(m_eraseAlpha);
        painterEfface.fillRect(zoneARafraichir, Qt::black);
    }
    
	addAlphaLayer(m_backgroundImage, m_alphaLayer, m_alphaBg, zoneARafraichir);
    update(/*zoneOrigine.unite(zoneARafraichir)*/);
    showHideNPC();
}


void Map::paintText(QString texte, QPoint positionSouris, QRect zoneARafraichir, SelectedColor couleur)
{
    QPainter painter;
    QColor couleurPinceau;

    if (couleur.type == ColorType)
    {
        painter.begin(m_backgroundImage);
        couleurPinceau = couleur.color;
    }            
    else if (couleur.type == Veil)
    {
        painter.begin(m_alphaLayer);
        couleurPinceau = getFogColor();
    }
    else if (couleur.type == Unveil)
    {
        painter.begin(m_alphaLayer);
        couleurPinceau = Qt::white;
    }
    else if (couleur.type == Erase)
    {
        painter.begin(m_eraseAlpha);
        couleurPinceau = Qt::white;
    }
    else
    {
		qWarning() << tr("Color type is not correct") <<  "(paintText - map.cpp)";
        return;
    }
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen;
    pen.setColor(couleurPinceau);
    painter.setPen(pen);

    QFont font;
    font.setPixelSize(16);
    painter.setFont(font);
    
    painter.drawText(positionSouris, texte);
    
    if (couleur.type == Erase)
    {
		addAlphaLayer(m_originalBackground, m_eraseAlpha, m_originalBackground, zoneARafraichir);
        QPainter painterFond(m_backgroundImage);
        painterFond.drawImage(zoneARafraichir, *m_originalBackground, zoneARafraichir);
        QPainter painterEfface(m_eraseAlpha);
        painterEfface.fillRect(zoneARafraichir, Qt::black);
    }
    
	addAlphaLayer(m_backgroundImage, m_alphaLayer, m_alphaBg, zoneARafraichir);
    update(/*zoneOrigine.unite(zoneARafraichir)*/);
    showHideNPC();
}


void Map::paintOther(NetMsg::Action action, QPoint depart, QPoint arrivee, QRect zoneARafraichir, quint8 diametre, SelectedColor couleur)
{
    QPainter painter;
    QColor couleurPinceau;

    if (couleur.type == ColorType)
    {
        painter.begin(m_backgroundImage);
        couleurPinceau = couleur.color;
    }            
    else if (couleur.type == Veil)
    {
        painter.begin(m_alphaLayer);
        couleurPinceau = getFogColor();
    }
    else if (couleur.type == Unveil)
    {
        painter.begin(m_alphaLayer);
        couleurPinceau = Qt::white;
    }
    else if (couleur.type == Erase)
    {
        painter.begin(m_eraseAlpha);
        couleurPinceau = Qt::white;
    }
    else
    {
		qWarning() << tr("Color type is not correct") <<  "(paintOther - map.cpp)";
        return;
    }
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen;
    pen.setWidth(diametre);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setColor(couleurPinceau);
    painter.setPen(pen);


    if (action == NetMsg::linePainting)
    {
        painter.drawLine(depart, arrivee);
    }
    
    else if (action == NetMsg::emptyRectanglePainting)
    {
        QPoint supGauche, infDroit;
        supGauche.setX(depart.x()<arrivee.x()?depart.x():arrivee.x());
        supGauche.setY(depart.y()<arrivee.y()?depart.y():arrivee.y());
        infDroit.setX(depart.x()>arrivee.x()?depart.x():arrivee.x());
        infDroit.setY(depart.y()>arrivee.y()?depart.y():arrivee.y());

        if ((infDroit.x()-supGauche.x() < diametre) && (infDroit.y()-supGauche.y() < diametre))
        {
            QPoint delta(diametre/2, diametre/2);
            painter.fillRect(QRect(supGauche - delta, infDroit + delta), couleurPinceau);
        }
        else
            painter.drawRect(QRect(supGauche, infDroit));
    }
    
    else if (action == NetMsg::filledRectanglePainting)
    {
        painter.fillRect(QRect(depart, arrivee), couleurPinceau);
    }
    
    else if (action == NetMsg::emptyEllipsePainting)
    {
        QPoint diff = arrivee - depart;
        QPoint nouveauPointOrigine = depart - diff;

        if (abs(arrivee.x()-nouveauPointOrigine.x()) < diametre && abs(arrivee.y()-nouveauPointOrigine.y()) < diametre)
        {
            QPoint supGauche, infDroit;
            supGauche.setX(nouveauPointOrigine.x()<arrivee.x()?nouveauPointOrigine.x():arrivee.x());
            supGauche.setY(nouveauPointOrigine.y()<arrivee.y()?nouveauPointOrigine.y():arrivee.y());
            infDroit.setX(nouveauPointOrigine.x()>arrivee.x()?nouveauPointOrigine.x():arrivee.x());
            infDroit.setY(nouveauPointOrigine.y()>arrivee.y()?nouveauPointOrigine.y():arrivee.y());
            
            QPen pen;
            pen.setColor(couleurPinceau);
            pen.setWidth(0);
            painter.setPen(pen);
            painter.setBrush(couleurPinceau);
            QPoint delta(diametre/2, diametre/2);
            painter.drawEllipse(QRect(supGauche - delta, infDroit + delta));
        }
        else
            painter.drawEllipse(QRect(nouveauPointOrigine, arrivee));
    }
    
    else if (action == NetMsg::filledEllipsePainting)
    {
        QPoint diff = arrivee - depart;
        QPoint nouveauPointOrigine = depart - diff;

        QPen pen;
        pen.setColor(couleurPinceau);
        pen.setWidth(0);
        painter.setPen(pen);
        painter.setBrush(couleurPinceau);
        painter.drawEllipse(QRect(nouveauPointOrigine, arrivee));
    }
            
    else
		qWarning() << (tr("Undefined Tool (paintOther - map.cpp)"));

    if (couleur.type == Erase)
    {
		addAlphaLayer(m_originalBackground, m_eraseAlpha, m_originalBackground, zoneARafraichir);
        QPainter painterFond(m_backgroundImage);
        painterFond.drawImage(zoneARafraichir, *m_originalBackground, zoneARafraichir);
        QPainter painterEfface(m_eraseAlpha);
        painterEfface.fillRect(zoneARafraichir, Qt::black);
    }
    
	addAlphaLayer(m_backgroundImage, m_alphaLayer, m_alphaBg, zoneARafraichir);
    update(/*zoneOrigine.unite(zoneARafraichir)*/);
    showHideNPC();
}

QColor Map::getFogColor()
{
	if(!m_localIsPlayer)
    {
       return PreferencesManager::getInstance()->value("Fog_color",QColor(50,50,50)).value<QColor>();
    }
    else
    {
        return QColor(0,0,0);
    }
}

void Map::adaptAlphaLayer(quint8 intensiteAlpha)
{
    if (intensiteAlpha == getFogColor().red())
        return;
        
    
    qint16 diff = (qint16)(getFogColor().red()) - (qint16)intensiteAlpha;
    int tailleAlpha = m_alphaLayer->width() * m_alphaLayer->height();
    QRgb *pixelAlpha = (QRgb *)m_alphaLayer->bits();

    qint16 nouvelleIntensite;
    for (int i=0; i<tailleAlpha; i++)
    {
        if (qRed(pixelAlpha[i]) != 255)
        {
            nouvelleIntensite = qRed(pixelAlpha[i]) + diff;

            if (nouvelleIntensite < 0)
                nouvelleIntensite = 0;
            else if (nouvelleIntensite > 255)
                nouvelleIntensite = 255;

            pixelAlpha[i] = 0xFF000000 | nouvelleIntensite << 16 | nouvelleIntensite << 8 | nouvelleIntensite;
        }
    }
	addAlphaLayer(m_backgroundImage, m_alphaLayer, m_alphaBg);
}

void Map::startCharacterMove(QString idPerso, QList<QPoint> listePoints)
{
    int i;
    int tailleListe = m_motionList.size();
    bool trouve = false;
    for (i=0; i<tailleListe && !trouve; i++)
        if (m_motionList[i].idCharacter == idPerso)
            trouve = true;

    if (trouve)
        m_motionList[i-1].motion += listePoints;
    else
    {
        CharacterMotion trajetPerso;
        trajetPerso.idCharacter = idPerso;
        trajetPerso.motion = listePoints;
        m_motionList.append(trajetPerso);
        if (m_motionList.size() == 1)
        {
            QTimer::singleShot(10, this, SLOT(moveAllCharacters()));
        }
    }
}


void Map::moveAllCharacters()
{
    QPoint position;
    int i=0;
    while(i < m_motionList.size())
    {

        CharacterToken *perso = findCharacter(m_motionList[i].idCharacter);


        if (nullptr != perso)
        {
            position = m_motionList[i].motion.takeFirst();
            perso->moveCharaterCenter(position);
            showHideNPC(perso);
        }
        if (m_motionList[i].motion.isEmpty() || !perso)
        {
            m_motionList.removeAt(i);
        }
        else
            i++;
    }
    if (!m_motionList.isEmpty())
    {
        QTimer::singleShot(10, this, SLOT(moveAllCharacters()));
    }
}


void Map::saveMap(QDataStream &out, QString titre)
{
    bool ok;
    QByteArray baFondOriginal;
    QBuffer bufFondOriginal(&baFondOriginal);
    ok = m_originalBackground->save(&bufFondOriginal, "jpeg", 100);
    if (!ok)
    {
		qWarning() << tr("Compression issue on original background(saveMap - map.cpp)");
    }


    QByteArray baFond;
    QBuffer bufFond(&baFond);
    ok = m_backgroundImage->save(&bufFond, "jpeg", 100);
    if (!ok)
    {
		qWarning() << tr("Compression issue on background(saveMap - map.cpp)");
    }


    QByteArray baAlpha;
    QBuffer bufAlpha(&baAlpha);
    ok = m_alphaLayer->save(&bufAlpha, "jpeg", 100);
    if (!ok)
    {
		qWarning() << tr("Compression issue on alpha layer (saveMap - map.cpp)");
    }

    out << titre;
    out << pos();
    out << (quint32)m_currentMode;
    out << m_alphaLayer->size();
	out << m_npcSize;
    out << baFondOriginal;
    out << baFond;
    out << baAlpha;




    CharacterToken *perso;
    quint16 nombrePnj = 0;
    QObjectList enfants = children();
    int tailleListe = enfants.size();
    for (int i=0; i<tailleListe; i++)
    {
        perso = (CharacterToken *)(enfants[i]);
        if (perso->isVisible())
            nombrePnj++;
    }


    out << nombrePnj;
    for (int i=0; i<tailleListe; i++)
    {
        perso = (CharacterToken *)(enfants[i]);
        if (perso->isVisible())
        {
            perso->write(out);
        }
    }
}

QString Map::getMapId()
{
    return m_mapId;
}

QString Map::getLastSelectedCharacterId()
{
    if(m_lastSelectedNpc==nullptr)
        return QString();
    return m_lastSelectedNpc->getCharacterId();
}
bool Map::selectCharacter(QString& id)
{
    CharacterToken* tmp=findCharacter(id);
    if(tmp!=nullptr)
        m_lastSelectedNpc=tmp;
    else
        return false;

    return true;
}
void Map::setPermissionMode(Map::PermissionMode mode)
{
    m_currentMode = mode;
    emit permissionModeChanged();
}
Map::PermissionMode Map::getPermissionMode()
{
    return m_currentMode;
}
void Map::setHasPermissionMode(bool b)
{
    m_hasPermissionMode =b;
}
bool Map::hasPermissionMode()
{
    return m_hasPermissionMode;
}

void Map::setPointeur(ToolsBar::SelectableTool currentTool)
{
    m_currentTool = currentTool;
    switch(currentTool)
    {
        case ToolsBar::Pen:
        case ToolsBar::Line:
        case ToolsBar::EmptyRect:
        case ToolsBar::FilledRect:
        case ToolsBar::EmptyEllipse:
        case ToolsBar::FilledEllipse:
            m_mouseCursor = *m_pencilCursor;
        break;
        case ToolsBar::Text:
            m_mouseCursor = *m_textCursor;
        break;
        case ToolsBar::Handler:
            m_mouseCursor =  Qt::OpenHandCursor;
        break;
        case ToolsBar::AddNpc:
            m_mouseCursor =*m_addCursor;
        break;
        case ToolsBar::DelNpc:
            m_mouseCursor =*m_delCursor;
        break;
        case ToolsBar::MoveCharacterToken:
            m_mouseCursor =*m_movCursor;
        break;
        case ToolsBar::ChangeCharacterState:
            m_mouseCursor = *m_stateCursor;
        break;
    }
    setCursor(m_mouseCursor);
}
void Map::setCurrentText(QString text)
{
	m_currentText= text;
}
void Map::setCurrentNpcName(QString text)
{
	m_currentNpcName= text;
}
void Map::setCurrentNpcNumber(int number)
{
	m_currentNpcNumber= number;
}
void Map::setPenSize(int number)
{
	m_penSize=number;
}

void Map::setCharacterSize(int number)
{
	m_npcSize=number;

}
void Map::setNpcNameVisible(bool b)
{
    m_showNpcName = b;
}

void Map::setPcNameVisible(bool b)
{
    m_showPcName =b;
}
void Map::setNpcNumberVisible(bool b)
{
    m_showNpcNumber = b;
}
void Map::setLocalIsPlayer(bool b)
{
	m_localIsPlayer = b;
}
