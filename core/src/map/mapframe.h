/*************************************************************************
 *    Copyright (C) 2007 by Romain Campioni                              *
 *    Copyright (C) 2009 by Renaud Guezennec                             *
 *    Copyright (C) 2011 by Joseph Boudou                                *
 *                                                                       *
 *      https://rolisteam.org/                                        *
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

#ifndef CARTE_FENETRE_H
#define CARTE_FENETRE_H

#include <QAction>
#include <QImage>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPoint>
#include <QPointer>
#include <QScrollArea>
#include <QWidget>
#include <memory>

#include "data/mediacontainer.h"

class Map;
class MapController;
/**
 * @brief The BipMapWindow class - is the scroll area which manages the display of map.
 */
class MapFrame : public MediaContainer
{
    Q_OBJECT

public:
    /**
     * @brief BipMapWindow
     * @param uneCarte - the embedded map
     * @param parent - parent QWidget
     */
    MapFrame(MapController* ctrl, QWidget* parent= nullptr);
    /**
     *
     */
    virtual ~MapFrame() override;
    /**
     * @brief carte
     * @return
     */
    Map* getMap();
    /**
     * @brief setMap
     */
    void setMap(Map*);
    /**
     * @brief getMapId
     * @return
     */
    virtual QString getMediaId() const override;

    /**
     * @brief setCleverUri
     * @param uri
     */
    bool readFileFromUri();
    /**
     * @brief openMedia
     */
    bool openMedia() override;
    /**
     * @brief createMap
     * @return
     */
    bool createMap();
    /**
     * @brief processMapMessage
     * @param msg
     * @return
     */
    bool processMapMessage(NetworkMessageReader* msg, bool localIsPlayer);
    /**
     * @brief readMapAndNpc
     * @param in
     * @param hidden
     * @param nomFichier
     * @return
     */
    bool readMapAndNpc(QDataStream& in, bool hidden= false);
    /**
     * @brief saveMedia
     */
    void saveMedia() override;
    /**
     * @brief putDataIntoCleverUri
     */
    void putDataIntoCleverUri() override;

    virtual void fill(NetworkMessageWriter& msg) override;
    virtual void readMessage(NetworkMessageReader& msg) override;

    ContainerType getContainerType() const;
    void setContainerType(const ContainerType& containerType);

signals:
    /**
     * @brief activated
     * @param carte
     */
    void activated(Map* getMap);

    /**
     * @brief notifyUser
     * @param str
     */
    void notifyUser(QString str);

public slots:
    /**
     * @brief commencerDeplacement
     * @param position
     */
    void startMoving(QPoint position);
    /**
     * @brief deplacer
     * @param position
     */
    void moveMap(QPoint position);

protected:
    /**
     * @brief focusInEvent
     * @param event
     */
    void focusInEvent(QFocusEvent* event) override;
    /**
     * @brief initMap
     */
    void initMap();
    /**
     * @brief openUriAndLoadMap - open file and call method to read it and sent it over network.
     * @param uri
     * @return true the reading was succesfull, false otherwise.
     */
    bool openUriAndLoadMap(QString uri);
    virtual void updateTitle() override;

private:
    QPointer<MapController> m_mapCtrl;
    std::unique_ptr<Map> m_map;
    std::unique_ptr<QScrollArea> m_widgetArea;

    QPoint pointDepart;
    int horizontalDepart;
    int verticalDepart;

    QColor m_color;
};

#endif
